#include "HttpResponse.h"
#include "../../net/Buffer.h"

#include <stdio.h>

// take http response message format as reference
void HttpResponse::appendToBuffer(Buffer* output) const{
    StringPiece crlf("\r\n");
    char buf[32];
    snprintf(buf, sizeof buf, "HTTP/1.1 %d", statusCode_);
    output->append(StringPiece(buf));
    output->append(statusMessage_);
    output->append(crlf);

    if(closeConnection_){
        output->append(StringPiece("Connection: close\r\n"));
    }
    else{
        snprintf(buf, sizeof buf, "Content-Length: %zd\r\n", static_cast<size_t>(body_.size()));
        output->append(StringPiece(buf));
        output->append(StringPiece("Connection: Keep-Alive\r\n"));
    }

    for(std::map<StringPiece, StringPiece>::const_iterator it = headers_.begin(); it != headers_.end(); it++){
        output->append(it->first);
        output->append(StringPiece(": "));
        output->append(it->second);
        output->append(crlf);
    }

    output->append(crlf);
    output->append(body_);
}
