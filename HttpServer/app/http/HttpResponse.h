#ifndef HTTPRESPONSE_H
#define HTTPRESPONSE_H

#include "../../net/StringPiece.h"

// #include <string>
#include <map>

class Buffer;

class HttpResponse{
public:
    enum HttpStatusCode{
        kUnknown,
        k200Ok = 200,
        k301MovePermanently = 301,
        k400BadRequest = 400,
        k404NotFound = 404
    };

    explicit HttpResponse(bool close)
        :statusCode_(kUnknown),
        closeConnection_(close){

    }

    void setStatusCode(HttpStatusCode code){
        statusCode_ = code;
    }

    void setStatusMessage(const StringPiece& message){
        statusMessage_ = message;
    }

    void setCloseConnection(bool on){
        closeConnection_ = on;
    }
    bool getCloseConnection()const{
        return closeConnection_;
    }

    void setContentType(const StringPiece& contentType){
        addHeader("Content-Type", contentType);
    }

    void addHeader(const StringPiece& key, const StringPiece& value){
        headers_[key] = value;
    }

    void setBody(const StringPiece& body){
        body_ = body;
    }

    void appendToBuffer(Buffer* output) const;

private:
    std::map<StringPiece, StringPiece> headers_;
    HttpStatusCode statusCode_;
    StringPiece statusMessage_;
    bool closeConnection_;
    StringPiece body_;
};

#endif

