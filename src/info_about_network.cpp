#include <Rcpp.h>
#include "Node_Container.h"
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
                        const IntegerVector types_count)
{
  // Initialize a random engine and seed
  Random_Engine random_engine{};
  random_engine.seed(42);

  auto nodes = Node_Container(nodes_id, nodes_type, types_name, types_count);
  auto edges = Edge_Container(edges_from, edges_to, nodes_id, nodes);

  // Assign random blocks
  auto blocks = Node_Container(5, nodes, random_engine);

  // Get reference to first node in network
  Node * n1 = &nodes.at(0,0);

  Rcout << "Got node " << n1->get_id(nodes_id) << " with degree of " << n1->get_degree() << std::endl;

  Rcout << "Random neighbor: " << n1->get_random_neighbor(random_engine)->get_id(nodes_id) << std::endl;
// //
//   // Get a proposed move for node
//   Node * proposed_block = propose_move(n1, blocks, random_engine, 0.1);





  return List::create(_["num_nodes"] = nodes.size(),
                      _["num_types"] = nodes.type_to_index.size()
                      // _["Random Selection"] = random_selection
                      // // _["num_edges"] = edges.size(),
                      // _["first type start"] = nodes.get_nodes_of_type(0).start_index,
                      // _["first type size"] = nodes.get_nodes_of_type(0).size,
                      // _["second type start"] = nodes.get_nodes_of_type(1).start_index,
                      // _["second type size"] = nodes.get_nodes_of_type(1).size
                        );
}




/*** R
library(magrittr)
library(sbmR)

edges <- clements_pollinators %>% dplyr::sample_n(100)

nodes <- dplyr::bind_rows(dplyr::tibble(id = edges$pollinator,
                                        type = "pollinator"),
                          dplyr::tibble(id = edges$flower,
                                        type = "flower")) %>%
  dplyr::distinct() %>%
  dplyr::arrange(type)


types <- dplyr::count(nodes, type, name = "count")

info_about_network(nodes$id, nodes$type,
                   edges$pollinator, edges$flower,
                   types$type, types$count)

*/
