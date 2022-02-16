#include "Application.h"

int main()
{
    // Below a few things you can do

    BasicRuleTest();

    AutoWordle("REPAS");
    AutoWordle("SAPIN");

    AutoSutom("DIAMETRE");

    FindBestOpening(5);

    ComputeAveragePerformance(5,10);

    ComputeAverageSutomPerformance(7,10);

    RealInteractiveGame();

    return 0;
}
