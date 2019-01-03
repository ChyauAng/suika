#include "LogFile.h"
#include "FileUtil.h"

#include <assert.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>

LogFile::LogFile(const std::string& basename, int rollSize, bool threadSafe, int flushInterval, int checkEveryN)
    :basename_(basename),
    rollSize_(rollSize),
    flushInterval_(flushInterval),
    checkEveryN_(checkEveryN),
    count_(0),
    mutex_(threadSafe ? new MutexLock : NULL),
    startOfPeriod_(0),
    lastRoll_(0),
    lastFlush_(0){
        assert(basename.find('/') == std::string::npos);
        rollFile();
}

LogFile::~LogFile(){

}

void LogFile::append(const char* logline, int len){
    if(mutex_){
        MutexLockGuard lock(*mutex_);
        append_unlocked(logline, len);
    }
    else{
        append_unlocked(logline, len);
    }
}


void LogFile::flush(){
    if(mutex_){
        MutexLockGuard lock(*mutex_);
        file_->flush();
    }
    else{
        file_->flush();
    }
}

void LogFile::append_unlocked(const char* logline, int len){
    file_->append(logline, len);
    if(file_->writtenBytes() > rollSize_){
        rollFile();
    }
    else{
        ++count_;
        if(count_ >= checkEveryN_){
            count_ = 0;
            time_t now = time(NULL);
            time_t thisPeriod = now / kRollPerSeconds_ * kRollPerSeconds_;
            if(thisPeriod != startOfPeriod_){
                rollFile();
            }
            else if(now - lastFlush_ > flushInterval_){
                lastFlush_ = now;
                file_->flush();
            }
        }
    }
}

// log file roll back
bool LogFile::rollFile(){
    time_t now = 0;
    std::string filename = getLogFileName(basename_, &now);
    time_t start = now / kRollPerSeconds_ * kRollPerSeconds_;

    if(now > lastRoll_){
        lastRoll_ = now;
        lastFlush_= now;
        startOfPeriod_ = start;
        file_.reset(new AppendFile(filename));  // reset directly
        return true;
    }
    return false;
}

std::string hostname(){
    char buf[256];  // HOST_NAME_MAX 64, _POSIX_HOST_NAME_MAX 255
    if(gethostname(buf, sizeof buf) == 0){
        buf[sizeof(buf) - 1] = '\0';
        return buf;
    }
    else{
        return "unkownhost";
    }
}

// nameOfProcess(basename).fileCreateTime(YMD-HMS).hostname.idOfProcess.log
std::string LogFile::getLogFileName(const std::string basename, time_t* now){
    std::string filename;
    filename.reserve(basename.size() + 64);
    filename = basename;
    char timebuf[32];
    struct tm tm;
    *now = time(NULL);
    gmtime_r(now, &tm);
    strftime(timebuf, sizeof timebuf, ".%Y%m%d-%H%M%S.", &tm);
    filename += timebuf;
    filename += hostname();
    char pidbuf[32];
    snprintf(pidbuf, sizeof pidbuf, ".%d", CurrentThread::pid());
    filename += pidbuf;
    filename += ".log";
    return filename;
}

