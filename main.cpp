#include <cstdint>
#include <cstdlib>
#include <ctime>
#include <queue>
#include <iostream>
#include <iomanip>
#include <memory>
#include <string>
#include <cassert>

#include <sstream>

#define LOG(x) Logger().get() << std::fixed << std::setprecision(3) << x 

#ifdef _DEBUG 
    #define LOG_DEBUG(x) Logger().get() << x 
#else
    #define LOG_DEBUG(x) 
#endif

class Generator
{
    public:
        virtual float GetNext() = 0;
};

class RandGenerator : public Generator
{

public:
    RandGenerator(unsigned max)
    : Max(max)
    {
        srand(time(NULL));
    }
    virtual float GetNext()
    {
        float toRet = rand() + 1;
        while(toRet > Max)
            toRet /= 10;
        return toRet; 
    }
private:
        unsigned Max;
};

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

    Req(Type type, float t)
    : type(type), time(t)
    { }

    float time;
    Type type;
};

struct ReqFactory
{
    ReqFactory(std::shared_ptr<Generator> gen)
    : pGen(gen)    
    {
        srand(time(NULL));
    }

    std::shared_ptr<Req> GetNext(Type t)
    {
        float next = pGen->GetNext();
        auto ptr = std::shared_ptr<Req>(new Req(t, next)); 
        return ptr;
    }
protected:
    std::shared_ptr<Generator> pGen;
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
        Q.erase(Q.begin());
        Elems--;
        return ret;
    }

    void Enq(T val)
    {
        if(Elems >= Max)
            return;

        Q.push_back(val);
        Elems++;
    }
    size_t Count()
    {
        return Elems;
    }
    std::string Serialize()
    {
        std::string ret = "[";
        for(auto &it : Q)
        {
            ret +=(it == T::TReq1) ? "e1, " : "e2, ";
        }
        if(ret.size() > 3)
            ret.erase(ret.size() - 2,2);
        ret += "]";
        return ret;
    }

private:
    std::vector<T> Q;
    size_t Elems;
    size_t Max;
};

class Server
{
public:
    Server(std::shared_ptr<Generator> gen)
    : pGen(gen),
    FinishingTime(0)
    { }

    bool Serve(float currTime)
    {
        if(BusyStatus)
            return false;

        FinishingTime = currTime + pGen->GetNext();
        BusyStatus = true;

        return true;
    }

    float GetTimeToProcess()
    {
        return FinishingTime;
    }
    
    bool IsBusy(float currTime)
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
    std::shared_ptr<Generator> pGen;
    float FinishingTime;
};

class Timer
{
    public:
        float time;
};

unsigned ENDING_T = 200;
unsigned EVENT_FREQ = 20;
unsigned QUEUE_CAP = 20;
unsigned SERVER_FREQ = 10;

int main(int argc, char **argv)
{
    float ending_t = ENDING_T, current_t = 0.0f, finishing_t = ending_t + 1, first_ev_t = 0, second_ev_t = 0;
    std::shared_ptr<Generator> reqGen = std::shared_ptr<Generator>(new RandGenerator(EVENT_FREQ));
    std::shared_ptr<Generator> servGen = std::shared_ptr<Generator>(new RandGenerator(SERVER_FREQ));

    ReqFactory factory(reqGen); 
    Server server(servGen);
    Queue<Type> queue(QUEUE_CAP); 

    auto type = Type::TReq1;
    
    first_ev_t = factory.GetNext(type)->time;
    second_ev_t = factory.GetNext(Type::TReq2)->time;
    LOG("t: " << current_t << "; e1: " << first_ev_t << "; e2: " << second_ev_t<< "; h: " << finishing_t <<"; S: " << server.IsBusy(current_t) << "; n: " << queue.Count() << "; Q: " << "[]"<< std::endl);
    std::string typeStr = "";
    while(current_t < ending_t)
    {
        if(finishing_t <= first_ev_t && finishing_t <= second_ev_t)
        //next event is server finishing with current request 
        {
            typeStr = "Finished processing";
            //set timer to that time
            current_t = finishing_t;

            //Server MUST NOT be busy at this moment
            assert(!server.IsBusy(current_t));

            //try to deque last element in a queue
            //and tell server to process
            if(!queue.IsEmpty())
            {
                queue.Deq();
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
            if(first_ev_t <= second_ev_t)
            {
                current_t = first_ev_t;
                type = Type::TReq1;
                typeStr = "e1";
            }
            else
            {
                current_t = second_ev_t;
                type = Type::TReq2;
                typeStr = "e2";
            }
            
            if(server.IsBusy(current_t))
            {
                //Server is busy
                //try to enqueue

                if(queue.IsFull())
                    throw nullptr;
                queue.Enq(type);
            }
            else
            {
                //Server is not busy
                // no queing
                server.Serve(current_t);
                finishing_t = server.GetTimeToProcess();

            }
            //get time for arrival of the request
            //of the consumed request type
            auto t_time = factory.GetNext(type)->time;
            if(type == Type::TReq1)
            {
                first_ev_t = current_t + t_time;
            }
            else
            {
                second_ev_t = current_t + t_time;
            }
        }

        LOG("t: " << current_t << "; e1: " << first_ev_t << "; e2: " << second_ev_t<< "; h: " << finishing_t <<"; S: " << server.IsBusy(current_t) << "; n: " << queue.Count() << "; Q: " << queue.Serialize() <<"; Type: " <<typeStr << std::endl);
    }
}
