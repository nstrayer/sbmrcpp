#ifndef __NODE_INCLUDED__
#define __NODE_INCLUDED__

// [[Rcpp::plugins(cpp11)]]
#include <Rcpp.h>
#include <vector>

#include "vector_helpers.h"

using namespace Rcpp;
class Node;

using Node_Ptrs = std::vector<Node*>;
using Node_Type_Vecs = std::vector<Node_Ptrs>;
using string = std::string;

class Node {
 private:
  Node_Type_Vecs edges;        // Vector of pointers to every connected node
  Node* parent_ref = nullptr;  // Index of block or parent node in next-level's
                               // `Node_Container`

 public:
  // Data
  int index;           // Index of this node in `nodes_*` vectors
  int type_index;      // Index of node type in `types_*` vectors
  Node_Ptrs children;  // Vector of pointers to every child node

  // Setters
  // ===========================================================================

  // Initialize the `index` and `type_index` data members
  Node(int i, int t, int n_t) : index(i), type_index(t) {
    edges = Node_Type_Vecs(n_t);
  }

  Node(const Node& copied_node) = delete;             // Copy constructor
  Node& operator=(const Node& copied_node) = delete;  // Copy assignment
  Node(Node&& moved_node) = delete;                   // Move constructor
  Node& operator=(Node&& moved_node) = delete;        // Move assignment

  // Append to pointer to connected node to proper type edges vector
  void add_edge(Node* node_ptr) { edges[node_ptr->type_index].push_back(node_ptr); }

  void add_child(Node* child_node_ptr) { children.push_back(child_node_ptr); }

  void remove_child(Node* child) { delete_from_vector(children, child); }

  // Set the value of `parent_index` to a given integer
  void set_parent(Node* parent_node) { parent_ref = parent_node; }

  // Getters
  // ===========================================================================
  int get_degree() const { return total_num_elements(edges); }

  Node* get_parent() const { return parent_ref; }

  int num_children() const { return children.size(); }

  const bool is_block() const { return index == -1; }

  const Node_Ptrs& edges_to_type(const int type) const { return edges.at(type); }

  string get_id(const CharacterVector& nodes_id) const {
    if (is_block())
      stop("Cant get id of block node");
    return string(nodes_id.at(index));
  }

  string get_type(const CharacterVector& types_name) const {
    return string(types_name.at(type_index));
  }

  // Comparison operators
  // ===========================================================================
  bool operator==(const Node& b) const { return index == b.index; }
  bool operator<(const Node& b) const { return index < b.index; }
};

#endif
