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

using Int_Pair = Ordered_Pair<int>;
using String_Pair = Ordered_Pair<std::string>;

// Initialize a unit test context. This is similar to how you
// might begin an R test file with 'context()', expect the
// associated context should be wrapped in braced.
context("Ordered_Pair template works properly with integers") {

  const auto p12 = Int_Pair(1,2);
  const auto p22 = Int_Pair(2,2);
  const auto p23 = Int_Pair(3,2);
  const auto p13 = Int_Pair(3,1);

  test_that("Equality works") {
    expect_true(p12 == Int_Pair(2,1));
    expect_true(p12 == Int_Pair(1,2));
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

context("Ordered_Pair template works properly with strings") {

  const auto p_ab = String_Pair("a","b");
  const auto p_bb = String_Pair("b","b");
  const auto p_cb = String_Pair("c","b");
  const auto p_ad = String_Pair("a","d");

  test_that("Equality works") {
    expect_true(p_ab == String_Pair("b","a"));
    expect_true(p_cb == String_Pair("c", "b"));
    expect_false(p_ab == p_bb);
    expect_false(p_ab == p_ad);
  }

  test_that("Less Than works"){
    expect_true(p_ab < p_bb);
    expect_true(p_ad < p_bb);

    expect_false(p_bb < p_ab);
    expect_false(p_bb < p_ad);
  }

}

context("Hashing works properly with ints") {

  std::unordered_set<Int_Pair, Ordered_Pair_Hash<int>> pair_set;


  pair_set.insert(Int_Pair(1,2));
  pair_set.insert(Int_Pair(2,1));
  pair_set.insert(Int_Pair(2,2));
  pair_set.insert(Int_Pair(3,2));
  pair_set.insert(Int_Pair(3,1));

  test_that("Size is right") {
    expect_true(pair_set.size() == 4); // One duplicate added
  }

}

context("Hashing works properly with strings") {

  std::unordered_set<String_Pair, Ordered_Pair_Hash<std::string>> pair_set;


  pair_set.insert(String_Pair("a","b"));
  pair_set.insert(String_Pair("b","a"));
  pair_set.insert(String_Pair("b","b"));
  pair_set.insert(String_Pair("c","b"));
  pair_set.insert(String_Pair("c","a"));

  test_that("Size is right") {
    expect_true(pair_set.size() == 4); // One duplicate added
  }

}
