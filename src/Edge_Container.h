#ifndef __EDGE_CONTAINER_INCLUDED__
#define __EDGE_CONTAINER_INCLUDED__

// [[Rcpp::plugins(cpp11)]]
#include <Rcpp.h>
#include "Ordered_Pair.h"
#include "Node_Container.h"
#include <vector>

using namespace Rcpp;
using string = std::string;
using Edge_Type = Ordered_Pair<int>;

class Edge_Container {
public:
  // Data
  std::vector<Ordered_Pair<int>> edges; // Mirrors order of `edges_*` vectors
  Ordered_Pair_Set<int> edge_types;  // Contains all the observed or allowed edge types
  bool types_specified = false; // Did the user explicitly state the allowed edge types

  // Setters
  // ===========================================================================
  Edge_Container(const CharacterVector& edges_from,
                 const CharacterVector& edges_to,
                 const CharacterVector& nodes_id,
                 Node_Container &nodes){

    // We need to quickly go from a node string id to its integer index in nodes_* vectors
    std::unordered_map<string, Node&> node_id_to_index;
    for (int i = 0; i < nodes_id.size(); i++) {
      node_id_to_index.emplace(nodes_id[i], nodes.at(i));
    }

    auto get_node = [&](const string& node_id, const int edge_index) {
      const auto node_loc = node_id_to_index.find(node_id);
      if (node_loc == node_id_to_index.end()) {
        stop("Node " + node_id + " from edges "
               + string(edges_from[edge_index]) + " - " + string(edges_to[edge_index])
               + " was not provided in list of nodes");
      }
      return &(node_loc->second);
    };

    const bool multipartite_nodes = nodes.is_multipartite();

    for (int i = 0; i < edges_from.size(); i++) {

      Node * from_node = get_node(string(edges_from[i]), i);
      Node * to_node   = get_node(string(edges_to[i]), i);

      // We only need to check edge types if we have multiple node types
      if (multipartite_nodes) {
        // Get edge type for this edge
        const auto edge_type = Edge_Type(from_node->type_index, to_node->type_index);

        const bool not_in_edge_types = edge_types.count(edge_type) == 0;

        if (edge_type.is_matching()){
          stop("Error for edge " + string(edges_from[i]) + " - " + string(edges_to[i])
                 + ": Can't have an edge between two nodes of the same type in multipartite networks");
        }

        // Only need to take action if this is a new edge type
        if (not_in_edge_types) {
          if (types_specified) {
            stop("The edge type from edge "+ string(edges_from[i]) + " - "
                 + string(edges_to[i]) + " was not specified in allowed edge types");
          } else {
            // Make sure that this edge doesn't violate the rules of multipartite edges of being between nodes of the same type
            edge_types.insert(edge_type);
          }
        }
      }


      edges.emplace_back(from_node->index, to_node->index);

      // Add edges to nodes as well
      from_node->add_edge(to_node->index);
      to_node->add_edge(from_node->index);
    }

    // If we're in a unipartite network we never filled out the edge types, so do that here.
    if (!multipartite_nodes) {
      edge_types.insert(Edge_Type(0, 0));
    }

  }
  // Getters
  // ===========================================================================
  int size() const {
    return edges.size();
  }
};


// ## `Edge_Container::`
// __Data__
// `vec<Edge>      edges`: Vector containing `Edges` that mirror order of `edges_*` vectors
// `set<Edge_Type> edge_types`: Unordered set containing all the observed or allowed edge types
// `bool           specified_types`: Are edges to be build with specified types or do we figure out type pairs with data?

// __Setters__
// `Edge_Container(edges_*, nodes_id, allowed_edges_*)`:
//   - Build unordered_map of node id string -> index in `nodes_id`
//   - If we have `allowed_edges_*`
//     - Build `edge_types` from allowed and freeze it using `specified_types = true`
//   - Loop through `edges_*`
//     - Build `Edge` using id-to-string map
//     - Grab edge type using `Edge.get_type()`
//       - If `specified_types`, make sure it's in `edge_types`
//       - Otherwise just `insert()` it into `edge_types`

#endif
