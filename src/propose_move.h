#ifndef __PROPOSE_MOVE_INCLUDED__
#define __PROPOSE_MOVE_INCLUDED__

#include "Node_Container.h"


Node* propose_move(Node* node,
                   Node_Container& blocks,
                   Random_Engine& random_engine,
                   const double eps = 0.1) {
  // To propose a move of `node_i` of type `t_i` to a new block we

  // Sample a random neighbor block
  Node* neighbor_block = node->get_random_neighbor(random_engine)->get_parent();

  // Get all the edges the neighbor block has to nodes of the node-to-move's type
  Node_Ptrs& neighbor_edges_to_t = neighbor_block->get_edges_to_type(node->type_index);

  // Get a reference to all the blocks that the node-to-move _could_ join
  Node_Vec& all_potential_blocks = blocks.get_nodes_of_type(node->type_index);

  // Decide if we are going to choose a random block for our node
  const double ergo_amnt            = eps * all_potential_blocks.size();
  const double prob_of_random_block = ergo_amnt / (double(neighbor_edges_to_t.size()) + ergo_amnt);

  // Decide where we will get new block from and draw from potential candidates
  return std::uniform_real_distribution<>()(random_engine) < prob_of_random_block
    ? get_random_element(all_potential_blocks, random_engine).get()
    : get_random_element(neighbor_edges_to_t, random_engine);
}

#endif
