#include <Rcpp.h>
using namespace Rcpp;

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

  return List::create(_["num_nodes"] = num_nodes,
                      _["num_edges"] = num_edges,
                      _["num_types"] = num_types);
}



/*** R

info_about_network(edges$pollinator, edges$flower,
                   nodes$id, nodes$type,
                   types$type, types$count)

*/
