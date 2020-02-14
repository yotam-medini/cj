// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com -- Created: 2018/April/29

#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <vector>
#include <utility>

#include <cstdlib>

using namespace std;

typedef unsigned u_t;
typedef unsigned long ul_t;
typedef pair<u_t, u_t> uu_t;
typedef vector<ul_t> vul_t;
typedef vector<long> vl_t;
typedef vector<uu_t> vuu_t;
typedef map<vul_t, u_t> vul2u_t;

static unsigned dbg_flags;

class XMut
{
 public:
    XMut(istream& fi);
    void solve_naive();
    void solve_naive1();
    void solve();
    void print_solution(ostream&) const;
 private:
    u_t xmut(u_t depth);
    bool xmut_metal(u_t mi, u_t depth=0);
    bool can_xmut(u_t mi, long x, u_t depth=0);
    u_t M;
    vuu_t rr;
    vul_t metals;
    vul_t rmetals;
    vl_t metals_credit;
    ul_t solution;
    vul2u_t memo;
};

XMut::XMut(istream& fi) : solution(0)
{
    fi >> M;
    rr.reserve(M);
    for (u_t i = 0; i < M; ++i)
    {
        ul_t r1, r2;
        fi >> r1 >> r2;
        rr.push_back(uu_t(r1, r2));
    }
    metals.reserve(M);
    for (u_t i = 0; i < M; ++i)
    {
        ul_t g;
        fi >> g;
        metals.push_back(g);
    }
}

void XMut::solve_naive()
{
    rmetals = metals;
    solution = xmut(0);
}

void XMut::solve_naive1()
{
    u_t n_loop = 0;
    rmetals = metals;
    bool xmuting = true;
    while (xmuting)
    {
        ++n_loop;
        vul_t rmetals0(rmetals);
        xmuting = xmut_metal(0, 1);
        if (xmuting)
        {
            ul_t dup = ul_t(-1);
            for (u_t mi = 1; mi < M; ++mi)
            {
                if (rmetals0[mi] < rmetals[mi]) {
                    cerr << "BUG! line: " << __LINE__ << '\n';
                }
                ul_t delta = rmetals0[mi] - rmetals[mi];
                if (delta > 0)
                {
                    ul_t mdup = rmetals[mi] / delta;
                    if (dup > mdup)
                    {
                        dup = mdup;
                    }
                }
            }
            if (dup > 0)
            {
                rmetals[0] += dup;
                for (u_t mi = 1; mi < M; ++mi)
                {
                    ul_t delta = rmetals0[mi] - rmetals[mi];
                    rmetals[mi] -= dup * delta;
                }
            }
        }
    }
    if (dbg_flags & 0x1) { cerr << "n_loop="<<n_loop << '\n'; }
    solution = rmetals[0];
}

u_t XMut::xmut(u_t depth)
{
    u_t ret = 0;
    auto w = memo.find(rmetals);
    if (w != memo.end())
    {
        ret = (*w).second;
    }
    else
    {
        ret = rmetals[0];
        for (u_t rri = 0; rri < rr.size(); ++rri)
        {
            const uu_t &grr = rr[rri];
            u_t r1 = grr.first - 1;
            u_t r2 = grr.second - 1;
            if ((r1 > 0) && (r2 > 0) && (rmetals[r1] > 0) && (rmetals[r2] > 0))
            {
                --rmetals[r1];
                --rmetals[r2];
                ++rmetals[rri];
                u_t candid = xmut(depth + 1);
                if (ret < candid)
                {
                    ret = candid;
                }
                ++rmetals[r1];
                ++rmetals[r2];
                --rmetals[rri];
            }
        }
        vul2u_t::value_type v(rmetals, ret);
        memo.insert(memo.end(), v);
    }
    return ret;
}

bool XMut::xmut_metal(u_t mi, u_t depth)
{
    bool xmuted = depth < M;
    const uu_t& rrmi = rr[mi];
    for (u_t rri = 0; xmuted && (rri != 2); ++rri)
    {
        u_t mri = (rri == 0 ? rrmi.first : rrmi.second) - 1;
        xmuted = (mri != 0); // we don't lose lead !
        if (xmuted)
        {
            if (rmetals[mri] == 0)
            {
                xmuted = xmut_metal(mri, depth + 1);
            }
            if (rmetals[mri] > 0)
            {
                --rmetals[mri];
            }
        }
    }
    if (xmuted)
    {
        if ((mi > 0) && (rmetals[mi] > 0)) {
            cerr << "BUG! line: " << __LINE__ << '\n';
        }
        ++rmetals[mi];
    }
    return xmuted;
}

void XMut::solve() // using published analysis
{
    long can_low = metals[0], cannot_high = 0;
    vl_t metals_credit0;
    metals_credit0.reserve(M);
    for (u_t i = 0; i < M; ++i)
    {
        metals_credit0.push_back(metals[i]);
        cannot_high += metals[i];
    }
    while (can_low + 1 < cannot_high)
    {
        metals_credit = metals_credit0;
        long mid = (can_low+ + cannot_high)/2;
        if (can_xmut(0, mid))
        {
            can_low = mid;
        }
        else
        {
            cannot_high = mid;
        }
    }
    solution = can_low;
}

bool XMut::can_xmut(u_t mi, long x, u_t depth)
{
    bool can = (depth < M);
    if (can)
    {
        metals_credit[mi] -= x;
        can = (metals_credit[mi] >= 0);
        if (!can)
        {
            long deficit = - metals_credit[mi];
            const uu_t& rrmi = rr[mi];
            can = true;
            for (u_t rri = 0; can && (rri != 2); ++rri)
            {
                u_t mri = (rri == 0 ? rrmi.first : rrmi.second) - 1;
                can = can_xmut(mri, deficit, depth + 1);
            }
            if (can)
            {
                metals_credit[mi] = 0;
            }
        }
    }
    return can;
}

void XMut::print_solution(ostream &fo) const
{
    fo << " " << solution;
}

int main(int argc, char ** argv)
{
    const string dash("-");

    bool naive = false;
    bool naive1 = false;
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
        else if (opt == string("-naive1"))
        {
            naive1 = true;
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
        (naive ? &XMut::solve_naive :
        (naive1 ? &XMut::solve_naive1 : &XMut::solve));
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
