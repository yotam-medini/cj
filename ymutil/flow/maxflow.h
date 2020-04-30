// Author:  Yotam Medini  yotam.medini@gmail.com -- Created: 2020/April/29
#if !defined(_maxflow_H2)
#define _maxflow_H2 1

#include <map>
#include <array>

typedef unsigned u_t;
typedef std::array<u_t, 2> au2_t;
typedef std::map<au2_t, u_t> au2_2u_t;

// Assumuing edges indices are small as possible
extern bool max_flow(
    u_t& total_flow, 
    au2_2u_t& result_flow,
    const au2_2u_t& flow, 
    u_t source,
    u_t sink);

#endif /* _maxflow_H2 */

