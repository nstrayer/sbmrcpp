#pragma once
// Calculate the portion of entropy for network that is contributed to by the
// block assignments. This is given by the formula:
//   1/2 * [Sum over all pairs of blocks (r,s) : e_rs * ln(e_rs/(e_r*e_s))]
// Where e_rs    : number of edges between blocks r and s
//       e_{r,s} : total number of edges for blocks r or s (aka its degree)
//
// By using the edge list here we avoid double counting pairs of blocks (r->s,
// and s->r) and thus we don't have to divide by 2.
// However, as a result we need to double the contribution of pairs of same
// blocks (e.g. r->r) because in the original formula they are also counted only
// once.
//
// The arguments `new_block` and `old_block` are used to let the model know we only
// need to calculate the entropy portion associated with edges connected to the
// old or new blocks. This is useful when we are calculating the entropy delta
// caused by a move and don't need to recalculate the block pairs that have no
// change.

#include "Node_Container.h"
#include "Edge_Container.h"
#include "swap_blocks.h"

using Edge = Ordered_Pair<Node*>;
using Edge_Map = std::map<Edge, int>;
using Edge_Map_Pair = std::pair<Edge, int>;

double calc_edge_entropy(const Node_Container& nodes,
                         const Edge_Container& edges,
                         const Node* new_block  = nullptr,
                         const Node* old_block  = nullptr){
  Edge_Map edge_counts;

  // We only look at a subset of edges if our blocks to consider vector is not empty
  const bool no_subset = new_block == nullptr;

  // Scan down `edges` one by one to build edge count map
  for (const auto& edge : edges.data()) {
    Node* g1 = edge.first()->get_parent();
    Node* g2 = edge.second()->get_parent();

    // Determine if we want to count this edge or not
    // We count all edges if there's no subset, otherwise we only count
    // those that involve one of the new or old blocks
    const bool count_edge = no_subset
                         || g1 == new_block
                         || g1 == old_block
                         || g2 == new_block
                         || g2 == old_block;

    if(count_edge) edge_counts[Edge(g1, g2)]++;
  }

  auto calc_edge_entropy_part = [](double ent_sum, const Edge_Map_Pair& edge_count){
    const Edge& edge = edge_count.first;

    // Self edge contributions need their edge counts doubled (because they are half edges)
    // and also need to have their total contribution divided by two
    // because they are getting counted twice as "frequently as the off-diagonal pairs
    const double edge_scalar = edge.is_matching() ? 2.0: 1.0;
    const double n_edges = edge_count.second * edge_scalar;
    const double g1_degree = edge.first()->get_degree();
    const double g2_degree = edge.second()->get_degree();

    return ent_sum + n_edges * std::log(n_edges/(g1_degree*g2_degree)) / edge_scalar;;
  };

  return std::accumulate(edge_counts.begin(),
                         edge_counts.end(),
                         0.0,
                         calc_edge_entropy_part);

}
