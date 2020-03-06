#ifndef __VECTOR_HELPERS_INCLUDED__
#define __VECTOR_HELPERS_INCLUDED__

#include <vector>
#include <utility>
#include "Node.h"

using Node_Ptr = std::unique_ptr<Node>;
using Node_Ptr_Vec = std::vector<Node_Ptr>;

template <typename T>
void delete_from_vector(std::vector<T> & vec, const T & to_remove){

  // Get iterator to the element we're deleting
  auto it = std::find(vec.begin(), vec.end(), to_remove);

  // Swap element to delete and last element in vector
  std::swap(*it, vec.back());

  // Remove/delete last element of vector
  vec.pop_back();
}


void delete_node(Node_Ptr_Vec & node_vec, Node * to_remove){

  auto find_to_remove = [&to_remove](const Node_Ptr& node_unique_ptr){
    return node_unique_ptr.get() == to_remove;
  };

  // Get iterator to the Node we're deleting
  auto it = std::find_if(node_vec.begin(), node_vec.end(), find_to_remove);

  // Swap element to delete and last element in vector
  std::swap(*it, node_vec.back());

  // Remove/delete last element of vector
  node_vec.pop_back();
}


// Function takes a vector of smart pointers and returns a vector of raw pointers that's optionally been shuffled
template <typename T>
std::vector<T*> shadow_copy_smart_ptr_vec(const std::vector<std::unique_ptr<T>> & vec){
  // Instantiate vector of raw pointers for copy
  std::vector<T*> copy_vec;
  copy_vec.reserve(vec.size());

  // Fill in with raw pointers from main vec
  for (const auto& el : vec) {
    copy_vec.push_back(el.get());
  }

  return copy_vec;
}



#endif