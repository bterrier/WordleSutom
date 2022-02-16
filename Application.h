#ifndef APPLICATION_H
#define APPLICATION_H

#include <string>

void BasicRuleTest();
int AutoSutom(const std::string &ground_truth);
int AutoWordle(const std::string &ground_truth);
void FindBestOpening(int K);
void ComputeAveragePerformance(int K, int NB_TESTS);
void ComputeAverageSutomPerformance(int K, int NB_TESTS);
void RealInteractiveGame();

#endif // APPLICATION_H
