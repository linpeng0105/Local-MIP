#include "paras.h"
#include "../Solver.h"

int main(int argc, char **argv)
{

    INIT_ARGS

    __global_paras.print_change();

    Solver *solver = new Solver();
    solver->Run();

    return 0;
}