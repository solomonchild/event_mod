#pragma once

#include <cstdint>

class Generator
{
public:
    virtual float GetNext() = 0;
};

class RandomGenerator :  public Generator
{
public:
    RandomGenerator(uint16_t randMax);
    float GetNext();
private:
    uint16_t randMax_;


};
