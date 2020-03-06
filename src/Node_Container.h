#ifndef __NODE_CONTAINER_INCLUDED__
#define __NODE_CONTAINER_INCLUDED__

// [[Rcpp::plugins(cpp11)]]
#include <map>
#include <memory>
#include <Rcpp.h>
#include "Node.h"

using namespace Rcpp;

struct Type_Info
{
  int start_index; // Start index in `nodes` for nodes of this type
  int size = -1;   // How many nodes there are of this type
  Type_Info(int i, int s)
      : start_index(i), size(s)
  {
  }
};


using Type_Info_Map = std::map<int, Type_Info>;
using Node_Unique_Ptr = std::unique_ptr<Node>;
using Node_Vec = std::vector<Node_Unique_Ptr>;
using Node_Type_Vec = std::vector<Node_Vec>;

class Node_Container
{
private:
  const void check_for_type(const int type_i) const {
    if (type_i > nodes.size()) stop("Invalid type");
  }
public:
  // Data
  Node_Type_Vec nodes;  // Vector of vectors type->nodes of type ordering
  Type_Info_Map type_info; // Keyed by type_index, returns info about that type in relation to internal nodes vector
  std::map<string, int> type_to_index;
  // Setters
  // ===========================================================================
  Node_Container(const CharacterVector &nodes_id,
                 const CharacterVector &nodes_type,
                 const CharacterVector &types_name,
                 const IntegerVector   &types_count)
  {
    const int num_types = types_name.size();

    // Reserve proper number of sub vectors for nodes based on number of types
    nodes = Node_Type_Vec(num_types);

    // Build a map to go from type name to index for faster look-up
    for (int i = 0; i < num_types; i++)
    {
      type_to_index.emplace(types_name[i], i);  // Fill in type-to-index map entry for type
      nodes[i].reserve(types_count[i]);         // Reserve appropriate size for nodes vector for this type
    }

    for (int i = 0; i < nodes_id.size(); i++)
    {
      // Find index for type
      const auto type_index_it = type_to_index.find(std::string(nodes_type[i]));

      // Make sure it fits what we were given
      if (type_index_it == type_to_index.end())
        Rcpp::stop("Node " + string(nodes_id[i]) + " has type (" + string(nodes_type[i]) + ") not found in provided node types");

      // Update the reference to the
      const int type_index = type_index_it->second;

      // Build a new node wrapped in smart pointer in it's type vector
      nodes[type_index].emplace_back(new Node(i, type_index));
    }

  }

  // Getters
  // ===========================================================================
  // Node& at(const int i){
  //   if (i < -1 | i > nodes.size()){
  //     stop("Invalid node index.");
  //   }
  //   return nodes[i];
  // }

  const int size_of_type(const int type_i) const {
    check_for_type(type_i);

    return nodes[type_i].size();
  }

  Node_Vec& get_nodes_of_type(const int type_i)
  {
    check_for_type(type_i);

    return nodes[type_i];
  }


  Node& at(const int type_i, const int i){

    Node_Vec& nodes_of_type = get_nodes_of_type(type_i);

    if (i > nodes_of_type.size()){
      stop("Invalid node index.");
    }

    return *nodes_of_type[i];
  }

  int size() const {
    int total_num_nodes = 0;
    for(const auto& nodes_of_type : nodes){
      total_num_nodes += nodes_of_type.size();
    }
    return total_num_nodes;
  }

  int num_types() const {
    return nodes.size();
  }

  bool is_multipartite() const {
    return num_types() > 1;
  }


};

#endif
