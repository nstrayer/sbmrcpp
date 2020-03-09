#ifndef __VECTOR_HELPERS_INCLUDED__
#define __VECTOR_HELPERS_INCLUDED__

#include <utility>
#include <vector>

template <typename T>
using Vec_of_Vecs = std::vector<std::vector<T>>;

template <typename T>
using U_Ptr = std::unique_ptr<T>;

template <typename T>
using U_Ptr_Vec = std::vector<U_Ptr<T>>;

template <typename T>
bool delete_from_vector(std::vector<T>& vec, const T to_remove) {
  // Get iterator to the element we're deleting
  auto it = std::find(vec.begin(), vec.end(), to_remove);

  // If requested element couldn't be found, return false
  if (it == vec.end())
    return false;

  // Swap element to delete and last element in vector
  std::swap(*it, vec.back());

  // Remove/delete last element of vector
  vec.pop_back();

  return true;
}

template <typename T>
bool delete_from_vector(U_Ptr_Vec<T>& vec, const T* el) {
  // Lambda function to compare smart pointers and normal pointer
  auto find_el = [&el](const U_Ptr<T>& ptr) { return ptr.get() == el; };

  // Get iterator to the element we're deleting
  auto it = std::find_if(vec.begin(), vec.end(), find_el);

  // If requested element couldn't be found, return false
  if (it == vec.end())
    return false;

  // Swap element to delete and last element in vector
  std::swap(*it, vec.back());

  // Remove/delete last element of vector
  vec.pop_back();

  return true;
}

// Total number of elements in a vector of vectors
template <typename T>
int total_num_elements(const Vec_of_Vecs<T>& vec_of_vecs) {
  int total = 0;
  for (const auto& sub_vec : vec_of_vecs) {
    total += sub_vec.size();
  }
  return total;
}

#endif
