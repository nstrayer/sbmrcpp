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

  // Grab nodes by their ids
  auto node_by_id = nodes.get_id_to_node_map(nodes_id);
  Node* n1 = node_by_id.at("n1");
  Node* n2 = node_by_id.at("n2");
  Node* n3 = node_by_id.at("n3");
  Node* n4 = node_by_id.at("n4");
  Node* n5 = node_by_id.at("n5");

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
  const double move_delta = move_entropy_delta(n4, g3, nodes, blocks, edges);

  expect_approx_equal(move_delta, -0.1989031);

  // ent <- function(ers, er, es) {ers * log(ers/(er*es))}
  // pre_ent <-
  //   ent(4, 6, 6)/2 +
  //   ent(4, 7, 7)/2 +
  //   ent(2, 6, 7) +
  //   ent(1, 7, 1)
  //
  // post_ent <-
  //   ent(4, 6, 6)/2 +
  //   ent(2, 5, 5)/2 +
  //   ent(2, 3, 5) +
  //   ent(1, 6, 5) +
  //   ent(1, 3, 6)
  //
  // pre_ent - post_ent
}


context("Move proposal returns values are correct (simple unipartite)") {
  Random_Engine random_engine{};
  random_engine.seed(42);

  auto nodes_id   = Rcpp::CharacterVector{"n1", "n2", "n3", "n4", "n5", "n6"};
  auto nodes_type = Rcpp::CharacterVector{ "a",  "a",  "a",  "a",  "a",  "a"};
  auto types_name  = Rcpp::CharacterVector{"a"};
  auto types_count = Rcpp::IntegerVector{    6};

  const Rcpp::CharacterVector edges_from{"n1", "n1", "n1", "n1", "n2", "n2", "n2", "n3", "n3", "n4", "n4", "n5"};
  const Rcpp::CharacterVector   edges_to{"n2", "n3", "n4", "n5", "n3", "n4", "n5", "n4", "n6", "n5", "n6", "n6"};

  auto nodes = Node_Container(nodes_id, nodes_type, types_name, types_count);
  auto edges = Edge_Container(edges_from, edges_to, nodes_id, nodes);
  auto blocks = Node_Container(6, nodes, random_engine); // One block per node

  // Grab nodes by their ids
  auto node_by_id = nodes.get_id_to_node_map(nodes_id);
  Node* n1 = node_by_id.at("n1");
  Node* n2 = node_by_id.at("n2");
  Node* n3 = node_by_id.at("n3");
  Node* n4 = node_by_id.at("n4");
  Node* n5 = node_by_id.at("n5");
  Node* n6 = node_by_id.at("n6");

  // n1 and n2 share a block
  swap_block(n2, n1->get_parent(), blocks);
  // n3 and n4 share a block
  swap_block(n4, n3->get_parent(), blocks);
  // n5 and n6 share a block
  swap_block(n6, n5->get_parent(), blocks);

  // Make sure we have correct number of blocks
  expect_true(blocks.size() == 3);

  // Group c is n5 and n6's parent
  Node* group_c = n5->get_parent();
  // Propose move of n4 to group c
  const double move_delta = move_entropy_delta(n4, group_c, nodes, blocks, edges);

  // Delta from hand calculation
  expect_approx_equal(move_delta,  -0.1117765);

  // R code for finding value
  // ent <- function(ers, er, es) {ers * log(ers/(er*es))}
  // pre_ent <-
  // ent(4, 8, 9) +
  // ent(2, 8, 7) +
  // ent(3, 7, 9) +
  // ent(2, 7, 7)/2 +
  // ent(2, 9, 9)/2 +
  // ent(2, 8, 8)/2
  //
  // post_ent <-
  // ent(2, 8, 4) +
  // ent(4, 8, 12) +
  // ent(2, 12, 4) +
  // ent(2, 8, 8)/2 +
  // ent(6, 12, 12)/2
  //
  // pre_ent - post_ent
}


context("Move proposal returns values are correct (simple bipartite)") {
  Random_Engine random_engine{};
  random_engine.seed(42);

  auto nodes_id   = Rcpp::CharacterVector{"a1", "a2", "a3", "a4", "b1", "b2", "b3", "b4"};
  auto nodes_type = Rcpp::CharacterVector{ "a",  "a",  "a",  "a",  "b",  "b",  "b",  "b"};
  auto types_name  = Rcpp::CharacterVector{"a", "b"};
  auto types_count = Rcpp::IntegerVector{    4,   4};

  const Rcpp::CharacterVector edges_from{"a1", "a2", "a2", "a3", "a3", "a3", "a4"};
  const Rcpp::CharacterVector   edges_to{"b2", "b1", "b2", "b1", "b2", "b4", "b3"};

  auto nodes = Node_Container(nodes_id, nodes_type, types_name, types_count);
  auto edges = Edge_Container(edges_from, edges_to, nodes_id, nodes);
  auto blocks = Node_Container(4, nodes, random_engine); // One block per node


  // Grab nodes by their ids
  auto n_id = nodes.get_id_to_node_map(nodes_id);

  // a2 and a3 share a block
  swap_block(n_id.at("a3"), n_id.at("a2")->get_parent(), blocks);

  // b1 and b2 share a block
  swap_block(n_id.at("b2"), n_id.at("b1")->get_parent(), blocks);

  // Make sure we have correct number of blocks
  expect_true(blocks.size() == 6);

  // propose moving a2 into block with a1
  const double move_delta = move_entropy_delta(n_id.at("a2"), n_id.at("a1")->get_parent(), nodes, blocks, edges);

  expect_approx_equal(move_delta, -0.5924696);

  // pre_ent <-
  //   ent(1, 1, 5) +
  //   ent(4, 5, 5) +
  //   ent(1, 5, 1) +
  //   ent(1, 1, 1)
  //
  // post_ent <-
  //   ent(3, 3, 5) +
  //   ent(2, 3, 5) +
  //   ent(1, 3, 1) +
  //   ent(1, 1, 1)
  //
  // pre_ent - post_ent
}


