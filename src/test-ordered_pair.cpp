/*
 * This file uses the Catch unit testing library, alongside
 * testthat's simple bindings, to test a C++ function.
 *
 * For your own packages, ensure that your test files are
 * placed within the `src/` folder, and that you include
 * `LinkingTo: testthat` within your DESCRIPTION file.
 */

// All test files should include the <testthat.h>
// header file.
#include <unordered_set>
#include <testthat.h>
#include "Ordered_Pair.h"


// Initialize a unit test context. This is similar to how you
// might begin an R test file with 'context()', expect the
// associated context should be wrapped in braced.
context("Ordered_Pair template works properly") {

  const auto p12 = Ordered_Pair<int>(1,2);
  const auto p22 = Ordered_Pair<int>(2,2);
  const auto p23 = Ordered_Pair<int>(3,2);
  const auto p13 = Ordered_Pair<int>(3,1);

  test_that("Equality works") {
    expect_true(p12 == Ordered_Pair<int>(2,1));
    expect_true(p12 == Ordered_Pair<int>(1,2));
    expect_false(p12 == p22);
    expect_false(p12 == p13);
  }

  test_that("Less Than works"){
    expect_true(p12 < p22);
    expect_true(p13 < p22);

    expect_false(p22 < p12);
    expect_false(p22 < p13);
  }

}

context("Hashing works properly") {

  std::unordered_set<Ordered_Pair<int>, Ordered_Pair_Hash<int>> pair_set;


  pair_set.insert(Ordered_Pair<int>(1,2));
  pair_set.insert(Ordered_Pair<int>(2,1));
  pair_set.insert(Ordered_Pair<int>(2,2));
  pair_set.insert(Ordered_Pair<int>(3,2));
  pair_set.insert(Ordered_Pair<int>(3,1));

  test_that("Size is right") {
    expect_true(pair_set.size() == 4); // One duplicate added
  }

}
