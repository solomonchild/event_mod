#include <cstdint>
#include <cstdlib>
#include <ctime>
#include <queue>
#include <iostream>
#include <memory>
#include <string>
#include <cassert>

#include <sstream>


class Logger
{
    protected:
        std::ostringstream os;
    public:
        std::ostringstream& get()
        {
            return os;
        }
        ~Logger()
        {
            std::cout << os.str() << std::endl;
        }

};


enum class Type
{
    TReq1, TReq2
};

struct Req
{

    Req(Type type, unsigned t)
    {
        type = type;
        time = t;
    }

    unsigned time;
    Type type;
};



struct ReqFactory
{
    ReqFactory(uint16_t maxrand)
    : Maxrand(maxrand)    
    {
        srand(time(NULL));
    }

    std::shared_ptr<Req> GetNext(Type t)
    {
        unsigned next = rand() % Maxrand;
        auto ptr = std::shared_ptr<Req>(new Req(t, next)); 
        return ptr;
    }
protected:
    uint16_t Maxrand;
};


template<class T>
class Queue
{
public:
    Queue(unsigned max = 10)
    : Elems(0),
    Max(max)
    {

    }

    bool IsEmpty()
    {
        return Elems == 0;
    }

    bool IsFull()
    {
        return Elems == Max;
    }

    T Deq()
    {
        if(Elems < 0 || Q.size() == 0)
            throw nullptr;
        auto ret = Q.front();
        Q.pop();
        Elems--;
        return ret;
    }

    void Enq(T val)
    {
        if(Elems >= Max)
            return;

        Q.push(val);
        Elems++;
    }
    size_t Count()
    {
        return Elems;
    }

private:
    std::queue<T> Q;
    size_t Elems;
    size_t Max;
};

class Server
{
public:
    Server(unsigned randmax = 20)
    : BusyStatus(false),
    RandMax(randmax),
    FinishingTime(0)
    {
        srand(time(NULL));
    }

    bool Serve(unsigned currTime)
    {
        if(BusyStatus)
            return false;

        FinishingTime = currTime + ((rand() + 1) % RandMax);
        BusyStatus = true;

        return true;
    }

    unsigned GetTimeToProcess()
    {
        return FinishingTime;
    }
    
    bool IsBusy(unsigned currTime)
    {
       if(FinishingTime <= currTime)
       {
           BusyStatus = false;
           FinishingTime = 0;
       }
       return BusyStatus;
    }

protected:
    bool BusyStatus;
    unsigned RandMax;
    unsigned FinishingTime;
};

class Timer
{
    public:
        unsigned time;
};

int main(int argc, char **argv)
{
    uint32_t ending_t = 500, current_t = 0, finishing_t = ending_t + 1, first_ev_t = 0, second_ev_t = 0;
    ReqFactory factory(20);
    Queue<Type> queue(100); //TODO: remove
    Server server(10);

    auto type = Type::TReq1;
    
    first_ev_t = factory.GetNext(type)->time;
    second_ev_t = factory.GetNext(Type::TReq2)->time;

    while(current_t < ending_t)
    {
        if(finishing_t < first_ev_t && finishing_t < second_ev_t)
        //next event is server finishing with current request 
        {
            //set timer to that time
            current_t = finishing_t;
            Logger().get() << "Server is now free, new current time: " << current_t ;

            //Server MUST NOT be busy at this moment
            assert(!server.IsBusy(current_t));

            //try to deque last element in a queue
            //and tell server to process
            if(!queue.IsEmpty())
            {
                queue.Deq();
                Logger().get() << "Deq'ed (" << queue.Count() << ")";
                server.Serve(current_t);
                finishing_t = server.GetTimeToProcess();
            }
            else
            {
                //otherwise, set finishing_t to unreachable time
                finishing_t = ending_t + 1;
            }
        }
        else
        {
            //next event is either of requests coming in
            if(first_ev_t < second_ev_t)
            {
                current_t = first_ev_t;
                type = Type::TReq1;
                Logger().get() << "Next event is e1 @" << current_t ;
            }
            else
            {
                current_t = second_ev_t;
                type = Type::TReq2;
                Logger().get() << "Next event is e2 @" << current_t;
            }
            
            if(server.IsBusy(current_t))
            {
                //Server is busy
                //try to enqueue

                if(queue.IsFull())
                    throw nullptr;
                Logger().get() << "Enqueued(" << queue.Count() << ")";
                queue.Enq(type);
            }
            else
            {
                //Server is not busy
                // no queing
                server.Serve(current_t);
                finishing_t = server.GetTimeToProcess();

                Logger().get() << "Server is now busy, will finish serving @" << finishing_t;
            }
            //get time for arrival of the request
            //of the consumed request type
            auto time = factory.GetNext(type)->time;
            if(type == Type::TReq1)
            {
                first_ev_t = current_t + time;
                Logger().get() << "Generated event 1: " << first_ev_t;
            }
            else
            {
                second_ev_t = current_t + time;
                Logger().get() << "Generated event 2: " << second_ev_t;
            }
        }

        std::cout << "Iteration end. Current time: " << current_t << " e1: " << first_ev_t << " e2: " << second_ev_t<< " will finish serving @ " << finishing_t << std::endl;
    }
}
