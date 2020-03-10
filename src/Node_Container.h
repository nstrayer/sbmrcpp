#ifndef __NODE_CONTAINER_INCLUDED__
#define __NODE_CONTAINER_INCLUDED__

// [[Rcpp::plugins(cpp11)]]
#include <map>
#include <memory>

#include <Rcpp.h>
#include "Node.h"

using namespace Rcpp;

// Two dimensional index for a Node in a node container
struct Node_Loc {
  // Index at first level of a node container's nodes vector (aka type)
  int type_index = -1;
  // Index inside the node's vector for a given type
  int nodes_index = -1;
  Node_Loc(int t, int i) : type_index(t), nodes_index(i) {}
  bool operator<(const Node_Loc& a) {
    return type_index == a.type_index ? nodes_index < a.nodes_index
                                      : type_index < a.type_index;
  }
  bool operator=(const Node_Loc& a) {
    return (type_index == a.type_index) & (nodes_index < a.nodes_index);
  }
};

using Node_Unique_Ptr = std::unique_ptr<Node>;
using Node_Vec = std::vector<Node_Unique_Ptr>;
using Node_Type_Vec = std::vector<Node_Vec>;
using Id_to_Node_Map = std::unordered_map<string, Node*>;

class Node_Container {
 private:
  bool are_block_nodes = false;
  int block_index = 0; // Every time a block is added this increases, guarenteing we have unique indices for blocks
  int n_types;

  const void check_for_type(const int type_i) const {
    if (type_i >= nodes.size())
      stop("Invalid type");
  }

  void add_node(const int index,
                const int type_index,
                const int n_types) {
    nodes[type_index].emplace_back(new Node(index, type_index, n_types));
  }

 public:
  // Data
  Node_Type_Vec nodes;  // Vector of vectors type->nodes of type ordering
  std::map<string, int> type_to_index;

  // Setters
  // ===========================================================================
  Node_Container(const CharacterVector& nodes_id,
                 const CharacterVector& nodes_type,
                 const CharacterVector& types_name,
                 const IntegerVector& types_count){
    n_types = types_name.size();

    // Reserve proper number of sub vectors for nodes based on number of types
    nodes = Node_Type_Vec(n_types);

    // Build a map to go from type name to index for faster look-up
    for (int i = 0; i < n_types; i++) {
      // Fill in type-to-index map entry for type
      type_to_index.emplace(types_name[i], i);

      // Reserve appropriate size for nodes vector for this type
      nodes[i].reserve(types_count[i]);
    }


    for (int i = 0; i < nodes_id.size(); i++) {
      // Find index for type
      const auto type_index_it = type_to_index.find(std::string(nodes_type[i]));

      // Make sure it fits what we were given
      if (type_index_it == type_to_index.end())
        Rcpp::stop("Node " + string(nodes_id[i]) + " has type (" +
                   string(nodes_type[i]) +
                   ") not found in provided node types");


      // Build a new node wrapped in smart pointer in it's type vector
      add_node(i, type_index_it->second, types_name.size());
    }
  }

  Node_Container(const int num_blocks,
                 Node_Container& child_nodes,
                 Random_Engine& random_engine) {
    are_block_nodes = true;
    n_types = child_nodes.num_types();
    // Initialize `nodes` vec proper number of types
    nodes = Node_Type_Vec(n_types);

    // Loop over types
    for (int type_i = 0; type_i < n_types; type_i++) {
      // Pull reference to children nodes of this type
      auto& child_nodes_of_type = child_nodes.get_nodes_of_type(type_i);

      if (num_blocks > child_nodes_of_type.size()) {
        stop("Can't initialize more blocks than there are nodes of a given type");
      }

      // Reserve elements for new nodes
      auto& blocks_for_type = nodes[type_i];
      blocks_for_type.reserve(num_blocks);

      for (int i = 0; i < num_blocks; i++) {
        // Build a new block node wrapped in smart pointer in it's type vector
        add_node(block_index, type_i, n_types);
        block_index++;
      }

      // Shuffle child nodes
      std::shuffle(child_nodes_of_type.begin(), child_nodes_of_type.end(),
                   random_engine);

      // Loop through now shuffled children nodes
      for (int i = 0; i < child_nodes_of_type.size(); i++) {
        Node* parent_block = blocks_for_type[i % num_blocks].get();
        Node* child_node = child_nodes_of_type[i].get();

        // Add blocks one at a time, looping back after end to each node
        child_node->set_parent(parent_block);

        // Add child to parent block
        parent_block->add_child(child_node);

        // Dump all the edges from child to this parent
        parent_block->add_edges(child_node->get_edges());
      }  // End block to child node assignment
    }    // End loop over node types
  }      // End constructor

  // Getters
  // ===========================================================================
  const int size_of_type(const int type_i) const {
    check_for_type(type_i);

    return nodes[type_i].size();
  }

  Node_Vec& get_nodes_of_type(const int type_i) {
    check_for_type(type_i);

    return nodes[type_i];
  }

  Node* at(Node_Loc location) {
    Node_Vec& nodes_of_type = get_nodes_of_type(location.type_index);

    if (location.nodes_index >= nodes_of_type.size()) {
      stop("Invalid node index.");
    }

    return nodes_of_type[location.nodes_index].get();
  }

  Node* at(const int type_i, const int node_i) { return at(Node_Loc(type_i, node_i));}

  int size() const { return total_num_elements(nodes); }

  int num_types() const { return n_types; }

  bool is_multipartite() const { return n_types > 1; }

  Id_to_Node_Map get_id_to_node_map(const CharacterVector& nodes_id) {

    if(are_block_nodes) stop("Can't get ids to block nodes");

    Id_to_Node_Map id_to_loc;

    for (const auto& type_vec : nodes) {
      for (const auto& node : type_vec) {
        id_to_loc.emplace(nodes_id[node->index], node.get());
      }
    }

    return id_to_loc;
  }
};

#endif
