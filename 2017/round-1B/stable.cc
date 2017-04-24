// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com -- Created: 2013/April/20

#include <iostream>
#include <fstream>
#include <string>
// #include <set>
// #include <map>
#include <deque>
#include <vector>
#include <utility>

#include <cstdlib>
// #include <gmpxx.h>

using namespace std;

// typedef mpz_class mpzc_t;
typedef unsigned long ul_t;
typedef unsigned long long ull_t;
typedef vector<unsigned> vu_t;

static unsigned dbg_flags;

class FBCandidate
{
 public:
    FBCandidate() : 
        n_basic(0), n_comb(0),
        basic{6, 6, 6}, comb{6, 6, 6},
        back{false, false, false, false, false, false}
        {}
    string str() const;
    unsigned n_basic;
    unsigned n_comb;
    unsigned basic[3]; // {0,1,2} 6-padded
    unsigned comb[3]; // {3,4,5} 6-padded
    struct bool back[6];
};

class Stable
{
 public:
    static void init_candidate();
    Stable(istream& fi);
    void solve_naive();
    void solve();
    void print_solution(ostream&) const;
 private:
    // r, y, b, g, v, o
    static const char *CC;
    static FBCandidate candidate[6][6];
    unsigned n;
    unsigned ryb[3];
    unsigned gvo[3];
    unsigned rybgvo[6];
    string solution;
    unsigned curr_ryb[3];
    unsigned curr_gvo[3];
    unsigned curr_rybgvo[6];
    unsigned ryb_max() const
    {
        unsigned ret = 0;
        if (curr_ryb[0] < curr_ryb[1])
        {
            ret = 1;
        }
        if (curr_ryb[ret] < curr_ryb[2])
        {
            ret = 2;
        }
        return ret;
    }
    unsigned gvo_max() const
    {
        unsigned ret = 0;
        if (curr_gvo[0] < curr_gvo[1])
        {
            ret = 1;
        }
        if (curr_gvo[ret] < curr_gvo[2])
        {
            ret = 2;
        }
        return ret;
    }
};

const char *Stable::CC = "RYBGVO";
FBCandidate Stable::candidate[6][6];

void Stable::init_candidate()
{
    static string space(size_t(1), ' ');

    for (unsigned basic = 0; basic < 3; ++basic)
    {
        unsigned comb = basic + 3;
        unsigned basic1 = (basic + 1) % 3;
        unsigned basic2 = (basic + 2) % 3;
        vu_t p;

        p.push_back(basic1);
        p.push_back(basic2);
        p.push_back(comb);
        candidate[basic][basic] = p;

        p.clear();
        // candidate[basic][comb] = p;
        // candidate[comb][basic] = p;

        p.push_back(basic);
        candidate[basic1][basic2] = p;
        candidate[basic2][basic1] = p;
        candidate[comb][comb] = p;

        candidate[basic1][comb] = p;
        candidate[basic2][comb] = p;
        candidate[comb][basic1] = p;
        candidate[comb][basic2] = p;
    }

    for (unsigned x = 0; x < 6; ++x)
    {
        for (unsigned y = 0; y < 6; ++y)
        {
            const vu_t &p = candidate[x][y];
            cerr << "(" << CC[x] <<","<<CC[y] << "): ";
            string s;
            for (unsigned i = 0; i < p.size(); ++i)
            {
                s += string(size_t(1), CC[p[i]]);
            }
            while (s.size() < 6)
            {
                s += space;
            }
            cerr << s;
        }
        cerr << "\n";
    }
}


Stable::Stable(istream& fi) : solution("")
{
    unsigned R, O, Y, G, B, V;
    fi >> n;
    fi >> R >> O >> Y >> G >> B >> V;
    ryb[0] = R;
    ryb[1] = Y;
    ryb[2] = B;
    gvo[0] = G;
    gvo[1] = V;
    gvo[2] = O;
    rybgvo[0] = R;
    rybgvo[1] = Y;
    rybgvo[2] = B;
    rybgvo[3] = G;
    rybgvo[4] = V;
    rybgvo[5] = O;
}

void Stable::solve_naive()
{
    static const string sryb[3] = {"R", "Y", "B"};
    for (unsigned i = 0; i < 3; ++i)
    {
        curr_ryb[i] = ryb[i];
        curr_gvo[i] = gvo[i];
    }
    deque<unsigned> dq;

    unsigned i = ryb_max();
    dq.push_back(i);
    --curr_ryb[i];
    unsigned more = n - 1;
    bool possible = true;
    while (possible && (more > 0))
    {
        if (dq.front() != dq.back())
        {
            i = ryb_max();
            if (more == 2)
            {
                 if ((i != dq.front()) && (i != dq.back()))
                 {
                     unsigned ialt = (i + 1) % 3;
                     if (curr_ryb[ialt] > 0)
                     {
                         i = ialt;
                     }
                     else
                     {
                         ialt = (ialt + 1) % 3;
                         if (curr_ryb[ialt] > 0)
                         {
                             i = ialt;
                         }
                     }
                 }
            }
            if (i == dq.back())
            {
                dq.push_front(i);
            }
            else
            {
                dq.push_back(i);
            }
        }
        else
        {
             unsigned back = dq.back();
             unsigned i0 = (back + 1) % 3;
             unsigned i1 = (i0 + 1) % 3;
             i = (curr_ryb[i0] > curr_ryb[i1] ? i0 : i1);
             dq.push_back(i);            
        }
        possible = (curr_ryb[i] > 0);
        if (possible)
        {
            --curr_ryb[i];
        }
        --more;
    }
    possible = possible && (dq.front() != dq.back());
    if (possible)
    {
        for (auto dqi = dq.begin(), dqe = dq.end(); dqi != dqe; ++dqi)
        {
            i = *dqi;
            solution += sryb[i];
        }
    }    
    else
    {
        solution = "IMPOSSIBLE";
    }
}

void Stable::solve()
{
    vu_t any;
    for (unsigned i = 0; i < 6; ++i)
    {
        curr_rybgvo[i] = rybgvo[i];
        any.push_back(i);
    }

    deque<unsigned> dq;
    unsigned i = rybgvo_max(any);
    dq.push_back(i);
    --curr_rybgvo[i];

    unsigned more = n - 1;
    bool possible = true;
    while (possible && (more > 0))
    {
        int i = rybgvo_max(candidate[dq.front()][dq.back()]);
        possible = (i >= 0);
        if (possible)
        {
             
        }
        --more;
    }    
}

void Stable::print_solution(ostream &fo) const
{
    fo << " " << solution;
}

int main(int argc, char ** argv)
{
    const string dash("-");

    unsigned n_opts = 0;
    bool naive = false;

    if ((argc > 1) && (string(argv[1]) == "-naive"))
    {
        naive = true;
        n_opts = 1;
    }
    int ai_in = n_opts + 1;
    int ai_out = ai_in + 1;
    int ai_dbg = ai_out + 1;
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

    if (ai_dbg < argc) { dbg_flags = strtoul(argv[ai_dbg], 0, 0); }

    unsigned n_cases;
    *pfi >> n_cases;

    void (Stable::*psolve)() =
        (naive ? &Stable::solve_naive : &Stable::solve);

    Stable::init_candidate();
    ostream &fout = *pfo;
    bool tellg = getenv("TELLG");
    ul_t fpos_last = pfi->tellg();
    for (unsigned ci = 0; ci < n_cases; ci++)
    {
        Stable problem(*pfi);
        if (tellg)
        {
            ul_t fpos = pfi->tellg();
            cerr << "Case (ci+1)="<<(ci+1) << ", tellg=[" <<
                fpos_last << ", " << fpos << ")\n";
            fpos_last = fpos;
        }

        (problem.*psolve)();
        fout << "Case #"<< ci+1 << ":";
        problem.print_solution(fout);
        fout << "\n";
        fout.flush();

    }

    if (pfi != &cin) { delete pfi; }
    if (pfo != &cout) { delete pfo; }
    return 0;
}
