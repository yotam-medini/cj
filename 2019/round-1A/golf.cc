// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com -- Created: 2018/April/12

#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
// #include <set>
// #include <map>
#include <vector>
#include <utility>

#include <cstdlib>
// #include <gmpxx.h>

using namespace std;

// typedef mpz_class mpzc_t;
typedef unsigned u_t;
typedef unsigned long ul_t;
typedef unsigned long long ull_t;
typedef vector<int> vi_t;
// typedef vector<ul_t> vul_t;

static unsigned dbg_flags = 0x0;

bool readline_ints(istream &fi, vi_t &v)
{
   v.clear();
   string line;
   getline(fi, line);
   istringstream  iss(line);
   while (!iss.eof())
   {
       int x;
       iss >> x;
       if (!iss.fail())
       {
           v.push_back(x);
       }
   }
   return fi.eof();
}

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
            // cerr << x;
            (*_f) << x;
        }
        return *this;
    }
    void flush() { if (_f) { _f->flush(); } }
    bool active() const { return (_f != 0); }
 private:
    ofstream *_f;
};

class Golf
{
 public:
    Golf(istream& fi, ErrLog &el, int N, int M);
    void solve_naive(istream& fi, ostream &fo);
    void solve(istream& fi, ostream &fo);
    void print_solution(ostream&) const;
 private:
    ErrLog &errlog;
    int n, m;
    int g;
};

Golf::Golf(istream& fi, ErrLog &el, int N, int M) : 
    errlog(el), n(N), m(M), g(-1)
{
    if (dbg_flags & 0x1) { 
       errlog << "n="<<n << ", m="<<m<<"\n"; errlog.flush();}
    // getline(fi, ignore);
    // if (dbg_flags & 0x1) { 
    //   errlog << __LINE__ << ": ignore='"<<ignore << "'.\n"; errlog.flush();}
}

void Golf::solve_naive(istream& fi, ostream &fo)
{
    string ignore;
    u_t B = 18;
    if (dbg_flags & 0x1) { 
       errlog << "B="<<B << "\n"; errlog.flush();}
    for (int k = 0; k < n; ++k)
    {
        const char *sep = "";
        for (u_t b = 0; b < 18; ++b)
        {
            fo << sep << B;  sep = " ";
        }
        fo << "\n"; fo.flush();
        int delta = 0;
        if (dbg_flags & 0x1) { 
            errlog << "k="<<k << ", sent 18\n"; errlog.flush(); }
        // getline(fi, ignore);
        // if (dbg_flags & 0x1) { 
        //     errlog << "ignore="<<ignore << "\n"; errlog.flush(); }
        if (dbg_flags & 0x1) { errlog << "mills:"; }
        vi_t mills;
        readline_ints(fi, mills);
        if (dbg_flags & 0x1) { 
            errlog << "#mills=" << mills.size() << "\n"; errlog.flush(); }
        if (mills.size() != 18)
        {
            errlog << "#mills="<< mills.size() << '\n'; errlog.flush(); 
            exit(1);
        }
        for (u_t mill: mills)
        {
            delta += mill;
            if (dbg_flags & 0x1) { errlog << " " << mill; }
        }
        if (dbg_flags & 0x1) { errlog << "\n"; errlog.flush(); }
        // getline(fi, ignore);
        // if (dbg_flags & 0x1) { 
        //     errlog << "ignore="<<ignore << "\n"; errlog.flush(); }
        if (g < delta)
        {
            g = delta;
        }
    }
    fo << g << "\n"; fo.flush();
    vi_t nums;
    readline_ints(fi, nums);
    int verdict = nums[0];
    if (dbg_flags & 0x1) { 
        errlog << "verdict="<<verdict<<"\n"; errlog.flush(); }
    if (verdict == -1)
    {
        exit(1);
    }
}

void Golf::solve(istream& fi, ostream &fo)
{
    int modulos[7] = {3, 4, 5, 7, 11, 13, 17};
    int residues[7];
    for (u_t k = 0; k < 7; ++k)
    {
        const char *sep = "";
        int modulo = modulos[k];
        for (u_t bi = 0; bi < 18; ++bi)
        {
            fo << sep << modulo;  sep = " ";
        }
        fo << "\n"; fo.flush();
        u_t delta = 0;
        if (dbg_flags & 0x1) { 
            errlog << "k="<<k << ", sent 18\n"; errlog.flush(); }
        // getline(fi, ignore);
        // if (dbg_flags & 0x1) { 
        //     errlog << "ignore="<<ignore << "\n"; errlog.flush(); }
        if (dbg_flags & 0x1) { errlog << "mills:"; }
        vi_t mills;
        readline_ints(fi, mills);
        if (dbg_flags & 0x1) { 
            errlog << "#mills=" << mills.size() << "\n"; errlog.flush(); }
        if (mills.size() != 18)
        {
            errlog << "#mills="<< mills.size() << '\n'; errlog.flush(); 
            exit(1);
        }
        for (u_t mill: mills)
        {
            delta += mill;
            if (dbg_flags & 0x1) { errlog << " " << mill; }
        }
        if (dbg_flags & 0x1) { errlog << "\n"; errlog.flush(); }
        // getline(fi, ignore);
        // if (dbg_flags & 0x1) { 
        //     errlog << "ignore="<<ignore << "\n"; errlog.flush(); }
        residues[k] = delta % modulo;
    }
    for (int x = 0; (g == -1) && (x <= m); ++x)
    {
        u_t k = 0;
        for (; (k < 7) && ((x % modulos[k]) == residues[k]); ++k);
        if (k == 7)
        {
            g = x;
        }
    }
    fo << g << "\n"; fo.flush();
    vi_t nums;
    readline_ints(fi, nums);
    int verdict = nums[0];
    if (dbg_flags & 0x1) { 
        errlog << "verdict="<<verdict<<"\n"; errlog.flush(); }
    if (verdict == -1)
    {
        exit(1);
    }
}

void Golf::print_solution(ostream &fo) const
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
    vi_t nums;
    
    string ignore;
    readline_ints(*pfi, nums);
    unsigned n_cases = nums[0];
    unsigned N = nums[1];
    unsigned M = nums[2];
    // getline(*pfi, ignore);
    errlog << "n_cases="<<n_cases << "\n"; errlog.flush();

    void (Golf::*psolve)(istream&, ostream&) =
        (naive ? &Golf::solve_naive : &Golf::solve);
    for (unsigned ci = 0; ci < n_cases; ci++)
    {
        errlog << "ci+1=" << ci << "\n";
        Golf golf(*pfi, errlog, N, M);
        (golf.*psolve)(*pfi, *pfo);
    }

    if (pfi != &cin) { delete pfi; }
    if (pfo != &cout) { delete pfo; }
    return 0;
}
