// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com --

#include <iostream>
#include <fstream>
#include <map>
#include <vector>
#include <utility>

#include <cstdlib>

using namespace std;

typedef unsigned u_t;
typedef unsigned long ul_t;
typedef unsigned long long ull_t;
typedef vector<u_t> vu_t;
typedef vector<vu_t> vvu_t;
typedef pair<u_t, u_t> uu_t;
typedef map<uu_t, u_t> uu2u_t;

static unsigned dbg_flags;

class Plates
{
 public:
    Plates(istream& fi);
    void solve_naive();
    void solve();
    void print_solution(ostream&) const;
 private:
    u_t sub_solve(u_t s, u_t pending) const;
    u_t sub_solve_memo(u_t s, u_t pending);
    u_t n, k, p;
    vvu_t sbeauty;
    u_t solution;
    uu2u_t memo;
};

Plates::Plates(istream& fi) : solution(0)
{
    fi >> n >> k >> p;
    sbeauty.reserve(n);
    for (u_t si = 0; si < n; ++si)
    {
        vu_t stack;
        for (u_t pi = 0; pi < k; ++pi)
        {
            u_t b;
            fi >> b;
            stack.push_back(b);
        }
        sbeauty.push_back(stack);
    }
}

void Plates::solve_naive()
{
    solution = sub_solve(0, p);
}

void Plates::solve()
{
    solution = sub_solve_memo(0, p);
}

void Plates::print_solution(ostream &fo) const
{
    fo << ' ' << solution;
}

u_t Plates::sub_solve(u_t s, u_t pending) const
{
    u_t best = 0;
    u_t my_max = (k < pending ? k : pending);
    if (s == n - 1)
    {
        for (u_t my = 0; my < my_max; ++my)
        {
            best += sbeauty[s][my];
        }
    }
    else
    {
        u_t my_total = 0;
        for (u_t my = 0; my <= my_max; ++my)
        {
            my_total += (my > 0 ? sbeauty[s][my - 1] : 0);
            u_t candid = my_total;
            candid += sub_solve(s + 1, pending - my);
            if (best < candid)
            {
                best = candid;
            }
        }
    }
    return best;
}

u_t Plates::sub_solve_memo(u_t s, u_t pending)
{
    u_t best = 0;

    uu_t key{s, pending};
    auto er = memo.equal_range(key);
    uu2u_t::iterator iter = er.first;
    if (er.first == er.second)
    {

        u_t my_max = (k < pending ? k : pending);
        if (s == n - 1)
        {
            for (u_t my = 0; my < my_max; ++my)
            {
                best += sbeauty[s][my];
            }
        }
        else
        {
            u_t my_total = 0;
            for (u_t my = 0; my <= my_max; ++my)
            {
                my_total += (my > 0 ? sbeauty[s][my - 1] : 0);
                u_t candid = my_total;
                candid += sub_solve_memo(s + 1, pending - my);
                if (best < candid)
                {
                    best = candid;
                }
            }
        }
        uu2u_t::value_type v{key, best};
        memo.insert(iter, v);

    }
    else
    {
        best = iter->second;
    }

    return best;
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

    void (Plates::*psolve)() =
        (naive ? &Plates::solve_naive : &Plates::solve);
    ostream &fout = *pfo;
    ul_t fpos_last = pfi->tellg();
    for (unsigned ci = 0; ci < n_cases; ci++)
    {
        Plates plates(*pfi);
        getline(*pfi, ignore);
        if (tellg)
        {
            ul_t fpos = pfi->tellg();
            cerr << "Case (ci+1)="<<(ci+1) << ", tellg=[" <<
                fpos_last << " " << fpos << ") size=" <<
                (fpos - fpos_last) << "\n";
            fpos_last = fpos;
        }

        (plates.*psolve)();
        fout << "Case #"<< ci+1 << ":";
        plates.print_solution(fout);
        fout << "\n";
        fout.flush();
    }

    if (pfi != &cin) { delete pfi; }
    if (pfo != &cout) { delete pfo; }
    return 0;
}
