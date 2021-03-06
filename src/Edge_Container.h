#ifndef __EDGE_CONTAINER_INCLUDED__
#define __EDGE_CONTAINER_INCLUDED__

// [[Rcpp::plugins(cpp11)]]
#include <Rcpp.h>
#include <vector>
#include "Node_Container.h"
#include "Ordered_Pair.h"

using namespace Rcpp;
using string = std::string;
using Edge_Type = Ordered_Pair<int>;
using Edge_Vec = std::vector<Ordered_Pair<Node*>>;
using Int_Vec = std::vector<int>;

class Edge_Container {
private:
  // Mirrors order of `edges_*` vectors
  Edge_Vec edges;
  // Did the user explicitly state the allowed edge types
  bool types_specified = false;
  // Map so we can easily get all potential neighbor types for a type
  std::map<int, Int_Vec> neighbor_types;

public:
  // Setters
  // ===========================================================================
  Edge_Container(const CharacterVector& edges_from,
                 const CharacterVector& edges_to,
                 const CharacterVector& nodes_id,
                 Node_Container& nodes,
                 const CharacterVector& allowed_types_from = {},
                 const CharacterVector& allowed_types_to = {}) {

    Ordered_Pair_Set<int> edge_types;

    // If our edge_types_* vectors are not empty, we need to build allowed types
    if (allowed_types_from.size() != 0) {
      types_specified = true;

      for (int i = 0; i < allowed_types_from.size(); i++) {
        edge_types.insert(Edge_Type(nodes.type_to_index.at(string(allowed_types_from[i])),
                                    nodes.type_to_index.at(string(allowed_types_to[i]))));
      }
    }

    // We need to quickly go from a node string id to its integer index in
    // nodes_* vectors
    auto id_to_node = nodes.get_id_to_node_map(nodes_id);

    auto get_node = [&](const string& node_id, const int edge_index) {
      const auto node_loc = id_to_node.find(node_id);
      if (node_loc == id_to_node.end()) {
        stop("Node " + node_id + " from edges " +
             string(edges_from[edge_index]) + " - " +
             string(edges_to[edge_index]) +
             " was not provided in list of nodes");
      }
      return node_loc->second;
    };

    const bool multipartite_nodes = nodes.is_multipartite();

    for (int i = 0; i < edges_from.size(); i++) {
      Node* from_node = get_node(string(edges_from[i]), i);
      Node* to_node = get_node(string(edges_to[i]), i);

      // We only need to check edge types if we have multiple node types
      if (multipartite_nodes) {
        // Get edge type for this edge
        const auto edge_type =
            Edge_Type(from_node->type_index, to_node->type_index);

        const bool not_in_edge_types = edge_types.count(edge_type) == 0;

        if (edge_type.is_matching()) {
          stop("Error for edge " + string(edges_from[i]) + " - " +
               string(edges_to[i]) +
               ": Can't have an edge between two nodes of the same type in "
               "multipartite networks");
        }

        // Only need to take action if this is a new edge type
        if (not_in_edge_types) {
          if (types_specified) {
            stop("The edge type from edge " + string(edges_from[i]) + " - " +
                 string(edges_to[i]) +
                 " was not specified in allowed edge types");
          } else {
            // Make sure that this edge doesn't violate the rules of
            // multipartite edges of being between nodes of the same type
            edge_types.insert(edge_type);
          }
        }
      }

      edges.emplace_back(from_node, to_node);

      // Add edges to nodes as well
      from_node->add_edge(to_node);
      to_node->add_edge(from_node);
    }

    // Build map to go from edge type to allowed neighbor types
    if (multipartite_nodes) {
      for (const auto& edge_type : edge_types) {
        neighbor_types[edge_type.first()].push_back(edge_type.second());
        neighbor_types[edge_type.second()].push_back(edge_type.first());
      }
    } else {
      // No need to be fancy with unipartite networks
      neighbor_types[0].push_back(0);
    }

  }

  // Getters
  // ===========================================================================
  int size() const { return edges.size(); }

  const Edge_Vec& data() const { return edges; }

  Int_Vec neighbor_types_for_node(const int node_type) const {
    return neighbor_types.at(node_type);
  }
};

#endif
