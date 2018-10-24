#ifndef _FILEUTIL_H_
#define _FILEUTIL_H_

#include "../notCopyable.h"
#include "../StringPiece.h"

class AppendFile: public notCopyable{
public:
    explicit AppendFile(StringArg filename);
    ~AppendFile();

    void append(const char* logline, const size_t len);
    void flush();
    int writtenBytes() const{
        return writtenBytes_;
    }

private:
    size_t write(const char* logline, size_t len);
    
    FILE* fp_;
    char buffer_[64 * 1024];
    int writtenBytes_;
};

#endif


