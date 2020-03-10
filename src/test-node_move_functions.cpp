#include <testthat.h>
#include "Node_Container.h"
#include "Edge_Container.h"
#include "propose_move.h"
#include "move_entropy_delta.h"
#include <random>

using Random_Engine = std::mt19937;


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
  Node * a1 = &nodes.at(0,0);

  Node * proposed_block = propose_move(a1, blocks, random_engine, 0.1);
  const int num_samples = 100;

  for (int i = 0; i < num_samples; i++) {
    // The proposed block better have the same type as the node being proposed
    expect_true(proposed_block->type_index == a1->type_index);
  }
}


