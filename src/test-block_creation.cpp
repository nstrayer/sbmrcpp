#include <testthat.h>
#include "Node_Container.h"
#include "swap_blocks.h"
#include <random>

using Random_Engine = std::mt19937;

// associated context should be wrapped in braced.
context("Adding blocks for unipartite network") {

  const Rcpp::CharacterVector    nodes_id{"a1", "a2", "a3"};
  const Rcpp::CharacterVector  nodes_type{"a", "a", "a"};
  const Rcpp::CharacterVector  types_name{"a"};
  const Rcpp::IntegerVector   types_count{ 3 };

  // Initialize a random engine and seed
  Random_Engine random_engine{};
  random_engine.seed(42);

  auto nodes = Node_Container(nodes_id, nodes_type, types_name, types_count);

  test_that("Before building blocks, each child node has no parent") {
    expect_true(nodes.at(0, 0).get_parent() == nullptr); // a1
    expect_true(nodes.at(0, 1).get_parent() == nullptr); // a2
    expect_true(nodes.at(0, 2).get_parent() == nullptr); // a3
  }

  auto blocks = Node_Container(2, nodes, random_engine);

  test_that("Sizing is correct") {
    expect_true(blocks.size() == 2);
  }

  test_that("Each child node has a parent") {
    expect_true(nodes.at(0, 0).get_parent() != nullptr); // a1
    expect_true(nodes.at(0, 1).get_parent() != nullptr); // a2
    expect_true(nodes.at(0, 2).get_parent() != nullptr); // a3
  }

  test_that("Each block node has at least one child") {
    expect_true(blocks.at(0, 0).children.size() > 0);
    expect_true(blocks.at(0, 1).children.size() > 0);
  }

  test_that("Wont allow more blocks than nodes") {
    expect_error(Node_Container(4, nodes, random_engine));
  }
}



context("Adding blocks for tripartite network") {

  auto nodes = Node_Container(Rcpp::CharacterVector{"a1", "a2", "a3", "b1", "b2", "c1", "c2", "c3"},
                              Rcpp::CharacterVector{ "a",  "a",  "a",  "b",  "b",  "c",  "c",  "c"},
                              Rcpp::CharacterVector{"a", "b", "c"},
                              Rcpp::IntegerVector{    3,   2,   3});

  // Initialize a random engine and seed
  Random_Engine random_engine{};
  random_engine.seed(42);


  test_that("Before building blocks, each child node has no parent") {
    expect_true(nodes.at(0, 0).get_parent() == nullptr); // a1
    expect_true(nodes.at(0, 1).get_parent() == nullptr); // a2
    expect_true(nodes.at(0, 2).get_parent() == nullptr); // a3
    expect_true(nodes.at(1, 0).get_parent() == nullptr); // b1
    expect_true(nodes.at(1, 1).get_parent() == nullptr); // b2
    expect_true(nodes.at(2, 0).get_parent() == nullptr); // c1
    expect_true(nodes.at(2, 1).get_parent() == nullptr); // c2
    expect_true(nodes.at(2, 2).get_parent() == nullptr); // c3
  }

  auto blocks = Node_Container(2, nodes, random_engine);

  test_that("Sizing is correct") {
    expect_true(blocks.size() == 6); // 2 blocks for 3 node types
  }

  test_that("Each child node now has a parent") {
    expect_true(nodes.at(0, 0).get_parent() != nullptr); // a1
    expect_true(nodes.at(0, 1).get_parent() != nullptr); // a2
    expect_true(nodes.at(0, 2).get_parent() != nullptr); // a3
    expect_true(nodes.at(1, 0).get_parent() != nullptr); // b1
    expect_true(nodes.at(1, 1).get_parent() != nullptr); // b2
    expect_true(nodes.at(2, 0).get_parent() != nullptr); // c1
    expect_true(nodes.at(2, 1).get_parent() != nullptr); // c2
    expect_true(nodes.at(2, 2).get_parent() != nullptr); // c3
  }

  test_that("Each block node has at least one child") {
    expect_true(blocks.at(0, 0).children.size() > 0);
    expect_true(blocks.at(0, 1).children.size() > 0);
    expect_true(blocks.at(1, 0).children.size() > 0);
    expect_true(blocks.at(1, 1).children.size() > 0);
    expect_true(blocks.at(2, 0).children.size() > 0);
    expect_true(blocks.at(2, 1).children.size() > 0);
  }

  test_that("Wont allow more blocks than nodes") {
    expect_error(Node_Container(3, nodes, random_engine)); // Type b only has two nodes
  }
}

context("Block swapping") {
  // Three types of nodes with three nodes each
  auto nodes = Node_Container(Rcpp::CharacterVector{"a1", "a2", "a3", "b1", "b2", "b3", "c1", "c2", "c3"},
                              Rcpp::CharacterVector{ "a",  "a",  "a",  "b",  "b",  "b",  "c",  "c",  "c"},
                              Rcpp::CharacterVector{"a", "b", "c"},
                              Rcpp::IntegerVector{    3,   3,   3});

  // Initialize a random engine and seed
  Random_Engine random_engine{};
  random_engine.seed(42);

  // Give every node its own block
  auto blocks = Node_Container(3, nodes, random_engine);

  Node* node_a = &nodes.at(0,0);
  Node* node_b = &nodes.at(0,1);

  // Make sure that node a and b don't have the same parent
  expect_true(node_a->get_parent() != node_b->get_parent());

  // Swap first nodes block to the block of the second node, allowing empty block removal
  swap_block(node_a, node_b->get_parent(), blocks, true);

  // Check to make sure now the two nodes share the same block
  expect_true(node_a->get_parent() == node_b->get_parent());

  // Check to make sure the blocks for that type has shrunk by one.
  expect_true(blocks.size_of_type(0) == 2);
}


