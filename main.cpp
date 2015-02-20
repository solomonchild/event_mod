
#include "Processor.h"
#include "Generator.h"

int main(int argc, char **argv)
{
    Generator g;
    Processor proc(g, 500);
    proc.process();

}
