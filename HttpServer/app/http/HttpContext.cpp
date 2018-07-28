#include "../../net/Buffer.h"
#include "HttpContext.h"

// take http request message format as reference 
bool HttpContext::processRequestLine(const char* begin, const char* end){
    bool succeed = false;
    const char* start = begin;
    const char* space = std::find(start, end, ' ');
    if(space != end && request_.setMethod(start, space)){
        start = space + 1;
        space = std::find(start, end, ' ');
        if(space != end){
            const char* queryPos = std::find(start, space, '?');
            if(queryPos != space){
                request_.setQuery(queryPos, space);
            }
            request_.setPath(start, queryPos);
            start = space + 1;
            succeed = ((end-start == 8) && (std::equal(start, end -1, "HTTP/1.")));
            if(succeed){
                if(*(end - 1) == '1'){
                    request_.setVersion(HttpRequest::kHttp11);
                }
                else if(*(end - 1) == '0'){
                    request_.setVersion(HttpRequest::kHttp10);
                }
                else{
                    succeed = false;
                }
            }
        }
    }
    return succeed;
}

bool HttpContext::parseRequest(Buffer* buf, Timestamp receiveTime){
    bool ok = true;
    bool hasMore = true;
    while(hasMore){
        if(state_ == kExpectRequestLine){
            const char* crlf = buf->findCRLF();
            if(crlf){
                ok = processRequestLine(buf->peek(), crlf);
                if(ok){
                    request_.setReceiveTime(receiveTime);
                    // pass the /r/n part
                    buf->retrieveUntil(crlf + 2);
                    state_ = kExpectHeaders;
                }
                else{
                    hasMore = false;
                }
            }
            else{
                hasMore = false;
            }
        }
        else if(state_ == kExpectHeaders){
            const char* crlf = buf->findCRLF();
            if(crlf){
                const char* colon = std::find(buf->peek(), crlf, ':');
                if(colon != crlf){
                    request_.addHeader(buf->peek(), colon, crlf);
                }
                else{
                    // the empty line, the following is Body part 
                    state_ = kGotAll;
                    hasMore = false;
                }
                // pass the /r/n part
                buf->retrieveUntil(crlf + 2);
            }
            else{
                hasMore = false;
            }
        }
        else if(state_ == kExpectBody){

        }
    }
    return ok;
}
