#ifndef __ORDERED_PAIR_INCLUDED__
#define __ORDERED_PAIR_INCLUDED__

template <typename T>
class Ordered_Pair
{
private:
  T val_1;
  T val_2;

public:
  Ordered_Pair(const T a, const T b) : val_1(a < b ? a : b),
                                       val_2(a >= b ? a : b)
  {
  }

  std::pair<T, T> get() const
  {
    return std::make_pair(val_1, val_2);
  }
  T first() const
  {
    return val_1;
  }
  T second() const
  {
    return val_2;
  }
};

template <typename T>
bool operator==(const Ordered_Pair<T> &a, const Ordered_Pair<T> &b)
{
  return (a.first() == b.first()) & (a.second() == b.second());
}

template <typename T>
bool operator<(const Ordered_Pair<T> &a, const Ordered_Pair<T> &b)
{
  // If the first value is same as second, then check the second
  if (a.first() == b.first())
  {
    return a.second() < b.second();
  }
  else
  {
    return a.first() < b.first();
  }
}

// Hash function for ordered pairs so they can be used in hashed containers like unordered_map and unordered_set
template <typename T>
struct Ordered_Pair_Hash {
  size_t operator()(const Ordered_Pair<T> p) const
  {
    return std::hash<T>()(p.first()) ^ std::hash<T>()(p.second());
  }
};


template <typename T>
  using Ordered_Pair_Set = std::unordered_set<Ordered_Pair<T>, Ordered_Pair_Hash<T>>;
// `Bool   ==(Edge_Type a, Edge_Type b)`: Check if `a.type_1_index == b.type_1_index & a.type_2_index == b.type_2_index`
// `size_t hash(Edge_Type e_t)`: Combine hashes for both type indexes. I.e. `return Hash<int>()(e_t.type_1_index)^ Hash<int>()(e_t.type_2_index);`

#endif
