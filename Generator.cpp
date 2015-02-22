#include "Generator.h"

#include <ctime>
#include <cstdlib>

float RandomGenerator::GetNext()
{
    return rand() % randMax_;
}

RandomGenerator::RandomGenerator(uint16_t randMax)
: randMax_(randMax)
{
    srand(time(NULL));

}
