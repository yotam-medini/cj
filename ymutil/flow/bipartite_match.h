// -*- c++ -*-
#if !defined(bIpArtIteE_H)
#define bIpArtIteE_H 1

#include <set>
#include <utility>

typedef std::pair<unsigned, unsigned> uu_t;
typedef std::set<uu_t> lr_edges_t;

extern int bipartitee_max_match(lr_edges_t &match, const lr_edges_t &edges);

#endif /* bIpArtIteE_H */
