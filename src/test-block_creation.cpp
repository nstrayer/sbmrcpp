#include <testthat.h>
#include "Node_Container.h"
#include <random>

using Random_Engine = std::mt19937;

// associated context should be wrapped in braced.
context("Adding blocks for unipartite network") {

  const Rcpp::CharacterVector    nodes_id{"a1", "a2", "a3"};
  const Rcpp::CharacterVector  nodes_type{"a", "a", "a"};
  const Rcpp::CharacterVector  types_name{"a"};
  const Rcpp::IntegerMatrix   types_count{ 3 };

  // Initialize a random engine and seed
  Random_Engine random_engine{};
  random_engine.seed(42);

  auto nodes = Node_Container(nodes_id, nodes_type, types_name, types_count);

  test_that("Before building blocks, each child node has no parent") {
    expect_true(nodes.at(0, 0).parent_ref == nullptr); // a1
    expect_true(nodes.at(0, 1).parent_ref == nullptr); // a2
    expect_true(nodes.at(0, 2).parent_ref == nullptr); // a3
  }

  auto blocks = Node_Container(2, nodes, random_engine);

  test_that("Sizing is correct") {
    expect_true(blocks.size() == 2);
  }

  test_that("Each child node has a parent") {
    expect_true(nodes.at(0, 0).parent_ref != nullptr); // a1
    expect_true(nodes.at(0, 1).parent_ref != nullptr); // a2
    expect_true(nodes.at(0, 2).parent_ref != nullptr); // a3
  }

  test_that("Each block node has at least one child") {
    expect_true(blocks.at(0, 0).children.size() > 0);
    expect_true(blocks.at(0, 1).children.size() > 0);
  }
}

