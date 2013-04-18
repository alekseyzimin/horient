#ifndef _DISJOINT_SET_H_
#define _DISJOINT_SET_H_

#include <algorithm>

template<typename T>
struct disjoint_set {
  typedef T                value_type;
  typedef T&               reference;
  typedef const T&         const_reference;
  typedef T*               pointer;
  typedef const T*         const_pointer;
  typedef disjoint_set<T>* set_type;

  set_type     parent;
  unsigned int rank;
  T            value;

  disjoint_set() : parent(this), rank(0) { }
  explicit disjoint_set(const T& v) : parent(this), rank(0), value(v) { }
  explicit disjoint_set(T&& v) : parent(this), rank(0), value(std::move(v)) { }

  set_type find() {
    if(parent != this)
      parent = parent->find();
    return parent;
  }
};

template<typename T>
void union_set(disjoint_set<T>& x, disjoint_set<T>& y) {
  typedef typename disjoint_set<T>::set_type set_type;
  set_type xRoot = x.find();
  set_type yRoot = y.find();

  if(xRoot == yRoot)
    return;
  if(xRoot->rank < yRoot->rank) {
    xRoot->parent  = yRoot;
  } else {
    yRoot->parent  = xRoot;
    xRoot->rank   += (xRoot->rank == yRoot->rank);
  }
}

template<typename T>
typename disjoint_set<T>::set_type find_root(disjoint_set<T>& x) { return x.find(); }

#endif /* _DISJOINT_SET_H_ */
