#ifndef _NOT_COPYABLE_H_
#define _NOT_COPYABLE_H_

class notCopyable{
protected:
    notCopyable(){};
    ~notCopyable(){};

private:
    notCopyable(const notCopyable&);
    notCopyable& operator=(const notCopyable&);
};

#endif
