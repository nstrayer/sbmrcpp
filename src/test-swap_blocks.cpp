#include <testthat.h>
#include "Edge_Container.h"
#include "swap_blocks.h"
#include <random>

using Random_Engine = std::mt19937;


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



context("Edge counts are properly accounted after swapping") {

  auto nodes_id   = Rcpp::CharacterVector{"a1", "a2", "b1", "b2", "b3", "c1", "c2", "c3"};
  auto nodes_type = Rcpp::CharacterVector{ "a",  "a",  "b",  "b",  "b",  "c",  "c",  "c"};
  auto types_name  = Rcpp::CharacterVector{"a", "b", "c"};
  auto types_count = Rcpp::IntegerVector{    2,   3,   3};

  // Has connections from a-b, a-c, and b-c
  const Rcpp::CharacterVector edges_from{"a1", "a1", "a1", "a2", "a2", "a2", "a2"};
  const Rcpp::CharacterVector   edges_to{"b1", "b2", "c1", "b2", "b3", "c2", "c3"};

  auto nodes = Node_Container(nodes_id, nodes_type, types_name, types_count);
  auto edges = Edge_Container(edges_from, edges_to, nodes_id, nodes);

  // Initialize a random engine and seed
  Random_Engine random_engine{};
  random_engine.seed(42);

  // Give every node its own block
  auto blocks = Node_Container(2, nodes, random_engine);

  // Get reference to the two blocks that make up the a type blocks
  Node * a1 = &nodes.at(0,0);
  Node * a2 = &nodes.at(0,1);
  Node * ba1 = a1->get_parent();
  Node * ba2 = a2->get_parent();

  // Blocks should just mimic their only node's edge counts
  expect_true(ba1->get_edges_to_type(1).size() == 2);
  expect_true(ba1->get_edges_to_type(2).size() == 1);

  expect_true(ba2->get_edges_to_type(1).size() == 2);
  expect_true(ba2->get_edges_to_type(2).size() == 2);

  // Now we want to bring a2 into the same group as a1 (don't remove empty group)
  swap_block(a2, ba1, blocks, false);

  // Blocks should just mimic their only node's edge counts
  expect_true(ba1->get_edges_to_type(1).size() == 4);
  expect_true(ba1->get_edges_to_type(2).size() == 3);

  expect_true(ba2->get_edges_to_type(1).size() == 0);
  expect_true(ba2->get_edges_to_type(2).size() == 0);

}

