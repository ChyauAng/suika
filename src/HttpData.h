#ifndef _HTTPDATA_H_
#define _HTTPDATA_H_

#include <algorithm>
#include <map>
#include <memory>
#include <string>

#include "base/notCopyable.h"
#include "base/Timestamp.h"

class Buffer;
class TcpContext;

class HttpData:public notCopyable{
public:
    HttpData();
    ~HttpData();

    void setHolder(std::shared_ptr<TcpContext> holder);

    // callback functions
    // 解析请求报文,并生成响应报文
    void onRequest(Buffer*);

    // request part
    enum Method{
        kInvalid, kGet, kPost, kHead, kPut, kDelete
    };

    enum Version{
        kUnknownVersion, kHttp10, kHttp11
    };

    void setVersion(Version v){
        version_ = v;
    }
    Version getVersion()const {
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
            method_ = kDelete;
        }
        else if(m == "PUT"){
            method_ = kPut;
        }
        else if(m == "DELETE"){
            method_ = kDelete;
        }
        else{
        }
        
        return method_ != kInvalid;
    }
    Method getMethod() const{
        return method_;
    }

    void setPath(const char* start, const char* end){
        path_.assign(start, end);
    }
    const std::string& getPath()const{
        return path_;
    }

    void setQuery(const char* start, const char* end){
        query_.assign(start, end);
    }
    const std::string& getQuery()const{
        return query_;
    }

    void setReceiveTime(Timestamp t){
        receiveTime_ = t;
    }
    Timestamp getReceiveTime()const{
        return receiveTime_;
    }

    void addReqHeader(const char* start, const char* colon, const char* end){
    std::string field(start, colon);
    ++colon;
    while(colon < end && isspace(*colon)){
        ++colon;
    }
    std::string value(colon, end);
    while(!value.empty() && isspace(value[value.size() - 1])){
        value.resize(value.size() - 1);
    }
    requestHeaders_[field] = value;
    }
    std::string getAReqHeader(const std::string& field)const{
        std::string result;
        std::map<std::string, std::string>::const_iterator it = requestHeaders_.find(field);
        if(it != requestHeaders_.end()){
            result = it->second;
        }
        return result;
    }
    const std::map<std::string, std::string>& getReqHeaders()const{
        return requestHeaders_;
    }

    // context part
    enum HttpRequestParseState{
        kExpectRequestLine,
        kExpectHeaders,
        kExpectBody,
        kGotAll
    };

    bool parseRequest(Buffer* buf, Timestamp receiveTime);

    bool isGotAll()const{
        return state_ == kGotAll;
    }

    // response part
    enum HttpStatesCode{
        kUnknownState,
        k200Ok = 200,
        k301MovePermanently = 301,
        k400BadRequest = 400,
        k404NotFound = 404
    };

    void setStatusCode(HttpStatesCode code){
        statusCode_ = code;
    }
    void setStatusMessage(const std::string& message){
       statusMessage_ = message;
    }

    void setCloseConnection(bool on){
        closeConnection_ = on;
    }
    bool getCloseConnection() const{
        return closeConnection_;
    }

    void setContentType(const std::string& contentType){
        addARepHeader("Content-Type", contentType);
    }

    void addARepHeader(const std::string& key, const std::string& value){
        responseHeaders_[key] = value;
    }
    
    void setBody(const std::string& body){
        body_ = body;
    }

    void appendToBuffer(Buffer* output)const;

    void clearAllContent(){
        requestHeaders_.clear();
        responseHeaders_.clear();

        closeConnection_ = false;
        method_ = kInvalid;
        version_ = kUnknownVersion;
        state_ = kExpectRequestLine;
        statusCode_ = kUnknownState;
    }





private:
    bool processRequestLine(const char* begin, const char* end);

    bool closeConnection_;

    Method method_;
    Version version_;
    HttpRequestParseState state_;
    HttpStatesCode statusCode_;


    std::string path_;
    std::string query_;
    std::string statusMessage_;
    std::string body_;
    // HttpRequest, HttpContext, HttpResponse...
    std::weak_ptr<TcpContext> holder_;

    Buffer buf_;
    
    std::map<std::string, std::string> requestHeaders_;
    std::map<std::string, std::string> responseHeaders_;

    Timestamp receiveTime_;
   
    static char favicon[555];

};



#endif
