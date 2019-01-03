#ifndef HTTPCONTEXT_H
#define HTTPCONTEXT_H

#include "HttpRequest.h"

class Buffer;

class HttpContext{
public:
    enum HttpRequestParseState{
        kExpectRequestLine,
        kExpectHeaders,
        kExpectBody, 
        kGotAll
    };

    HttpContext()
        :state_(kExpectRequestLine){

    }

    // use default c-ctor && operator= as data members are well-defined 

    bool parseRequest(Buffer* buf, Timestamp receiveTime);

    bool isGotAll() const{
        return state_ == kGotAll;
    }

    void reset(){
        state_ = kExpectRequestLine;
        HttpRequest dummy;
        request_.swap(dummy);
    }

    const HttpRequest& getRequest() const{
        return request_;
    }
    HttpRequest& getRequest(){
        return request_;
    }

private:
    bool processRequestLine(const char* begin, const char* end);

    HttpRequestParseState state_;
    HttpRequest request_;
};

#endif
