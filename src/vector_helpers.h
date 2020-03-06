#ifndef __VECTOR_HELPERS_INCLUDED__
#define __VECTOR_HELPERS_INCLUDED__

#include <vector>
#include <utility>

template <typename T>
void delete_from_vector(std::vector<T> & vec, const T & to_remove){

  // Get iterator to the element we're deleting
  auto it = std::find(vec.begin(), vec.end(), to_remove);

  // Swap element to delete and last element in vector
  std::swap(*it, vec.back());

  // Remove/delete last element of vector
  vec.pop_back();
}



#endif
