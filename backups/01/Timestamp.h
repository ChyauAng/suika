#ifndef _TIMRSTAMP_H_
#define _TIMESTAMP_H_

#include <inttypes.h>

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

    static Timestamp now();
    static const int kMicroSecondsPerSecond = 1000 * 1000;

private:
    int64_t microSecondsSinceEpoch_;
    
};
#endif
