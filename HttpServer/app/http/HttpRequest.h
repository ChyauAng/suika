#ifndef HTTPREQUEST_H
#define HTTPREQUEST_H

#include "../../net/Timestamp.h"

#include <map>
#include <assert.h>
#include <stdio.h>
#include <string>

class HttpRequest{
public:
    enum Method{
        kInvalid, kGet, kPost, kHead, kPut, kDelete
    };

    enum Version{
        kUnknown, kHttp10, kHttp11
    };

    HttpRequest()
        :method_(kInvalid),
        version_(kUnknown){

    }

    void setVersion(Version v){
        version_ = v;
    }
    Version getVersion() const{
        return version_;
    }

    bool setMethod(const char* start, const char* end){
        assert(method_ == kInvalid);
        std::string m(start, end);

        if(m == "GET"){
            method_ = kGet;
        }
        else if(m == "POST"){
            method_ = kPost;
        }
        else if(m == "HEAD"){
            method_ = kHead;
        }
        else if(m == "PUT"){
            method_ = kPut;
        }
        else if(m == "DELETE"){
            method_ = kDelete;
        }
        else{
            method_ = kInvalid;
        }
        return method_ != kInvalid;
    }
    Method getMethod() const{
        return method_;
    }

    void setPath(const char* start, const char* end){
        path_.assign(start, end);
    }
    const std::string& getPath() const{
        return path_;
    }

    void setQuery(const char* start, const char* end){
        query_.assign(start, end);
    }
    const std::string& getQuery() const{
        return query_;
    }

    void setReceiveTime(Timestamp t){
        receiveTime_ = t;
    }
    Timestamp getReceiveTime() const{
        return receiveTime_;
    }
    
    // process (key, value) pairs.
    void addHeader(const char* start, const char* colon, const char* end){
        std::string field(start, colon);
        ++colon;
        while(colon < end && isspace(*colon)){
            ++colon;
        }
        std::string value(colon, end);
        while(!value.empty() && isspace(value[value.size() - 1])){
            value.resize(value.size() - 1);
        }
        headers_[field] = value;
    }
    
    std::string getAHeader(const std::string& field) const{
        std::string result;
        std::map<std::string, std::string>::const_iterator it = headers_.find(field);
        if(it != headers_.end()){
            result = it->second;
        }
        return result;
    }

    const std::map<std::string, std::string>& getHeaders() const{
        return headers_;
    }

    void swap(HttpRequest& hr){
        std::swap(method_, hr.method_);
        std::swap(version_, hr.version_);
        path_.swap(hr.path_);
        query_.swap(hr.query_);
        receiveTime_.swap(hr.receiveTime_);
        headers_.swap(hr.headers_);
    }

private:
    Method method_;
    Version version_;
    std::string path_;
    std::string query_;
    Timestamp receiveTime_;
    std::map<std::string, std::string> headers_;
};

#endif



    

