#ifndef __get_move_result_info_INCLUDED__
#define __get_move_result_info_INCLUDED__

// #include "Node_Container.h"
// #include "Edge_Container.h"
#include "calc_edge_entropy.h"
#include "calc_move_prob.h"
#include "swap_blocks.h"

using Edge = Ordered_Pair<Node*>;
using Edge_Map = std::map<Edge, int>;
using Edge_Vec = std::vector<Edge>;
using Node_Edge_Counts = std::map<Node*, int>;
using Edge_Count_Pair = std::pair<Node*, int>;
using Node_It = std::vector<Node*>::iterator;

double get_move_result_info(Node* node,
                          Node* new_block,
                          const Node_Container& nodes,
                          Node_Container& blocks,
                          const Edge_Container& edges,
                          const double eps = 0.1){

  Node* old_block = node->get_parent();
  // No need to go on if we're "swapping" to the same group
  if(new_block == old_block) return 0.0;


  const double node_degree = node->get_degree();
  const Int_Vec node_neighbor_types = edges.neighbor_types_for_node(node->type_index);
  const int n_possible_neighbors = std::accumulate(node_neighbor_types.begin(),
                                                   node_neighbor_types.end(),
                                                   0,
                                                   [&nodes](int n, const int type) {
                                                     return n + nodes.size_of_type(type);
                                                   });

  const double epsB = eps * double(n_possible_neighbors);
  Node_Edge_Counts node_to_blocks = node->get_block_edge_counts();

  const double pre_move_ent = calc_edge_entropy(nodes, edges, {old_block, new_block});
  const double pre_move_prob = calc_move_prob(node_to_blocks, new_block, node_degree, eps, epsB);

  // Move node to proposed block
  swap_block(node, new_block, blocks, false);

  const double post_move_ent = calc_edge_entropy(nodes, edges, {old_block, new_block});
  const double post_move_prob = calc_move_prob(node_to_blocks, old_block, node_degree, eps, epsB);

  return pre_move_ent - post_move_ent;
}



#endif
