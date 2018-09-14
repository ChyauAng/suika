#ifndef _HTTPDATA_H_
#define _HTTPDATA_H_

#include "base/notCopyable.h"

class HttpData:public notCopyable{
public:
    HttpData();
    ~HttpData();
private:
    // HttpRequest, HttpContext, HttpResponse...

};



#endif
