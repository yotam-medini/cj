// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com -- Created: 2018/April/29

#include <iostream>
#include <fstream>
#include <string>
// #include <set>
#include <map>
#include <vector>
#include <utility>

#include <cstdlib>

using namespace std;

typedef unsigned u_t;
typedef unsigned long ul_t;
typedef pair<u_t, u_t> uu_t;
typedef vector<u_t> vu_t;
typedef vector<uu_t> vuu_t;
typedef map<vu_t, u_t> vu2u_t;

static unsigned dbg_flags;

class XMut
{
 public:
    XMut(istream& fi);
    void solve_naive();
    void solve();
    void print_solution(ostream&) const;
 private:
    u_t xmut(const vu_t &gm, u_t depth);
    u_t M;
    vuu_t rr;
    vu_t metals;
    u_t solution;
    vu2u_t memo;
};

XMut::XMut(istream& fi) : solution(0)
{
    fi >> M;
    rr.reserve(M);
    for (u_t i = 0; i < M; ++i)
    {
        u_t r1, r2;
        fi >> r1 >> r2;
        rr.push_back(uu_t(r1, r2));
    }
    metals.reserve(M);
    for (u_t i = 0; i < M; ++i)
    {
        u_t g;
        fi >> g;
        metals.push_back(g);
    }
}

void XMut::solve_naive()
{
    solution = xmut(metals, 0);
}

void XMut::solve()
{
    solve_naive();
}

u_t XMut::xmut(const vu_t &gm, u_t depth)
{
    u_t ret = 0;
    auto w = memo.find(gm);
    if (w != memo.end())
    {
        ret = (*w).second;
    }
    else
    {
        ret = gm[0];
        for (u_t rri = 0; rri < rr.size(); ++rri)
        {
            const uu_t &grr = rr[rri];
            u_t r1 = grr.first - 1;
            u_t r2 = grr.second - 1;
            if ((r1 > 0) && (r2 > 0) && (gm[r1] > 0) && (gm[r2] > 0))
            {
                vu_t gmsub(gm);
                --gmsub[r1];
                --gmsub[r2];
                ++gmsub[rri];
                u_t candid = xmut(gmsub, depth + 1);
                if (ret < candid)
                {
                    ret = candid;
                }
            }
        }
        vu2u_t::value_type v(gm, ret);
        memo.insert(memo.end(), v);
    }
    return ret;
}

void XMut::print_solution(ostream &fo) const
{
    fo << " " << solution;
}

int main(int argc, char ** argv)
{
    const string dash("-");

    bool naive = false;
    bool tellg = false;
    int rc = 0, ai;

    for (ai = 1; (rc == 0) && (ai < argc) && (argv[ai][0] == '-') &&
        argv[ai][1] != '\0'; ++ai)
    {
        const string opt(argv[ai]);
        if (opt == string("-naive"))
        {
            naive = true;
        }
        else if (opt == string("-debug"))
        {
            dbg_flags = strtoul(argv[++ai], 0, 0);
        }
        else if (opt == string("-tellg"))
        {
            tellg = true;
        }
        else
        {
            cerr << "Bad option: " << opt << "\n";
            return 1;
        }
    }

    int ai_in = ai;
    int ai_out = ai + 1;
    istream *pfi = (argc <= ai_in || (string(argv[ai_in]) == dash))
         ? &cin
         : new ifstream(argv[ai_in]);
    ostream *pfo = (argc <= ai_out || (string(argv[ai_out]) == dash))
         ? &cout
         : new ofstream(argv[ai_out]);

    if ((!pfi) || (!pfo) || pfi->fail() || pfo->fail())
    {
        cerr << "Open file error\n";
        exit(1);
    }

    string ignore;
    unsigned n_cases;
    *pfi >> n_cases;
    getline(*pfi, ignore);

    void (XMut::*psolve)() =
        (naive ? &XMut::solve_naive : &XMut::solve);
    ostream &fout = *pfo;
    ul_t fpos_last = pfi->tellg();
    for (unsigned ci = 0; ci < n_cases; ci++)
    {
        XMut xmut(*pfi);
        getline(*pfi, ignore);
        if (tellg)
        {
            ul_t fpos = pfi->tellg();
            cerr << "Case (ci+1)="<<(ci+1) << ", tellg=[" <<
                fpos_last << " " << fpos << ") size=" <<
                (fpos - fpos_last) << "\n";
            fpos_last = fpos;
        }

        (xmut.*psolve)();
        fout << "Case #"<< ci+1 << ":";
        xmut.print_solution(fout);
        fout << "\n";
        fout.flush();
    }

    if (pfi != &cin) { delete pfi; }
    if (pfo != &cout) { delete pfo; }
    return 0;
}
