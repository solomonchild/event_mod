#pragma once
#include <cstdint>
#include "Generator.h"

struct Transact
{

};

class Processor
{
public:
    Processor(Generator generator, uint32_t tEnd);

    void process(/*TODO: add params*/);
private:
    uint32_t ending_t_;
    uint32_t current_t_;
    uint32_t h_;
    Generator gen_;
};
