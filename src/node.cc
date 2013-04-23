#include <node.hpp>

node* merge_nodes(node& n1, node& n2) {
  assert(n1.parent == 0);
  assert(n2.parent == 0);

  node *p = &n1, *c = &n2;
  if(n1.rank < n2.rank)
    std::swap(p, c);
  p->rank   += (p->rank == c->rank);
  c->parent  = p;
  c->orient  = (p->orient == c->orient) ? 1 : -1;
  p->size   += c->size;
  p->merge(*c);
  return c;
}
