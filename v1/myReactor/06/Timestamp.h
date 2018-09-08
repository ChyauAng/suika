#ifndef _TIMESTAMP_H_
#define _TIMESTAMP_H_

#include <inttypes.h>
#include <string>

class Timestamp{
public:
    Timestamp():
        microSecondsSinceEpoch_(0){

    }

    explicit Timestamp(int64_t microSecondsSinceEpochArg)
        :microSecondsSinceEpoch_(microSecondsSinceEpochArg){
        
        }

    ~Timestamp(){

    }
    
    bool operator<(const Timestamp& rhs)const{
        return this->microSecondsSinceEpoch() < rhs.microSecondsSinceEpoch();
    }

    bool operator==(const Timestamp& rhs)const{
        return this->microSecondsSinceEpoch() == rhs.microSecondsSinceEpoch();
    }


    std::string toString() const;
    
    bool valid() const{
        return microSecondsSinceEpoch_ > 0;
    }

    // get function
    int64_t microSecondsSinceEpoch() const{
        return microSecondsSinceEpoch_;
    }

    static Timestamp now();
    static Timestamp invalid(){
        return Timestamp();
    }

    static const int kMicroSecondsPerSecond = 1000 * 1000;

private:
    int64_t microSecondsSinceEpoch_;
    
};

inline Timestamp addTime(Timestamp timestamp, double seconds){
    int64_t delta = static_cast<int64_t>(seconds * Timestamp::kMicroSecondsPerSecond);
    return Timestamp(timestamp.microSecondsSinceEpoch() + delta);
}


#endif
