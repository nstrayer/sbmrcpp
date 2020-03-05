#ifndef __NODE_INCLUDED__
#define __NODE_INCLUDED__

// [[Rcpp::plugins(cpp11)]]
#include <Rcpp.h>

#include <vector>

using Int_Vec = std::vector<int>;
using string = std::string;

class Node
{
public:
  // Data
  int index;             // Index of this node in `nodes_*` vectors
  int type_index;        // Index of node type in `types_*` vectors
  Int_Vec edge_indices;  // Vector of integers corresponding to the `index` of every connected node
  int parent_index = -1; // Index of block or parent node in next-level's `Node_Container`

  // Setters
  // ===========================================================================

  // Initialize the `index` and `type_index` data members
  Node(int i, int t) : index(i), type_index(t) {}

  // Append to `edge_indices` vector a new edge integer
  void add_edge(int e_i)
  {
    edge_indices.push_back(e_i);
  }

  // Set the value of `parent_index` to a given integer
  void add_parent(int p_i)
  {
    parent_index = p_i;
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
    return edge_indices.size();
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
