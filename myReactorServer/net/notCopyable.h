#include"config.h"

#ifndef _NOT_COPYABLE_H
#define _NOT_COPYABLE_H
class notCopyable{
protected:
    notCopyable(){};
    ~notCopyable(){};

private:
    notCopyable(const notCopyable&);
    notCopyable& operator=(const notCopyable&);

};

#endif
