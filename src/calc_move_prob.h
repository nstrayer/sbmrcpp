#pragma once
// Calculates the probability of a node moving to a given neighbor group.
// Assuming we're calculating the prob of node i moving to group s:
// Uses formula sum over all blocks t, p^i_t * (e_st + eps) / (e_s + epsB)
// Where p^i_t : proportion of node i's edges that go to block t
//       e_st  : number of edges between blocks s and t
//       e_s   : total number of edges for block s (aka its degree)
//       eps   : parameter controlling ergodicity of moves
//       B     : total number of possible neighbors that node i could have

#include "Node_Container.h"

using Node_Edge_Counts = std::map<Node*, int>;
using Edge_Count_Pair = std::pair<Node*, int>;

double calc_move_prob(const Node_Edge_Counts& node_to_blocks,
                      Node* block_moved_to,
                      const double node_degree,
                      const double eps,
                      const double epsB) {

  Node_Edge_Counts block_counts = block_moved_to->get_block_edge_counts();

  auto add_probs = [&](double sum, const Edge_Count_Pair& edge_count){
    const int neighbor_degree = edge_count.first->get_degree();

    return sum + edge_count.second/node_degree * (block_counts[edge_count.first] + eps) /
                                                 (neighbor_degree                + epsB);
  };

  return std::accumulate(node_to_blocks.begin(), node_to_blocks.end(), 0.0, add_probs);
}
