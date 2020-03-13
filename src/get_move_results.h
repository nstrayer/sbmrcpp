#pragma once
// Takes a node and a new block along with the containers for the blocks and nodes and
// calculates both the entropy delta of the SBM before and after the proposed move and
// the ratio of the probabilities of moving to the proposed block before the move and
// moving back to the original block after the move.
// #include "calc_edge_entropy.h"
#include "Edge_Container.h"
#include "calc_move_prob.h"
#include "swap_blocks.h"

using Edge = Ordered_Pair<Node*>;
using Edge_Map = std::map<Edge, int>;
using Edge_Map_Pair = std::pair<Edge, int>;

using Node_Edge_Counts = std::map<Node*, int>;
using Edge_Count = std::pair<Node*, int>;

struct Move_Results {
  double entropy_delta = 0.0;
  double prob_ratio = 1.0;
  Move_Results(const double& e, const double& p) :
    entropy_delta(e),
    prob_ratio(p) {}
};

void sum_edge_counts(Edge_Map& pair_counts,
                     Node* this_block,
                     const Node* to_ignore = nullptr){

  auto add_edge = [&](const Edge_Count& block_count){
    if (block_count.first != to_ignore)
      pair_counts[Edge(this_block, block_count.first)] += block_count.second;
  };

  Node_Edge_Counts edges_cts = this_block->get_block_edge_counts();

  std::for_each(edges_cts.begin(), edges_cts.end(), add_edge);
}

Move_Results get_move_results(Node* node,
                              Node* new_block,
                              const Node_Container& nodes,
                              Node_Container& blocks,
                              const Edge_Container& edges,
                              const double eps = 0.1){
  Node* old_block = node->get_parent();

  // No need to go on if we're "swapping" to the same group
  if(new_block == old_block) return Move_Results(0, 1);

  const double node_degree = node->get_degree();
  const Int_Vec node_neighbor_types = edges.neighbor_types_for_node(node->type_index);
  const int n_possible_neighbors = std::accumulate(node_neighbor_types.begin(),
                                                   node_neighbor_types.end(),
                                                   0,
                                                   [&blocks](int n, const int type) {
                                                     return n + blocks.size_of_type(type);
                                                   });

  const double epsB = eps * double(n_possible_neighbors);

  Node_Edge_Counts node_to_blocks = node->get_block_edge_counts();

  Edge_Map block_pair_counts;
  sum_edge_counts(block_pair_counts, old_block, new_block);
  sum_edge_counts(block_pair_counts, new_block);
  bool post_move = false;

  auto calc_edge_entropy_part = [&post_move, &old_block, &new_block, &node_degree]
                                (double ent_sum, const Edge_Map_Pair& edge_count){
    const double n_edges = edge_count.second;
    if(n_edges == 0.0) return ent_sum;

    // Self edge contributions need their edge counts doubled (because they are half edges)
    // and also need to have their total contribution divided by two
    // because they are getting counted twice as "frequently as the off-diagonal pairs
    const Edge& edge = edge_count.first;
    const double edge_scalar = edge.is_matching() ? 2.0: 1.0;

    const Node* g1 = edge.first();
    const Node* g2 = edge.second();

    double g1_degree = g1->get_degree();
    double g2_degree = g2->get_degree();

    if(post_move){
      if (g1 == old_block) {
        g1_degree -= node_degree;
      } else
      if (g1 == new_block) {
        g1_degree += node_degree;
      }

      if (g2 == old_block) {
        g2_degree -= node_degree;
      } else
      if (g2 == new_block) {
        g2_degree += node_degree;
      }
    }

    return ent_sum + n_edges * std::log(n_edges/(g1_degree*g2_degree)) / edge_scalar;;
  };

  const double pre_move_ent = std::accumulate(block_pair_counts.begin(),
                                              block_pair_counts.end(),
                                              0.0,
                                              calc_edge_entropy_part);

  // Update edge counts
  for (const auto& block_count : node_to_blocks) {
    // subtract contributions for when node was in old block
    block_pair_counts[Edge(old_block, block_count.first)] -= block_count.second * (block_count.first == old_block ? 2 : 1);

    // add edge to new blocks edge counts
    block_pair_counts[Edge(new_block, block_count.first)] += block_count.second * (block_count.first == new_block ? 2 : 1);
  }

  post_move = true;
  const double post_move_ent = std::accumulate(block_pair_counts.begin(),
                                               block_pair_counts.end(),
                                               0.0,
                                               calc_edge_entropy_part);


  const double prob_move_to_new = calc_move_prob(node_to_blocks, new_block, node_degree, eps, epsB);
  // Move node to proposed block
  swap_block(node, new_block, blocks, false);
  const double prob_return_to_old = calc_move_prob(node_to_blocks, old_block, node_degree, eps, epsB);

  // Return node to original block
  swap_block(node, old_block, blocks, false);

  return Move_Results(pre_move_ent - post_move_ent,
                      prob_return_to_old / prob_move_to_new);
}

