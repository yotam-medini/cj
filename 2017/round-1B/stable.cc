// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com -- Created: 2013/April/20

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <algorithm>
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
        back{false, false, false, false, false, false},
        last{false, false, false, false, false, false}
        {}
    string str() const;
    const unsigned *pbasic() const { return &basic[0]; }
    const unsigned *pcomb() const { return &comb[0]; }
    bool has(unsigned i) const
    {
        bool ret = (i < 3
            ? find(pbasic(), pbasic() + n_basic, i) != pbasic() + n_basic
            : find(pcomb(), pcomb() + n_comb, i) != pcomb() + n_comb);
        return ret;
    }
    unsigned n_basic;
    unsigned n_comb;
    unsigned basic[3]; // {0,1,2} 6-padded
    unsigned comb[3]; // {3,4,5} 6-padded
    bool back[6];
    bool last[6];
};

string FBCandidate::str() const
{
    ostringstream oss;
    oss << "{B=";
    for (unsigned i = 0; i < 3; ++i)
    {
        oss << (i < n_basic ? "RYB"[basic[i]] : ' ');
    }
    oss << " C=";
    for (unsigned i = 0; i < 3; ++i)
    {
        oss << (i < n_comb ? "GVO"[comb[i] - 3] : ' ');
    }
    oss << " >=";
    for (unsigned i = 0; i < 6; ++i)
    {
        oss << "fT"[int(back[i])];
    }
    oss << " L=";
    for (unsigned i = 0; i < 6; ++i)
    {
        oss << "fT"[int(last[i])];
    }
    oss << "}";
    return oss.str();
}

class Stable
{
 public:
    static void init_candidate();
    Stable(istream& fi);
    void solve_naive();
    void solve();
    void print_solution(ostream&) const;
 private:
    bool last2(deque<unsigned> &dq);
    void solve_finish(bool possible, const deque<unsigned> &dq);
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

    for (unsigned f = 0; f < 3; ++f)
    {
        for (unsigned b = 0; b < 3; ++b)
        {
            FBCandidate &fbc = candidate[f][b];
            if (f == b)
            {
                fbc.n_basic = 2;
                fbc.basic[0] = b == 0 ? 1 : 0;
                fbc.basic[1] = 3 - (b + fbc.basic[0]);
                fbc.n_comb = 1;
                fbc.comb[0] = b + 3;
                fill_n(&fbc.back[0], 6, true);
                fbc.last[(b + 1) % 3] = true;
                fbc.last[(b + 2) % 3] = true;
                fbc.last[b + 3] = true;
            }
            else
            {
                fbc.n_basic = 3;
                for (unsigned k = 0; k < 3; ++k)
                {
                    fbc.basic[k] = k;
                    fbc.back[k] = (k != b);
                }
                fbc.n_comb = 2;
                unsigned fi = f < b ? 0 : 1;
                unsigned bi = 1 - fi;
                fbc.comb[fi] = f + 3;
                fbc.comb[bi] = b + 3;
                fbc.back[f + 3] = false;
                fbc.back[b + 3] = true;
                fbc.last[3 - (f + b)] = true;
            }
        }
    }

    for (unsigned basic = 0; basic < 3; ++basic)
    {
        for (unsigned comb = 3; comb < 6; ++comb)
        {
            FBCandidate &fbc = candidate[basic][comb];
            if (basic + 3 == comb)
            {
                fbc.n_basic = 3;
                for (unsigned k = 0; k < 3; ++k)
                {
                    fbc.basic[k] = k;
                    fbc.back[k] = (k == basic);
                }
                fbc.n_comb = 1;
                fbc.comb[0] = comb;
                fbc.back[comb] = true;
                // fbc.last = 6*false
            }
            else
            {
                fbc.n_basic = 2;
                fbc.basic[0] = basic == 0 ? 1 : 0;
                fbc.basic[1] = 3 - (basic + fbc.basic[0]);
                fbc.back[comb - 3] = true;
                fbc.n_comb = 1;
                fbc.comb[0] = basic + 3;
                fbc.last[comb - 3] = true;
            }

            FBCandidate &mirror = candidate[comb][basic];
            mirror = fbc;
            for (unsigned k = 0; k < 6; ++k)
            {
                mirror.back[k] = !mirror.back[k];
            }
        }
    }

    for (unsigned fb = 3; fb < 6; ++fb) // if 3<=f<b<6 then nothing!
    {
        FBCandidate &fbc = candidate[fb][fb];
        fbc.n_basic = 1;
        fbc.basic[0] = fb - 3;
        fbc.n_comb = 0;
        fbc.comb[fb - 3] = true;
        fbc.last[fb - 3] = true;
    }

    for (unsigned f = 0; f < 6; ++f)
    {
        for (unsigned b = 0; b < 6; ++b)
        {
            static const char *NM = "RYBGVO";
            const FBCandidate &fbc = candidate[f][b];
            cerr << "  ("<< NM[f] << "," << NM[b] <<"): " << fbc.str();
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

void Stable::solve_finish(bool possible, const deque<unsigned> &dq)
{
    if (possible)
    {
        for (auto dqi = dq.begin(), dqe = dq.end(); dqi != dqe; ++dqi)
        {
            unsigned i = *dqi;
            solution += "RYBGVO"[i];
        }
    }    
    else
    {
        solution = "IMPOSSIBLE";
    }
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
    solve_finish(possible, dq);
}

bool Stable::last2(deque<unsigned> &dq)
{
    bool done = false;
    unsigned i;
    for (i = 0; curr_rybgvo[i] == 0; ++i) {}
    unsigned next1 = i;
    --curr_rybgvo[i];
    for (i = 0; curr_rybgvo[i] == 0; ++i) {}
    unsigned next2 = i;
    --curr_rybgvo[i];
    const FBCandidate &fbc = candidate[dq.front()][dq.back()];
    for (unsigned try2 = 0; (try2 < 2) && !done; ++try2)
    {
        if (fbc.has(next1))
        {
             if (fbc.back[next1])
             {
                  if (candidate[dq.front()][next1].last[next2])
                  {
                       dq.push_back(next1);
                       dq.push_back(next2);
                       done = true;
                  }
                  else if (candidate[next1][dq.back()].last[next2])
                  {
                       dq.push_back(next2);
                       dq.push_back(next1);
                       done = true;
                  }
             }
        }
        swap(next1, next2);
    }
    return done;
}


void Stable::solve()
{
    for (unsigned i = 0; i < 6; ++i)
    {
        curr_rybgvo[i] = rybgvo[i];
    }

    deque<unsigned> dq;
    unsigned i;
    for (i = 0; curr_rybgvo[i] == 0; ++i) {}
    dq.push_back(i);
    --curr_rybgvo[i];

    unsigned more = n - 1;
    bool possible = true;
    while (possible && (more > 0))
    {
        if (more == 2)
        {
            possible = last2(dq);
            more = 0;
        }
        else
        {
            const FBCandidate &fbc = candidate[dq.front()][dq.back()];
            unsigned cmax = 0;
            for (unsigned jj = 0; jj != fbc.n_comb; ++jj)
            {
                unsigned j = fbc.comb[jj];
                if (cmax < curr_rybgvo[j])
                {
                    cmax  = curr_rybgvo[j];
                    i = j;
                }
            }
            for (unsigned jj = (cmax == 0 ? 0 : fbc.n_basic); 
                jj != fbc.n_basic; ++jj)
            {
                unsigned j = fbc.basic[jj];
                if (cmax < curr_rybgvo[j])
                {
                    cmax  = curr_rybgvo[j];
                    i = j;
                }
            }
            possible = (cmax > 0);
            if (possible)
            {
                if (fbc.back[i])
                {
                    dq.push_back(i);
                }
                else
                {
                    dq.push_front(i);
                }
                --curr_rybgvo[i];
                --more;
            }
        }
    }    
    solve_finish(possible, dq);
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
