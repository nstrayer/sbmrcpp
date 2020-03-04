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
    Type_Info(int i, int s) : start_index(i), size(s) {}
};

using Type_Info_Map = std::map<int, Type_Info>;

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
        for (int i = 0; i < num_types; i++)
        {
            type_to_index.emplace(types_name[i], i);
        }

        // Keep track of last seen type (starting at first one)
        int current_type_start_index = 0;
        int current_type_index;
        std::string current_type;

        for (int i = 0; i < num_nodes; i++)
        {
            const auto node_type = std::string(nodes_type[i]);
            
            const bool first_node = i == 0;
            const bool last_node = i == (num_nodes - 1);
            const bool new_type = current_type != node_type;

            if (new_type)
            {
                if (current_type > node_type)
                {
                    Rcpp::stop("Nodes not supplied in order. Make sure not to modify the node order");
                }

                // Find index for type
                const auto type_index_it = type_to_index.find(new_type);
                // Make sure it fits what we were given
                if (type_index_it == type_to_index.end())
                {
                    Rcpp::stop("Node " + string(nodes_name[i]) + " has type (" + string(nodes_type[i]) + ") not found in provided node types");
                }

                current_type_index = *type_index_it;
            }

            if (new_type & !first_node | last_node){
                const int num_nodes_of_type = i - current_type_start_index;

                // Build type_info struct for the just finished class
                type_info.emplace(current_type_index, Type_Info(current_type_start_index, num_nodes_of_type));

                // Update current type to be the new type for the current node
                current_type = current_type;
                current_type_start_index = i;
            }

            // Initialize node in next slot in nodes vector
            nodes.emplace_back(i, *type_index_it);
        }
    }
    // Getters
    // ===========================================================================
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
