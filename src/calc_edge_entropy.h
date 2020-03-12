#pragma once

#include "Node_Container.h"
#include "Edge_Container.h"
#include "swap_blocks.h"

using Edge = Ordered_Pair<Node*>;
using Edge_Map = std::map<Edge, int>;
using Edge_Map_Pair = std::pair<Edge, int>;

double calc_edge_entropy(const Node_Container& nodes,
                         const Edge_Container& edges,
                         const std::vector<Node*>& blocks_to_consider = {}){
  Edge_Map edge_counts;

  // We only look at a subset of edges if our blocks to consider vector is not empty
  const bool no_subset = blocks_to_consider.empty();

  // Scan down `edges` one by one to build edge count map
  for (const auto& edge : edges.data()) {
    Node* g1 = edge.first()->get_parent();
    Node* g2 = edge.second()->get_parent();

    // Determine if we want to count this edge or not
    const bool count_edge = no_subset || std::any_of(blocks_to_consider.begin(),
                                                     blocks_to_consider.end(),
                                                     [&g1, &g2](Node* node){
                                                       return node == g1 | node == g2;
                                                     });;

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
