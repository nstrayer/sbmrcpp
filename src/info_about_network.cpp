#include <Rcpp.h>
#include "Node.h"

using namespace Rcpp;

using Node_Vec = std::vector<Node>;

// [[Rcpp::export]]
List info_about_network(const CharacterVector nodes_id,
                        const CharacterVector nodes_type,
                        const CharacterVector edges_from,
                        const CharacterVector edges_to,
                        const CharacterVector types_name,
                        const CharacterVector types_count)
{
  const int num_nodes = nodes_id.size();
  const int num_edges = edges_from.size();
  const int num_types = types_name.size();

  Node_Vec nodes;
  nodes.reserve(num_nodes);
  for(int i = 0; i < num_nodes; i++){
    nodes.emplace_back(i, i);
  }



  return List::create(_["num_nodes"] = num_nodes,
                      _["node_vec_s"] = nodes.size(),
                      _["num_edges"] = num_edges,
                      _["num_types"] = num_types);
}



/*** R

info_about_network(edges$pollinator, edges$flower,
                   nodes$id, nodes$type,
                   types$type, types$count)

*/
