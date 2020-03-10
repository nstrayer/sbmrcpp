#include <testthat.h>
#include "Edge_Container.h"
#include "propose_move.h"
#include "move_entropy_delta.h"
#include "swap_blocks.h"
#include <random>

// #include <testthat.h>
// #include "Edge_Container.h"
// #include "swap_blocks.h"
// #include <random>

using Random_Engine = std::mt19937;

void expect_approx_equal(const double a, const double b, const double thresh = 0.1){
  expect_true(std::abs(a - b) < thresh);
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

  Node* node_a = nodes.at(0,0);
  Node* node_b = nodes.at(0,1);

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
  Node * a1 = nodes.at(0,0);
  Node * a2 = nodes.at(0,1);
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



context("Move proposals") {

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
  Node * a1 = nodes.at(0,0);

  Node * proposed_block = propose_move(a1, blocks, random_engine, 0.1);
  const int num_samples = 100;

  for (int i = 0; i < num_samples; i++) {
    // The proposed block better have the same type as the node being proposed
    expect_true(proposed_block->type_index == a1->type_index);
  }
}



context("Move entropy delta") {
  // Initialize a random engine and seed
  Random_Engine random_engine{};
  random_engine.seed(42);

  auto nodes_id   = Rcpp::CharacterVector{"n1", "n2", "n3", "n4", "n5"};
  auto nodes_type = Rcpp::CharacterVector{ "a",  "a",  "a",  "a",  "a"};
  auto types_name  = Rcpp::CharacterVector{"a"};
  auto types_count = Rcpp::IntegerVector{    5};

  const Rcpp::CharacterVector edges_from{"n1", "n1", "n3", "n1", "n3", "n3", "n5"};
  const Rcpp::CharacterVector   edges_to{"n2", "n2", "n4", "n4", "n2", "n4", "n4"};

  auto nodes = Node_Container(nodes_id, nodes_type, types_name, types_count);
  auto edges = Edge_Container(edges_from, edges_to, nodes_id, nodes);
  auto blocks = Node_Container(5, nodes, random_engine); // One block per node

  // This order is retrieved by just printing and assigning
  Node* n1 = nodes.at(0, 3);
  Node* n2 = nodes.at(0, 1);
  Node* n3 = nodes.at(0, 4);
  Node* n4 = nodes.at(0, 0);
  Node* n5 = nodes.at(0, 2);

  // Merge blocks for nodes 1,2 and 3,4
  swap_block(n2, n1->get_parent(), blocks);
  swap_block(n4, n3->get_parent(), blocks);

  // Make sure this worked
  expect_true(blocks.size() == 3);
  expect_true(n1->get_parent() == n2->get_parent());
  expect_true(n3->get_parent() == n4->get_parent());
  expect_true(n5->get_parent() != n1->get_parent());
  expect_true(n5->get_parent() != n3->get_parent());

  // Grab the three remaining blocks
  Node* g3 = n5->get_parent();

  // Propose moving n4 from g2 to g3
  const double move_delta = move_entropy_delta(n4, g3, edges.edges);

  expect_approx_equal(move_delta, 0.8920503);
  // Calculated this value in R using
  // pre_ent <- 2*log(2/(6 * 6)) +
  //   2*log(2/(7 * 7)) +
  //   2*log(2/(6 * 7)) +
  //   1*log(1/(7 * 1))
  //
  // post_ent <- 2*log(2/(6 * 6)) +
  //   2*log(2/(3 * 5)) +
  //   1*log(1/(6 * 5)) +
  //   1*log(1/(3 * 6)) +
  //   1*log(1/(5 * 5))
  //
  // post_ent - pre_ent

}

