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



class Stable
{
 public:
    void init_possible();
    Stable(istream& fi);
    void solve_naive();
    void solve();
    void print_solution(ostream&) const;
 private:
    static vu_t possible[6][6];
    unsigned n;
    unsigned ryb[3];
    unsigned gvo[3];
    string solution;
    unsigned curr_ryb[3];
    unsigned curr_gvo[3];
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
    if (gvo[0] == 0 && gvo[1] == 0 && gvo[2] == 0)
    {
        solve_naive();
    }
    else
    {
        cerr << "GVO nonzero - fail\n";
        exit(1);
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
