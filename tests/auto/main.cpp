#include <gtest/gtest.h>

#include "gamestate.h"

// Demonstrate some basic assertions.
TEST(HelloTest, ComputePattern) {

  const std::string target = "ABCDEF";

  EXPECT_EQ(ComputePattern("XXXXXX", target), 0);
  EXPECT_EQ(ComputePattern("AXXXXX", target), 2);
  EXPECT_EQ(ComputePattern("XAXXXX", target), 3);
  EXPECT_EQ(ComputePattern("AEXXXX", target), 5);
  EXPECT_EQ(ComputePattern("AAXXXX", target), 2);
  EXPECT_EQ(ComputePattern("XAAXXX", target), 3);
  EXPECT_EQ(ComputePattern("ABXXXX", target), 2 + 2*3);
  EXPECT_EQ(ComputePattern("ABCDEF", target), 2 + 2*3 + 2*9 + 2*27 + 2*81 +2*243);
  EXPECT_EQ(ComputePattern("BXXXXX", target), 1);

  EXPECT_EQ(ComputePattern("AXAXX", "AABCD"), 11);
  EXPECT_EQ(ComputePattern("AAXXX", "AABCD"), 8);
  EXPECT_EQ(ComputePattern("AAXXA", "AABCD"), 8);

  EXPECT_EQ(ComputePattern("AAXXA", "AAACD"), 2 + 2*3 + 0*9 + 0*27 +1*81);
}
