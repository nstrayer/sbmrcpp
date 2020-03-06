#ifndef __VECTOR_HELPERS_INCLUDED__
#define __VECTOR_HELPERS_INCLUDED__

#include <utility>
#include <vector>

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

#endif
