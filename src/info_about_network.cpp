#include <Rcpp.h>
#include "Edge_Container.h"
#include "Ordered_Pair.h"

using namespace Rcpp;

using Ordered_Ints = Ordered_Pair<int>;
// [[Rcpp::export]]
List info_about_network(const CharacterVector nodes_id,
                        const CharacterVector nodes_type,
                        const CharacterVector edges_from,
                        const CharacterVector edges_to,
                        const CharacterVector types_name,
                        const CharacterVector types_count)
{
  auto nodes = Node_Container(nodes_id, nodes_type, types_name);
  auto edges = Edge_Container(edges_from, edges_to, nodes_id, nodes);

  return List::create(_["num_nodes"] = nodes.size(),
                      _["num_types"] = nodes.num_types(),
                      _["num_edges"] = edges.size(),
                      _["first type start"] = nodes.get_nodes_of_type(0).start_index,
                      _["first type size"] = nodes.get_nodes_of_type(0).size,
                      _["second type start"] = nodes.get_nodes_of_type(1).start_index,
                      _["second type size"] = nodes.get_nodes_of_type(1).size);
}



/*** R

info_about_network(nodes$id, nodes$type,
                   edges$pollinator, edges$flower,
                   types$type, types$count)

*/
