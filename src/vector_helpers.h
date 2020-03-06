#ifndef __VECTOR_HELPERS_INCLUDED__
#define __VECTOR_HELPERS_INCLUDED__

#include <vector>
#include <utility>

template <typename T>
using U_Ptr = std::unique_ptr<T>;

template <typename T>
using U_Ptr_Vec = std::vector<U_Ptr<T>>;


template <typename T>
void delete_from_vector(std::vector<T> & vec, const T to_remove){

  // Get iterator to the element we're deleting
  auto it = std::find(vec.begin(), vec.end(), to_remove);

  // Swap element to delete and last element in vector
  std::swap(*it, vec.back());

  // Remove/delete last element of vector
  vec.pop_back();
}



template <typename T>
void delete_from_vector(U_Ptr_Vec<T> & vec, const T * el){

  // Get iterator to the element we're deleting
  auto it = std::find_if(vec.begin(), vec.end(), [&el](const U_Ptr<T>& ptr){return ptr.get() == el;});

  // Swap element to delete and last element in vector
  std::swap(*it, vec.back());

  // Remove/delete last element of vector
  vec.pop_back();
}


// Function takes a vector of smart pointers and returns a vector of raw pointers that's optionally been shuffled
template <typename T>
std::vector<T*> shadow_copy_smart_ptr_vec(const U_Ptr_Vec<T> & vec){
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
