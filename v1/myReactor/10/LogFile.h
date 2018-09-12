#ifndef _LOGFILE_H_
#define _LOGFILE_H_

#include "Mutex.h"
#include "notCopyable.h"

#include <memory>
#include <string>

class AppendFile;

class LogFile: public notCopyable{
public:
    LogFile(const std::string& basename, int rollSize, bool threadSafe = true, int flushInterval = 3, int checkEvenyN = 2014);
    ~LogFile();

    void append(const char* logFile, int len);
    void flush();
    bool rollFile();

private:
    void append_unlocked(const char* logline, int len);
    static std::string getLogFileName(const std::string basename, time_t* now);

    const std::string basename_;
    const int rollSize_;
    const int flushInterval_;
    const int checkEveryN_;
    
    int count_;

    const std::unique_ptr<MutexLock> mutex_;
    std::unique_ptr<AppendFile> file_;

    time_t startOfPeriod_;
    time_t lastRoll_;
    time_t lastFlush_;
    const static int kRollPerSeconds_ = 60 * 60 * 24;
};

#endif
