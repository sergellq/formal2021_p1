#include <gtest/gtest.h>
#include <Automaton.h>
#include "solve.h"

TEST(manual_tests, test1) {
  ASSERT_EQ("INF", solve("ab+c.aba.*.bac.+.+*", "b", 2));
  ASSERT_EQ("4", solve("acb..bab.c.*.ab.ba.+.+*a.", "a", 2));

  ASSERT_EQ("INF", solve("0", "a", 1));
  ASSERT_EQ("INF", solve("0", "a", 0));
  ASSERT_EQ("INF", solve("1", "a", 1));
  ASSERT_EQ("0", solve("1", "a", 0));

  ASSERT_EQ("INF", solve("abcabcabc...*...*..", "a", 10));
  ASSERT_EQ("3", solve("ab.bc+*cab+*.*.+", "a", 3));
  ASSERT_EQ("3", solve("ab.bc+*cab+*.*.+", "b", 3));
  ASSERT_EQ("3", solve("ab.bc+*cab+*.*.+", "c", 3));

  ASSERT_EQ("5", solve("abcbc.+*..abcbc*.+.+abcbc....++*", "a", 5));
  ASSERT_EQ("6", solve("abcbc.+*..abcbc*.+.+abcbc....++*", "b", 5));
  ASSERT_EQ("7", solve("abcbc.+*..abcbc*.+.+abcbc....++*", "c", 5));

  ASSERT_EQ("7", solve("abcbc.+*..abcbc*.+.+abcbc....++*abcbc.+*..abcbc*.+.+abcbc....++*abcbc.+*..abcbc*.+.+abcbc....++*..", "a", 7));
  ASSERT_EQ("8", solve("abcbc.+*..abcbc*.+.+abcbc....++*abcbc.+*..abcbc*.+.+abcbc....++*abcbc.+*..abcbc*.+.+abcbc....++*..", "b", 7));
  ASSERT_EQ("9", solve("abcbc.+*..abcbc*.+.+abcbc....++*abcbc.+*..abcbc*.+.+abcbc....++*abcbc.+*..abcbc*.+.+abcbc....++*..", "c", 7));
}
