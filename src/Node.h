#ifndef __NODE_INCLUDED__
#define __NODE_INCLUDED__

// [[Rcpp::plugins(cpp11)]]
#include <Rcpp.h>
#include <vector>

#include "vector_helpers.h"

class Node;

using Int_Vec = std::vector<int>;
using Node_Ptrs = std::vector<Node*>;
using string = std::string;

class Node
{
private:
  Node_Ptrs edges;     // Vector of pointers to every connected node

public:
  // Data
  int index;           // Index of this node in `nodes_*` vectors
  int type_index;      // Index of node type in `types_*` vectors
  Node_Ptrs children;  // Vector of pointers to every child node
  Node* parent_ref = nullptr;    // Index of block or parent node in next-level's `Node_Container`

  // Setters
  // ===========================================================================

  // Initialize the `index` and `type_index` data members
  Node(int i, int t) : index(i), type_index(t) {}


  Node(const Node& copied_node) = delete;             // Copy constructor
  Node& operator=(const Node& copied_node)  = delete; // Copy assignment
  Node(Node&& moved_node)  = delete;                  // Move constructor
  Node& operator=(Node&& moved_node) = delete;        // Move assignment

  // Append to `edges` vector a new edge integer
  void add_edge(Node * connected_node_ptr)
  {
    edges.push_back(connected_node_ptr);
  }

  void add_child(Node * child_node_ptr)
  {
    children.push_back(child_node_ptr);
  }

  void remove_child(Node * child_node_ptr)
  {
    delete_from_vector(children, child_node_ptr);
  }

  // Set the value of `parent_index` to a given integer
  void set_parent(Node * parent_node)
  {
    parent_ref = parent_node;
  }

  // Getters
  // ===========================================================================
  string get_id(const Rcpp::CharacterVector &nodes_id) const
  {
    return std::string(nodes_id.at(index));
  }

  string get_type(const Rcpp::CharacterVector &types_name) const
  {
    return std::string(types_name.at(type_index));
  }

  int get_degree() const
  {
    return edges.size();
  }

  // Comparison operators
  // ===========================================================================
  bool operator==(const Node &b) const
  {
    return index == b.index;
  }

  bool operator<(const Node &b) const
  {
    return index < b.index;
  }
};


#endif
