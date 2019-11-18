// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com -- Created: 2018/April/12

#include <iostream>
#include <fstream>
#include <string>
#include <set>
// #include <map>
#include <algorithm>
#include <vector>
#include <utility>

#include <cstdlib>
// #include <gmpxx.h>

using namespace std;

// typedef mpz_class mpzc_t;
typedef unsigned u_t;
typedef unsigned long ul_t;
typedef unsigned long long ull_t;
// typedef vector<ul_t> vul_t;
typedef vector<u_t> vu_t;
typedef pair<u_t, u_t> uu_t;
typedef vector<uu_t> vuu_t;
typedef set<u_t> setu_t;

static unsigned dbg_flags;

class ErrLog
{
 public:
    ErrLog(const char *fn=0) : _f(fn ? new ofstream(fn) : 0) {}
    ~ErrLog() { delete _f; }
    template <class T>
    ErrLog& operator<<(const T &x)
    {
        if (_f)
        {
            cerr << x;
            (*_f) << x;
        }
        return *this;
    }
    void flush() { if (_f) { _f->flush(); } }
    bool active() const { return (_f != 0); }
 private:
    ofstream *_f;
};

class Lolli
{
 public:
    Lolli(istream& fi, ErrLog &el);
    void solve_naive(istream& fi, ostream &fo);
    void solve(istream& fi, ostream &fo);
    void print_solution(ostream&) const;
 private:
    ErrLog &errlog;
    u_t N;
    vu_t stats;
    vuu_t sorted_stats; // Number asked, flavor
};

Lolli::Lolli(istream& fi, ErrLog &el) : errlog(el)
{
    fi >> N;
    el << "Got N="<<N<< "\n"; el.flush();
}

void Lolli::solve_naive(istream& fi, ostream &fo)
{
    string dumnl;
    setu_t supply;
    for (u_t flvi = 0; flvi < N; ++flvi)
    {
        stats.push_back(0);
        // stats.push_back(uu_t(0, fi));
        supply.insert(supply.end(), flvi);
    }
    for (u_t ci = 0; ci < N; ++ci)
    {
        u_t D;
        fi >> D;  
        if (dbg_flags & 0x1) {
            errlog << "ci="<<ci << ", D="<<D << "\n"; errlog.flush(); }
        if (D == 0)
        {
            if (dbg_flags & 0x1) {
                errlog << "ci="<<ci << ", ZERO\n"; errlog.flush(); }
            getline(fi, dumnl);
            fo << "-1\n";
        }
        else
        {
            setu_t likes;
            for (u_t di = 0; di < D; ++di)
            {
                u_t flv1 = 1313;
                fi >> flv1;
                likes.insert(likes.end(), flv1);
            }
            getline(fi, dumnl);
            if (dbg_flags & 0x1) { errlog << "likes: ";
               for (u_t flv: likes) {
                  errlog << " " << flv; } errlog << "\n"; }
            sorted_stats.clear();
            for (u_t flvi = 0; flvi < N; ++flvi)
            {
                sorted_stats.push_back(uu_t(stats[flvi], flvi));
            }
            sort(sorted_stats.begin(), sorted_stats.end());
            bool sold = false;
            for (u_t sfi = 0; (sfi < N) && !sold; ++sfi)
            {
                u_t flv = sorted_stats[sfi].second;
                if ((supply.find(flv) != supply.end()) &&
                    (likes.find(flv) != likes.end()))
                {
                    fo << flv << "\n";
                    if (dbg_flags & 0x1) { errlog << "sold: "<<flv << "\n"; }
                    sold = true;
                    supply.erase(setu_t::key_type(flv));
                }
            }
            if (!sold)
            {
                fo << "-1\n";
                if (dbg_flags & 0x1) { errlog << "sold: -1\n"; }
            }
            fo.flush();            

            for (u_t flv: likes)
            {
                stats[flv]++;
            }
        }
    }
}

void Lolli::solve(istream& fi, ostream &fo)
{
    solve_naive(fi, fo);
}

void Lolli::print_solution(ostream &fo) const
{
}

int main(int argc, char ** argv)
{
    const string dash("-");

    bool naive = false;
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

    ErrLog errlog(dbg_flags & 0x1 ? "/tmp/ymcj.log" : 0);
    string ignore;
    unsigned n_cases;
    *pfi >> n_cases;
    getline(*pfi, ignore);
    errlog << "n_cases="<<n_cases << "\n";

    void (Lolli::*psolve)(istream&, ostream&) =
        (naive ? &Lolli::solve_naive : &Lolli::solve);
    for (unsigned ci = 0; ci < n_cases; ci++)
    {
        Lolli lolli(*pfi, errlog);
        (lolli.*psolve)(*pfi, *pfo);
    }

    if (pfi != &cin) { delete pfi; }
    if (pfo != &cout) { delete pfo; }
    return 0;
}
