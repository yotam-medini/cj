// Author:  Yotam Medini  yotam.medini@gmail.com -- Created: 2018/March/22
#if !defined(_maxflow_H)
#define _maxflow_H 1

#include <map>
#include <utility>

typedef std::pair<unsigned, unsigned> uu_edge_t;
typedef std::map<uu_edge_t, unsigned> uu_edge_2_u_t;

// Assumuing edges indices are small as possible
extern bool max_flow(
    unsigned &total_flow, 
    uu_edge_2_u_t &result_flow,
    const uu_edge_2_u_t &flow, 
    unsigned source,
    unsigned sink);

#endif /* _maxflow_H */

