// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com -- Created: 2013/April/20

#include <iostream>
#include <fstream>
#include <string>
#include <utility>

#include <cstdlib>
#include <cstring>

using namespace std;

typedef unsigned long ul_t;
typedef unsigned long long ull_t;

static unsigned dbg_flags;

static string c1tos(char c)
{
    string ret(&c, 1);
    return ret;
}

static string s1d[10];
static void s1d_init()
{
    for (unsigned n = 0; n <= 9; ++n)
    {
        s1d[n] = c1tos('0' + n);
    }
}

class CloseMatch
{
 public:
    CloseMatch(istream& fi);
    void solve();
    void print_solution(ostream&) const;
 private:
    void solve_real();
    void solve_naive3();
    void sub_solve(const string& chead, const string &jhead, unsigned hsize);
    string C, J;
    const char *csC, *csJ;
    string solutionC, solutionJ;
    unsigned size;
    ull_t best_diff;
};

CloseMatch::CloseMatch(istream& fi) : best_diff(~(ull_t(0)))
{
    fi >> C >> J;
}


void CloseMatch::solve()
{
    if (dbg_flags & 0x1) { solve_naive3(); } else { solve_real(); }
}

void CloseMatch::solve_real()
{
    csC = C.c_str();
    csJ = J.c_str();
    size = strlen(csC);
    sub_solve("", "", 0);
}

void
CloseMatch:: sub_solve(const string& chead, const string &jhead, unsigned hsize)
{
    if (hsize == size)
    {
        ull_t cn = strtoull(chead.c_str(), 0, 10);
        ull_t jn = strtoull(jhead.c_str(), 0, 10);
        ull_t d = (cn < jn ? jn - cn : cn - jn);
        if (best_diff > d)
        {
            best_diff = d;
            solutionC = chead;
            solutionJ = jhead;
        }
    }
    else
    {
        char cc = csC[hsize];
        char cj = csJ[hsize];
        unsigned hsz1 = hsize + 1;
        if (cc == '?' || cj == '?')
        {
            if (chead == jhead)
            {
                unsigned ql = 0;
                for (; csC[hsize + ql] == '?' && csJ[hsize + ql] == '?'; ++ql);
                if (ql > 0)
                {
                    string zql(size_t(ql - 1), '0');
                    string chz = chead + zql;                    
                    string jhz = jhead + zql;                    
                    for (unsigned c01 = 0; c01 < 2; ++c01)
                    {
                        for (unsigned j01 = 0; j01 < 2; ++j01)
                        {
                            sub_solve(chz + s1d[c01], jhz + s1d[j01],
                                hsize + ql);
                        }
                    }
                }
                else
                {
                    char cnext = (cc == '?' ? cj : cc);
                    int d = cnext - '0';
                    for (int si = max(0, d - 1); si <= min(9, d + 1); ++si)
                    {
                        if (cc == '?')
                        {
                            sub_solve(chead + s1d[si], jhead + s1d[d], hsz1);
                        }
                        else
                        {
                            sub_solve(chead + s1d[d], jhead + s1d[si], hsz1);
                        }
                    }
                }
            }
            if (chead < jhead)
            {
                if (cc == '?' && cj == '?')
                {
                    sub_solve(chead + s1d[9], jhead + s1d[0], hsz1);
                }
                else if (cc == '?')
                {
                    sub_solve(chead + s1d[9], jhead + s1d[cj - '0'], hsz1);
                }
                else // cj = '?'
                {
                    sub_solve(chead + s1d[cc - '0'], jhead + s1d[0], hsz1);
                }
            }
            else // chead > jhead
            {
                if (cc == '?' && cj == '?')
                {
                    sub_solve(chead + s1d[0], jhead + s1d[9], hsz1);
                }
                else if (cc == '?')
                {
                    sub_solve(chead + s1d[0], jhead + s1d[cj - '0'], hsz1);
                }
                else // cj = '?'
                {
                    sub_solve(chead + s1d[cc - '0'], jhead + s1d[9], hsz1);
                }
            }

        }
        else
        {
            sub_solve(chead + s1d[cc - '0'], jhead + s1d[cj - '0'], hsz1);
        }
    }
}

static void qmbe(char &b, char &e, char c)
{
    b = e = c;
    if (c == '?')
    {
        b = '0'; e = '9';
    }
}

void CloseMatch::solve_naive3()
{
    const char *csC = C.c_str();
    const char *csJ = J.c_str();
    unsigned sz = strlen(csC);
    char cb[3], ce[3], jb[3], je[3];
    char xc[4];
    char xj[4];

    fill_n(xc, 4, '\0');
    fill_n(xj, 4, '\0');
    strcpy(xc, csC);
    strcpy(xj, csJ);

    for (unsigned k = 0; k < sz; ++k)
    {
        qmbe(cb[k], ce[k], csC[k]);
        qmbe(jb[k], je[k], csJ[k]);
    }

    for (unsigned k = sz; k < 3; ++k)
    {
        cb[k] = ce[k] = '\0';
        jb[k] = je[k] = '\0';
    }

    int min_dist = 10000;
    for (char c0 = cb[0]; c0 <= ce[0]; ++c0)
    {
        xc[0] = c0;
        for (char j0 = jb[0]; j0 <= je[0]; ++j0)
        {
            xj[0] = j0;

            for (char c1 = cb[1]; c1 <= ce[1]; ++c1)
            {
                xc[1] = c1;
                for (char j1 = jb[1]; j1 <= je[1]; ++j1)
                {
                    xj[1] = j1;

                    for (char c2 = cb[2]; c2 <= ce[2]; ++c2)
                    {
                        xc[2] = c2;
                        for (char j2 = jb[2]; j2 <= je[2]; ++j2)
                        {
                            xj[2] = j2;

                            int nc = strtoul(xc, 0, 10);
                            int nj = strtoul(xj, 0, 10);
                            int d = abs(nc - nj);
                            if (min_dist > d)
                            {
                                min_dist = d;
                                solutionC = string(xc);
                                solutionJ = string(xj);
                            }
                        }
                    }
                }
            }

        }
    }
}

void CloseMatch::print_solution(ostream &fo) const
{
    fo << " " << solutionC << " " << solutionJ;
}

int main(int argc, char ** argv)
{
    s1d_init();
    const string dash("-");

    istream *pfi = (argc < 2 || (string(argv[1]) == dash))
         ? &cin
         : new ifstream(argv[1]);
    ostream *pfo = (argc < 3 || (string(argv[2]) == dash))
         ? &cout
         : new ofstream(argv[2]);

    if ((!pfi) || (!pfo) || pfi->fail() || pfo->fail())
    {
        cerr << "Open file error\n";
        exit(1);
    }

    if (argc > 3) { dbg_flags = strtoul(argv[3], 0, 0); }
    // bool large = (argc > 4) && !strcmp(argv[4], "-large");

    unsigned n_cases;
    *pfi >> n_cases;

    ostream &fout = *pfo;
    for (unsigned ci = 0; ci < n_cases; ci++)
    {
        CloseMatch problem(*pfi);
        // cerr << "Case ci="<<ci << " (ci+1)="<<ci+1 << "\n";
        problem.solve();
        fout << "Case #"<< ci+1 << ":";
        problem.print_solution(fout);
        fout << "\n";
        fout.flush();

    }

    if (pfi != &cin) { delete pfi; }
    if (pfo != &cout) { delete pfo; }
    return 0;
}
