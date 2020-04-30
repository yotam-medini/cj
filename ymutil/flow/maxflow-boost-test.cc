#include "maxflow-boost.h"
#include <iostream>
#include <string>
#include <sstream>

using namespace std;

static bool read_graph(uu_edge_2_u_t &flow, unsigned &src, unsigned &sink)
{
    while (!cin.eof())
    {
        string line;
        getline(cin, line);
        if (!cin.eof())
        {
            char c = line[0];
            char cdum;
            unsigned s, t, x, cap;
            string st;
            istringstream lstream(line);
            switch (c)
            {
              case 'n':
              {
                lstream >> cdum >> x >> st;
                unsigned *pst = (st == string("s") ? &src : &sink);
                *pst = x;
              }
                break;
              case 'a':
              {
                lstream >> cdum >> s >> t >> cap;
                uu_edge_t e{s, t};
                uu_edge_2_u_t::value_type v{e, cap};
                flow.insert(flow.end(), v);
              }
                break;
              default: ;
            }
        }
    }
    return true;
}

int main(int argc, char **argv)
{
    uu_edge_2_u_t flow; unsigned src; unsigned sink;
    bool ok = read_graph(flow, src, sink);
    if (ok)
    {
        unsigned total_flow;
        uu_edge_2_u_t result_flow;
        ok = max_flow(total_flow, result_flow, flow, src, sink);
        if (ok)
        {
            cout << "Total flow: " << total_flow << "\n";
            for (const auto &v: result_flow)
            {
                const uu_edge_t &e = v.first;
                unsigned mxf = v.second;
                cout << "["<<e.first << ", "<< e.second << "]: " << mxf << "\n";
            }
        }
    }
    return ok ? 0 : 1;
}
