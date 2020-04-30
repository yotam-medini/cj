// -*- c++ -*-
#if !defined(bIpArtIteE_H)
#define bIpArtIteE_H 1

#include <set>
#include <array>

typedef std::array<unsigned, 2> au2_t;
typedef std::set<au2_t> lr_edges_t;

extern int bipartitee_max_match(lr_edges_t &match, const lr_edges_t &edges);

#endif /* bIpArtIteE_H */
