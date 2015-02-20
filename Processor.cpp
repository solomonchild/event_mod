#include <cstdio>
#include "Processor.h"

#define LOG_STATE()\
{\
    printf("current t: %u, ending t: %u, h: %u", current_t_, ending_t_, h_);\
    printf("\n");\
}

Processor::Processor(Generator gen, uint32_t tEnd)
: ending_t_(tEnd),
  h_(0), 
  current_t_(0),
  gen_(gen)
{

}

void Processor::process()
{
    while(current_t_ < ending_t_)
    {
        LOG_STATE();
        current_t_++;
    }

};
