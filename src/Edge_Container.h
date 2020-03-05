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

    for (int i = 0; i < edges_from.size(); i++) {
      Node& from_node = node_id_to_index.at(string(edges_from[i]));
      Node& to_node = node_id_to_index.at(string(edges_to[i]));

      // Get edge type for this edge
      const auto edge_type = Edge_Type(from_node.type_index, to_node.type_index);

      // Either check it's allowed or add it to seen types
      if (types_specified) {
        if (edge_types.count(edge_type) == 0){
          stop("The edge type from edge "
                 + string(edges_from[i]) + " - "
                 + string(edges_to[i])
                 + " was not specified in allowed edge types");
        }
      } else {
        edge_types.insert(edge_type);
      }

      edges.emplace_back(from_node.index, to_node.index);

      // Add edges to nodes as well
      from_node.add_edge(to_node.index);
      to_node.add_edge(from_node.index);
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
