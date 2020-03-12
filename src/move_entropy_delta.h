#ifndef __MOVE_ENTROPY_DELTA_INCLUDED__
#define __MOVE_ENTROPY_DELTA_INCLUDED__

#include "Node_Container.h"
#include "Edge_Container.h"

using Edge = Ordered_Pair<Node*>;
using Edge_Vec = std::vector<Edge>;
using Edge_Map = std::map<Edge, int>;
using Node_Edge_Counts = std::map<Node*, int>;

template <typename T>
using Edge_Map_Pair = std::pair<T, int>;


struct Move_Probs {
  double pre = 0.0;
  double post = 0.0;
};

double move_entropy_delta(Node* node,
                          Node* new_block,
                          const Node_Container& nodes,
                          const Edge_Container& edges,
                          const double eps = 0.1){

  Node* old_block = node->get_parent();
  // No need to go on if we're "swapping" to the same group
  if(new_block == old_block) return 0.0;

  // Initialize a `pre_move_edge_map` and a `post_move_edge_map`s
  Edge_Map pre_move_edge_counts;
  Edge_Map post_move_edge_counts;

  // Similarly, gather edges for the node being moved
  Node_Edge_Counts node_edge_counts;

  // Scan down `edges` one by one to build pre and post edge count maps
  for (const auto& edge : edges.data()) {
    const Node* n1 = edge.first();
    const Node* n2 = edge.second();
    Node* g1 = n1->get_parent();
    Node* g2 = n2->get_parent();


    const bool g1_is_old = g1 == old_block;
    const bool g1_is_new = g1 == new_block;
    const bool g2_is_old = g2 == old_block;
    const bool g2_is_new = g2 == new_block;

    // If `n1->parent` or `n2->parent` is either `old_block` or `new_block`
    if(g1_is_old | g1_is_new | g2_is_old | g2_is_new) {
      const bool n1_is_node = n1 == node;
      const bool n2_is_node = n2 == node;

      // Add edge to node's edge counts if necessary
      if(n1_is_node) { node_edge_counts[g2]++; }
      if(n2_is_node) { node_edge_counts[g1]++; }

      // Add edge of `n1->parent` - `n2->parent` to `pre_move_edge_map`
      pre_move_edge_counts[Edge(g1, g2)]++;

      // If `n1` or `n2` is `node_i`, then swap `n*->parent` with `new_block` and add to `post_move_edge_map`
      if(n1_is_node) { g1 = new_block; }
      if(n2_is_node) { g2 = new_block; }
      post_move_edge_counts[Edge(g1, g2)]++;
    }
  }

  // Calculate the edge sum `pre_move_edge_map` and `post_move_edge_map`,
  // making sure to update the degree of each block in the post-move sum
  // if it is either `old_block` or `new_block`.
  const int node_degree = node->get_degree();
  const Int_Vec node_neighbor_types = edges.neighbor_types_for_node(node->type_index);
  const int n_possible_neighbors = std::accumulate(node_neighbor_types.begin(),
                                                   node_neighbor_types.end(),
                                                   0,
                                                   [&nodes](int total, const int type) {
                                                     return total + nodes.size_of_type(type);
                                                   });
  const double epsB = eps * n_possible_neighbors;

  auto calc_edge_entropy_part = [&old_block, &new_block, &node_degree](const Edge_Map_Pair<Edge>& edge_count, const bool is_post_move){
    const Edge& edge = edge_count.first;
    const Node* g1 = edge.first();
    const Node* g2 = edge.second();

    // Self edge contributions need their edge counts doubled (because they are half edges)
    // and also need to have their total contribution divided by two
    // because they are getting counted twice as "frequently as the off-diagonal pairs
    const double edge_scalar = edge.is_matching() ? 2.0: 1.0;
    const double n_edges = edge_count.second * edge_scalar;

    double g1_degree = g1->get_degree();
    double g2_degree = g2->get_degree();

    if(is_post_move){
      if(g1 == old_block) g1_degree -= node_degree;
      if(g1 == new_block) g1_degree += node_degree;
      if(g2 == old_block) g2_degree -= node_degree;
      if(g2 == new_block) g2_degree += node_degree;
    }

    const double entropy_portion =  n_edges * std::log(n_edges/(g1_degree*g2_degree)) / edge_scalar;

    return entropy_portion;
  };

  auto calc_move_probs = [&] (Move_Probs& probs, const Edge_Map_Pair<Node*>& edge_count) {

    Node* block = edge_count.first;
    const double prop_edges_to_block = edge_count.second/node_degree;

    const double pre_move_block_to_new = pre_move_edge_counts.at(Edge{block, new_block});
    const double post_move_block_to_old = pre_move_edge_counts.at(Edge{block, old_block});

    const double degree_pre = block->get_degree();
    double degree_post = degree_pre;
    if (block == new_block) degree_post += node_degree;
    if (block == old_block) degree_post -= node_degree;

    probs.pre  += prop_edges_to_block * (pre_move_block_to_new  + eps) / (degree_pre  + epsB);
    probs.post += prop_edges_to_block * (post_move_block_to_old + eps) / (degree_post + epsB);

    return probs;
  };

  Move_Probs start_probs;
  // Move_Probs move_probs = std::accumulate(node_edge_counts.begin(),
  //                                         node_edge_counts.end(),
  //                                         start_probs,
  //                                         calc_move_probs);

  double pre_move_edge_entropy = std::accumulate(pre_move_edge_counts.begin(),
                                                 pre_move_edge_counts.end(),
                                                 0.0,
                                                 [&](double sum, const Edge_Map_Pair<Edge>& edge_count){
                                                   return sum + calc_edge_entropy_part(edge_count, false);
                                                 });

  double post_move_edge_entropy = std::accumulate(post_move_edge_counts.begin(),
                                                  post_move_edge_counts.end(),
                                                  0.0,
                                                  [&](double sum, const Edge_Map_Pair<Edge>& edge_count){
                                                    return sum + calc_edge_entropy_part(edge_count, true);
                                                  });


  // Take difference of two sums
  return pre_move_edge_entropy - post_move_edge_entropy;
}

#endif
