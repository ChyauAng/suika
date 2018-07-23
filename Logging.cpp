#include "Logging.h"
#include "CurrentThread.h"

#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <stdlib.h>
#include <sstream>

__thread char t_errnobuf[512];
__thread char t_time[64];
__thread time_t t_lastSecond;

const char* strerror_tl(int savedErrno){
    // strerror_r(3):get and record the error message
    // strerror(3) is not thread safe, strerror_r(3) is thread safe
    return strerror_r(savedErrno, t_errnobuf, sizeof t_errnobuf);
}


Logger::LogLevel initLogLevel(){
    if(getenv("LOG_TRACE")){
        return Logger::TRACE;
    }
    else if(getenv("LOG_DEBUG")){
        return Logger::DEBUG;
    }
    else
        return Logger::INFO;
}

Logger::LogLevel g_logLevel = initLogLevel();

const char* LogLevelName[Logger::NUM_LOG_LEVELS] = {
    "TRACE ", "DEBUG ", "INFO  ", "WARN  ", "ERROR ", "FATAL ", };

// helper class for known string length at compile time
class T{
public:
    T(const char* str, unsigned len)
        :str_(str),
        len_(len){
            assert(strlen(str) == len_);
        }

    const char* str_;
    const unsigned len_;
};

inline LogStream& operator<<(LogStream& s, T v){
    s.append(v.str_, v.len_);
    return s;
}

inline LogStream& operator<<(LogStream& s, const Logger::SourceFile& v){
    s.append(v.data_, v.size_);
    return s;
}

void defaultOutput(const char* msg, int len){
    size_t n = fwrite(msg, 1, len, stdout);
    (void) n;
}

void defaultFlush(){
    fflush(stdout);
}

Logger::OutputFunc g_output = defaultOutput;
Logger::FlushFunc g_flush = defaultFlush;

Logger::Impl::Impl(LogLevel level, int savedErrno, const SourceFile& file, int line)
    :time_(Timestamp::now()),
    stream_(),
    level_(level),
    line_(line),
    basename_(file){
        formatTime();
        CurrentThread::tid();
        stream_ << T(CurrentThread::tidString(), CurrentThread::tidStringLength());
        // 6: L35 set this value
        stream_ << T(LogLevelName[level], 6);
        if(savedErrno != 0){
            stream_ << strerror_tl(savedErrno) << " (errno=" << savedErrno << ") ";
        }
    }

void Logger::Impl::formatTime(){
    int64_t microSecondsSinceEpoch = time_.microSecondsSinceEpoch();
    time_t seconds = static_cast<time_t>(microSecondsSinceEpoch / Timestamp::kMicroSecondsPerSecond);
    int microseconds = static_cast<int>(microSecondsSinceEpoch % Timestamp::kMicroSecondsPerSecond);

    if(seconds != t_lastSecond){
        t_lastSecond = seconds;
        struct tm tm_time;
        gmtime_r(&seconds, &tm_time);

        int len = snprintf(t_time, sizeof(t_time), "%4d%02d%02d %02d:%02d:%02d", tm_time.tm_year + 1900, tm_time.tm_mon + 1, tm_time.tm_mday, tm_time.tm_hour, tm_time.tm_min, tm_time.tm_sec);
        assert(17 == len);
        (void) len;
    }

}

void Logger::Impl::finish(){
    stream_ << " - " << basename_ << ':' << line_ << '\n';
}

Logger::Logger(SourceFile file, int line)
    :impl_(INFO, 0, file, line){
    
}

Logger::Logger(SourceFile file, int line, LogLevel level)
    :impl_(level, 0, file, line){

}

Logger::Logger(SourceFile file, int line, LogLevel level, const char* func)
    :impl_(level, 0, file, line){
        impl_.stream_ << func << ' ';
}


Logger::Logger(SourceFile file, int line, bool toAbort)
    :impl_(toAbort ? FATAL : ERROR, errno, file, line){

}

Logger::~Logger(){
    impl_.finish();
    const LogStream::Buffer& buf(stream().buffer());
    g_output(buf.data(), buf.length());
    if(impl_.level_ == FATAL){
        g_flush();
        abort();
    }
}

void Logger::setLogLevel(Logger::LogLevel level){
    g_logLevel = level;
}

void Logger::setOutput(OutputFunc out){
    g_output = out;
}

void Logger::setFlush(FlushFunc flush){
    g_flush = flush;
}


