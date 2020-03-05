#ifndef __EDGE_CONTAINER_INCLUDED__
#define __EDGE_CONTAINER_INCLUDED__

// [[Rcpp::plugins(cpp11)]]
#include <Rcpp.h>
#include "Ordered_Pair.h";
#include <vector>

using string = std::string;
using Edge_Types = std::unordered_set<>

    class Edge_Container
{
public:
  // Data
  std::vector<Ordered_Pair<int>> edges; // Mirrors order of `edges_*` vectors

  // Setters
  // ===========================================================================

  // Getters
  // ===========================================================================
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
