// #include <Rcpp.h>
// #include "Edge_Container.h"
// #include "get_move_results.h"
// #include "swap_blocks.h"
// #include <random>
//
// using namespace Rcpp;
//
// using Ordered_Ints = Ordered_Pair<int>;
// // // [[Rcpp::export]]
// // List info_about_network(const CharacterVector nodes_id,
// //                         const CharacterVector nodes_type,
// //                         const CharacterVector edges_from,
// //                         const CharacterVector edges_to,
// //                         const CharacterVector types_name,
// //                         const IntegerVector types_count)
// // [[Rcpp::export]]
// List info_about_network()
// {
//   Random_Engine random_engine{};
//   random_engine.seed(42);
//
//   auto nodes_id   = Rcpp::CharacterVector{"n1", "n2", "n3", "n4", "n5", "n6"};
//   auto nodes_type = Rcpp::CharacterVector{ "a",  "a",  "a",  "a",  "a",  "a"};
//   auto types_name  = Rcpp::CharacterVector{"a"};
//   auto types_count = Rcpp::IntegerVector{    6};
//
//   const Rcpp::CharacterVector edges_from{"n1", "n1", "n1", "n1", "n2", "n2", "n2", "n3", "n3", "n4", "n4", "n5"};
//   const Rcpp::CharacterVector   edges_to{"n2", "n3", "n4", "n5", "n3", "n4", "n5", "n4", "n6", "n5", "n6", "n6"};
//
//   auto nodes = Node_Container(nodes_id, nodes_type, types_name, types_count);
//   auto edges = Edge_Container(edges_from, edges_to, nodes_id, nodes);
//   auto blocks = Node_Container(6, nodes, random_engine); // One block per node
//
//   // Grab nodes by their ids
//   auto node_by_id = nodes.get_id_to_node_map(nodes_id);
//   Node* n1 = node_by_id.at("n1");
//   Node* n2 = node_by_id.at("n2");
//   Node* n3 = node_by_id.at("n3");
//   Node* n4 = node_by_id.at("n4");
//   Node* n5 = node_by_id.at("n5");
//   Node* n6 = node_by_id.at("n6");
//
//   // n1 and n2 share a block
//   swap_block(n2, n1->get_parent(), blocks);
//   // n3 and n4 share a block
//   swap_block(n4, n3->get_parent(), blocks);
//   // n5 and n6 share a block
//   swap_block(n6, n5->get_parent(), blocks);
//
//
//   // Group c is n5 and n6's parent
//   Node* group_c = n5->get_parent();
//
//   // Propose move of n4 to group c
//   const auto move_results = get_move_results(n4, group_c, nodes, blocks, edges, 0.5);
//
//
//   return List::create(_["num_nodes"] = nodes.size(),
//                       _["num_types"] = nodes.type_to_index.size(),
//                       _["move delta"] = move_results.entropy_delta);
// }
//
//
//
//
// /*** R
// library(magrittr)
// library(sbmR)
//
// edges <- clements_pollinators %>% dplyr::sample_n(100)
//
// nodes <- dplyr::bind_rows(dplyr::tibble(id = edges$pollinator,
//                                         type = "pollinator"),
//                           dplyr::tibble(id = edges$flower,
//                                         type = "flower")) %>%
//   dplyr::distinct() %>%
//   dplyr::arrange(type)
//
//
// types <- dplyr::count(nodes, type, name = "count")
//
// # info_about_network(nodes$id, nodes$type,
// #                    edges$pollinator, edges$flower,
// #                    types$type, types$count)
//
// info_about_network()
//
// */
