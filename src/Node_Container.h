#ifndef __NODE_CONTAINER_INCLUDED__
#define __NODE_CONTAINER_INCLUDED__

// [[Rcpp::plugins(cpp11)]]
#include <map>
#include <Rcpp.h>
#include "Node.h"

struct Type_Info
{
  int start_index; // Start index in `nodes` for nodes of this type
  int size = -1;   // How many nodes there are of this type
  Type_Info(int i, int s)
      : start_index(i), size(s)
  {
  }
};

using namespace Rcpp;

using Type_Info_Map = std::map<int, Type_Info>;
using Node_Vec = std::vector<Node>;

class Node_Container
{
public:
  // Data
  Node_Vec nodes;          // Same order as in `node_*` inputs.
  Type_Info_Map type_info; // Keyed by type_index, returns info about that type in relation to internal nodes vector

  // Setters
  // ===========================================================================
  Node_Container(const Rcpp::CharacterVector &nodes_id,
                 const Rcpp::CharacterVector &nodes_type,
                 const Rcpp::CharacterVector &types_name)
  {
    const int num_nodes = nodes_id.size();
    const int num_types = types_name.size();

    // Build a map to go from type name to index for faster lookup
    std::map<string, int> type_to_index;
    Rcout << "Building type to index map..." << std::endl;
    for (int i = 0; i < num_types; i++)
    {
      Rcout << "   " << types_name[i] << " - " << i << std::endl;
      type_to_index.emplace(types_name[i], i);
    }

    // Keep track of last seen type (starting at first one)
    int type_start_index_in_nodes = 0;
    string current_type = string(nodes_type[0]);
    int current_type_index = type_to_index.at(current_type);

    for (int i = 0; i < num_nodes; i++)
    {

      const auto node_type = std::string(nodes_type[i]);

      const bool new_type = current_type != node_type;
      const bool last_node = i == (num_nodes - 1);

      // We've finished a type's nodes if we have a new type or its the last node
      if (new_type | last_node)
      {
        const int end_index = last_node ? i : i - 1;
        const int num_nodes_of_type = (end_index - type_start_index_in_nodes) + 1;

        // Build type_info struct for the just finished class
        type_info.emplace(current_type_index,
                          Type_Info(type_start_index_in_nodes, num_nodes_of_type));

        // now we setup for the next type's nodes
        current_type = node_type;
        type_start_index_in_nodes = i;
      }

      if (new_type)
      {
        if (current_type > node_type)
          Rcpp::stop("Nodes not supplied in order. Make sure not to modify the node order");

        // Find index for type
        const auto type_index_it = type_to_index.find(node_type);

        // Make sure it fits what we were given
        if (type_index_it == type_to_index.end())
          Rcpp::stop("Node " + string(nodes_id[i]) + " has type (" + string(nodes_type[i]) + ") not found in provided node types");

        current_type_index = type_index_it->second;
      }

      // Initialize node in next slot in nodes vector
      nodes.emplace_back(i, current_type_index);
    }
  }

  // Getters
  // ===========================================================================
  Type_Info get_nodes_of_type(const int type_int) const
  {
    return type_info.at(type_int);
  }

  int size() const {
    return nodes.size();
  }

  Node& at(const int i){
    if (i < -1 | i > nodes.size()){
      stop("Invalid node index.");
    }
    return nodes[i];
  }


};

// ## `Node_Container::`
// __Data__
// `vec<Node>       nodes`: Vector of `Node`s, same order as in `node_*` inputs.
// `map<int, Type>  type_info`: unordered_map keyed by a `type_index` that contains a struct with the following info
//   - `start_index`: Start index in `nodes` for nodes of this type
//   - `size`: How many nodes there are of type in `nodes`

// __Setters__
// `Node_Container(nodes_id, nodes_type, types_name, types_count)`: Build a simple node vector one at a time.
//    - Test type-order assumption while doing so
//    - At end of a type's set of nodes, build the entry for type in `type_info` map
// `add_edges(Edge_Container)`: Fill in edges for all nodes by using `Node::add_edge()` method for both sides of edge

// __Getters__
// `(int, int) get_nodes_of_type(type_int)`: Return pair giving start and end index of type in `nodes` (uses `type_info` map)

#endif
