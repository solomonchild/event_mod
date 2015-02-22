#include <cstdint>
#include <cstdlib>
#include <ctime>
#include <queue>
#include <iostream>
#include <memory>
#include <string>

enum class Type
{
    TReq1, TReq2
};

struct Req
{

    Req(Type type, unsigned t)
    {
        type_ = type;
        time_ = t;
    }

    unsigned time_;
    Type type_;
};



struct ReqFactory
{
    ReqFactory(uint16_t maxrand)
    : maxrand_(maxrand)    
    {
        srand(time(NULL));
    }

    std::shared_ptr<Req> GetNext(Type t)
    {
        unsigned next = rand() % 100;
        auto ptr = std::shared_ptr<Req>(new Req(t, next)); 
        return ptr;
    }
protected:
    uint16_t maxrand_;
};


template<class T>
class Queue
{
public:
    Queue(unsigned max = 10)
    : elems_(0),
    max_(max)
    {

    }

    bool IsEmpty()
    {
        return elems_ == 0;
    }

    bool IsFull()
    {
        return elems_ == max_;
    }

    T Deq()
    {
        if(elems_ < 0 || q_.size == 0)
            throw nullptr;

        return q_.pop();
    }

    void Enq(T val)
    {
        if(elems_ >= max_)
            return;

        q_.push(val);
        elems_++;
    }

private:
    std::queue<T> q_;
    size_t elems_;
    size_t max_;

};

class Server
{
public:
    Server()
    : isBusy_(false),
    randMax_(20),
    timeToProcess_(0)
    {
        srand(time(NULL));
    }

    bool Serve(unsigned currTime)
    {
        if(isBusy_)
            return false;

        timeToProcess_ = currTime + (rand() % randMax_);
        isBusy_ = true;

        return true;
    }

    unsigned GetTimeToProcess()
    {
        return timeToProcess_;
    }
    
    bool IsBusy(unsigned currTime)
    {
       if(timeToProcess_ >= currTime)
       {
           isBusy_ = false;
           timeToProcess_ = 0;
       }
       return isBusy_;
    }

protected:
    bool isBusy_;
    unsigned randMax_;
    unsigned timeToProcess_;
};



int main(int argc, char **argv)
{
    uint32_t timeEnd_ = 500, t_ = 0, h_ = 0, e1_ = 0, e2_ = 0;
    ReqFactory fac_(100);
    Queue<Type> q_;
    Server s_;

    auto type_ = Type::TReq1;
    
    auto req = fac_.GetNext(type_);

    while(t_ < timeEnd_)
    {
        if(h_ < e1_ && h_ < e2)
        {
            t_ = h_;
            if(!s_.IsBusy(t_))
                throw nullptr;

            if(!q_.IsEmpty())
            {
                h_ = s_.Serve();
                Type t = q_.Deq();
                if(t == TReq1)
                {
                    
                }
            }
        }
        else
        {
            if(e1_ < e2_)
            {
                t_ = e1_;
                type_ = Type::TReq1;
            }
            else
            {
                t_ = e2_;
                type_ = Type::TReq2;
            }

            if(!q_.IsEmpty())
            {
                if(q_.IsFull())
                    throw nullptr;
                q_.Enq(req.type_);
            }


        }

        if(!s.IsBusy(t_))
        {
            req = fac_.GetNext(type_);
            t_ = req->time_;
            s_.Serve(t_);
            h_ = s_.GetTimeToProcess();
        }
        else if(e1_ <= t_ || e2_ <= t_)
        {
            if(q_.IsFull())
                throw nullptr; 

            q_.Enq(req.type_);
        }

        std::cout << "Current time: " << t_ << " next 1st " << e1_ << " next 2nd " << e2_<< " next processed time " << h_ << std::endl;
    }
}
