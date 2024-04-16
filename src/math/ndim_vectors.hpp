#ifndef NDIM_VEC_HPP
#define NDIM_VEC_HPP

#include <vector>
#include <numeric>
#include <functional>

/*
    Found on https://codeforces.com/blog/entry/76149
*/

template<int D, typename T>
struct NdimVec : public std::vector<NdimVec<D - 1, T>> {
  static_assert(D >= 1, 
  "Vector dimension must be greater than zero!");
  template<typename... Args>
  NdimVec(int n = 0, Args... args) : std::vector<NdimVec<D - 1, T>>
  (n, NdimVec<D - 1, T>(args...)) {
  }
};
template<typename T>
struct NdimVec<1, T> : public std::vector<T> {
  NdimVec(int n = 0, const T& val = T()) : std::vector<T>(n, val) {
  }
};

/*
    Found on 
*/
std::vector<int> create_md_array(const std::vector<int> & dimensions);

#endif //NDIM_VEC_HPP