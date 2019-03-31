// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com --

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
typedef unsigned long long ull_t;
typedef pair<u_t, u_t> uu_t;
typedef vector<u_t> vu_t;
typedef vector<uu_t> vuu_t;
typedef map<u_t, u_t> u2u_t;

static unsigned dbg_flags;

class Contention
{
 public:
    Contention(istream& fi);
    void solve_naive();
    void solve();
    void print_solution(ostream&) const;
 private:
    u_t n, q;
    vuu_t bookings;
    u_t solution;
};

Contention::Contention(istream& fi) : solution(0)
{
    fi >> n >> q;
    bookings.reserve(q);
    for (u_t i = 0; i < q; ++i)
    {
        u_t l, r;
        fi >> l >> r;
        bookings.push_back(uu_t(l - 1, r)); // b,e
    }
}

void Contention::solve_naive()
{
    vuu_t sbookings;
    sbookings.reserve(q);
    for (u_t i = 0; i < q; ++i)
    {
        const uu_t &booking = bookings[i];
        u_t sz = booking.second - booking.first;
        bookings.push_back(uu_t(sz, booking.first));
    }
    sort(sbookings.begin(), sbookings.end());
    u2u_t cover;
    u2u_t worst = n;
    for (uu_t i = 0; i < q; ++i)
    {
        const uu_t &sb = sbookings[i];
        if (cover.empty())
        {
            worst = sb.first;
            cover.insert(cover.end(), u2u_t::value_type(sb.second, sb.first));
        }
        else
        {
            auto er = cover.equal_range(sb.second);
            u2u_t::iterator i = er.first;
            if (er.first == er.second)
            {
                if (i != cover.end())
                {
                    ++i;
                    next
                }
            }
        }
    }    
}

void Contention::solve()
{
}

void Contention::print_solution(ostream &fo) const
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

    void (Contention::*psolve)() =
        (naive ? &Contention::solve_naive : &Contention::solve);
    ostream &fout = *pfo;
    ul_t fpos_last = pfi->tellg();
    for (unsigned ci = 0; ci < n_cases; ci++)
    {
        Contention problem(*pfi);
        getline(*pfi, ignore);
        if (tellg)
        {
            ul_t fpos = pfi->tellg();
            cerr << "Case (ci+1)="<<(ci+1) << ", tellg=[" <<
                fpos_last << " " << fpos << ") size=" <<
                (fpos - fpos_last) << "\n";
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
