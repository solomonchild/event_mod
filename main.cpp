#include <cstdint>
#include <cstdlib>
#include <ctime>
#include <queue>
#include <iostream>
#include <memory>
#include <string>
#include <cassert>

template<class T>
void LOG(T arg, std::string msg)
{
    std::cout << "LOG: " << msg << " " << arg << " \n";
}

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
        if(elems_ < 0 || q_.size() == 0)
            throw nullptr;
        auto ret = q_.front();
        q_.pop();
        return ret;
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

        timeToProcess_ = currTime + ((rand() + 1) % randMax_);
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
    uint32_t timeEnd_ = 500, t_ = 0, h_ = timeEnd_ + 1, e1_ = 0, e2_ = 0;
    ReqFactory fac_(100);
    Queue<Type> q_;
    Server s_;

    auto type_ = Type::TReq1;
    
    auto req = fac_.GetNext(type_);
    e1_ = req->time_;
    e2_ = fac_.GetNext(Type::TReq2)->time_;

    while(t_ < timeEnd_)
    {
        if(h_ < e1_ && h_ < e2_)
        //next event is server finishing with current request 
        {
            //set timer to that time
            t_ = h_;
            LOG(t_, "Server finished, new current time: ");

            //Server MUST be busy at this moment
            assert(!s_.IsBusy(t_));

            //deque last element in a queue
            //and tell server to process
            if(!q_.IsEmpty())
            {
                s_.Serve(t_);
                h_ = s_.GetTimeToProcess();
            }
            else
            {
                //otherwise, set h_ to unreachable time
                h_ = timeEnd_ + 1;
            }
        }
        else
        {
            //next event is either of requests coming in
            if(e1_ < e2_)
            {
                t_ = e1_;
                LOG(t_, "e1 arrived @");
                type_ = Type::TReq1;
            }
            else
            {
                t_ = e2_;
                LOG(t_, "e2 arrived @");
                type_ = Type::TReq2;
            }

            if(s_.IsBusy(t_))
            {
                //Server is busy
                //try to enqueue

                if(!q_.IsEmpty())
                {
                    if(q_.IsFull())
                        throw nullptr;

                    q_.Enq(req->type_);


                }
            }
            else
            {
                //Server is not busy
                // no queing
                s_.Serve(t_);
                h_ = s_.GetTimeToProcess();

                LOG(h_, "Server is now busy, will finish serving @");
            }
            //get time for arrival of the request
            //of the consumed request type
            req = fac_.GetNext(type_);
            if(type_ == Type::TReq1)
                e1_ = t_ + req->time_;
            else
                e2_ = t_ + req->time_;
        }

        std::cout << "Current time: " << t_ << " e1: " << e1_ << " e2: " << e2_<< " will process @: " << h_ << std::endl;
    }
}
