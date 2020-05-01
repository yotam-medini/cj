// -*- c++ -*-
#if !defined(bIpArtIteE_H)
#define bIpArtIteE_H 1

#include <array>
#include <vector>

typedef std::array<unsigned, 2> au2_t;
typedef std::vector<au2_t> vau2_t;

extern int bipartitee_max_match(vau2_t &match, const vau2_t &edges);

#endif /* bIpArtIteE_H */
