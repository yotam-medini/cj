// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com --

#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>
#include <array>
#include <vector>
#include <utility>

#include <cstdlib>

using namespace std;

typedef unsigned u_t;
typedef unsigned long ul_t;
typedef unsigned long long ull_t;
typedef array<ull_t, 2> aull2_t;
typedef vector<aull2_t> vaull2_t;
typedef vector<ull_t> vull_t;
typedef pair<ull_t, bool> ullb_t;

class Seq
{
 public:
    Seq(ull_t _idx=0, ull_t _start=0, u_t _mult=0) :
        idx(_idx), start(_start), mult(_mult) {}
    ull_t idx;
    ull_t start;
    u_t mult;
};
typedef vector<Seq> vseq_t;

static unsigned dbg_flags;

class CombCls
{
 public:
    CombCls(istream& fi);
    void solve_naive();
    void solve();
    void print_solution(ostream&) const;
 private:
    void compute_lrq();
    void gen_seqs();
    ull_t get_score(ull_t idx) const;
    u_t N, Q;
    ull_t X1, X2, A1, B1, C1, M1;
    ull_t Y1, Y2, A2, B2, C2, M2;
    ull_t Z1, Z2, A3, B3, C3, M3;
    vaull2_t lr;
    vull_t q;
    vseq_t seqs;
    ull_t solution;
};

CombCls::CombCls(istream& fi) : solution(0)
{
    fi >> N >> Q;
    fi >> X1 >>X2 >>A1 >>B1 >>C1 >> M1;
    fi >> Y1 >>Y2 >>A2 >>B2 >>C2 >> M2;
    fi >> Z1 >>Z2 >>A3 >>B3 >>C3 >> M3;    
}

void CombCls::solve_naive()
{
    compute_lrq();
    vull_t all;
    for (ull_t c = 0; c < N; ++c)
    {
        const aull2_t lrc = lr[c];
        for (ull_t s = 0, ns = lrc[1] - lrc[0] + 1; s < ns; ++s)
        {
            all.push_back(lrc[0] + s);
        } 
    }
    sort(all.begin(), all.end());
    ull_t na = all.size();
    for (ull_t i = 0; i < Q; ++i)
    {
        ull_t k = q[i] + 1;
        ull_t score = (k <= na ? all[na - k] : 0);
        solution += (i + 1)*score;
    }
}

void CombCls::solve()
{
    // solve_naive();
    compute_lrq();
    gen_seqs();
    ull_t na = seqs.back().idx;
    for (ull_t i = 0; i < Q; ++i)
    {
        ull_t k = q[i] + 1;
        ull_t score = (k <= na ? get_score(na - k) : 0);
        solution += (i + 1)*score;
    }
}

void CombCls::compute_lrq()
{
    lr.reserve(N);
    lr.push_back(aull2_t{min(X1, Y1) + 1, max(X1, Y1) + 1});
    if (N > 1)
    {
        lr.push_back(aull2_t{min(X2, Y2) + 1, max(X2, Y2) + 1});
    }
    ull_t x1 = X1, x2 = X2, y1 = Y1, y2 = Y2;
    for (u_t i = 2; i < N; ++i)
    {
        ull_t xi = (A1*x2 + B1*x1 + C1) % M1;
        ull_t yi = (A2*y2 + B2*y1 + C2) % M2;
        lr.push_back(aull2_t{min(xi, yi) + 1, max(xi, yi) + 1});
        x1 = x2;
        x2 = xi;
        y1 = y2;
        y2 = yi;
    }
    q.reserve(Q);
    q.push_back(Z1);
    if (Q > 1)
    {
        q.push_back(Z2);
    }
    ull_t z1 = Z1, z2 = Z2;
    for (u_t i = 2; i < Q; ++i)
    {
        ull_t zi = (A3*z2 + B3*z1 + C3) % M3;
        q.push_back(zi);
        z1 = z2;
        z2 = zi;
    }
}

void CombCls::gen_seqs()
{
    vector<ullb_t> class_on;
    for (const aull2_t& lrc: lr)
    {
        class_on.push_back(ullb_t{lrc[0], true});
        class_on.push_back(ullb_t{lrc[1] + 1, false});
    }
    sort(class_on.begin(), class_on.end());
    u_t mult = 0;
    for (u_t coi = 0, coe = class_on.size(); coi != coe; )
    {
        ull_t start = class_on[coi].first;
        for ( ; (coi != coe) && class_on[coi].first == start; ++coi)
        {
            if (class_on[coi].second)
            {
                ++mult;
            }
            else
            {
                --mult;
            }
        }
        ull_t idx = 0;
        if (!seqs.empty())
        {
            const Seq& pre = seqs.back();
            idx = pre.idx + pre.mult * (start - pre.start);
        }
        Seq seq{idx, start, mult};
        if (seqs.empty() || (seqs.back().mult > 0))
        {
            seqs.push_back(seq);
        }
        else
        {
            seqs.back() = seq;
        }
    }
}

ull_t CombCls::get_score(ull_t idx) const
{
    ull_t score = 0;
    vseq_t::const_iterator lb = lower_bound(seqs.begin(), seqs.end(), idx, 
        [this](const Seq& seq, ull_t iidx) -> bool
        {
            bool lt = seq.idx < iidx;
            if (dbg_flags & 0x1) {
               cerr << "iidx="<<iidx << ", seq.idx="<<seq.idx << 
                 ", lt="<<lt<<'\n'; }
            return lt;
        });
    if (lb->idx > idx)
    {
        --lb;
    }
    for (; lb->mult == 0; --lb) {}
    u_t lbi = lb - seqs.begin();
    if (lbi + 1 < seqs.size())
    {
        const Seq& seq = seqs[lbi];
        score = seq.start + ((idx - seq.idx) / seq.mult);
    }
    return score;
}

void CombCls::print_solution(ostream &fo) const
{
    fo << ' ' << solution;
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

    void (CombCls::*psolve)() =
        (naive ? &CombCls::solve_naive : &CombCls::solve);
    ostream &fout = *pfo;
    ul_t fpos_last = pfi->tellg();
    for (unsigned ci = 0; ci < n_cases; ci++)
    {
        CombCls combcls(*pfi);
        getline(*pfi, ignore);
        if (tellg)
        {
            ul_t fpos = pfi->tellg();
            cerr << "Case (ci+1)="<<(ci+1) << ", tellg=[" <<
                fpos_last << " " << fpos << ") size=" <<
                (fpos - fpos_last) << "\n";
            fpos_last = fpos;
        }

        (combcls.*psolve)();
        fout << "Case #"<< ci+1 << ":";
        combcls.print_solution(fout);
        fout << "\n";
        fout.flush();
    }

    if (pfi != &cin) { delete pfi; }
    if (pfo != &cout) { delete pfo; }
    return 0;
}
