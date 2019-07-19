// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com -- Created:

#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <algorithm>
#include <set>
#include <vector>
#include <array>
#include <numeric>
#include <utility>

#include <cstdlib>
#include <cstring>
#include <unistd.h>

using namespace std;

typedef unsigned u_t;
typedef unsigned long ul_t;
typedef unsigned long long ull_t;
typedef vector<u_t> vu_t;
typedef vector<int> vi_t;
typedef vector<vu_t> vvu_t;
typedef array<u_t, 5> u5_t;
typedef vector<u5_t> vu5_t;
typedef set<u5_t> setu5_t;

static unsigned dbg_flags;

void permutation_first(vu_t &p, u_t n)
{
    p = vu_t(n);
    iota(p.begin(), p.end(), 0);
}

bool permutation_next(vu_t &p)
{
    u_t n = p.size();

    // find the last 'in order'
    int j = n - 1 - 1;
    while ((j >= 0) && (p[j] > p[j + 1]))
    {
        --j;
    }
    bool ret = j >= 0;
    if (ret)
    {
        // Find last > p[j]. Must find since p[j] < p[j+1]
        u_t l;
        for (l = n - 1; p[j] >= p[l]; --l);
        swap(p[j], p[l]);
        reverse(p.begin() + j + 1, p.end());
    }
    return ret;
}

u5_t &v2u5(u5_t &u5, const vu_t &v)
{
    for (u_t i = 0; i < 5; ++i)
    {
        u5[i] = v[i];
    }
    return u5;
}

vu5_t perms5;

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
            write(x);
        }
        return *this;
    }
    template <class T> void write(const string& m, const T &c)
    {
        if (_f)
        {
            write(m);
            for (auto x: c)
            {
                write(' '); write(x);
            }
            write('\n');
            flush();
        }
    }
    void flush() { if (_f) { _f->flush(); } }
    bool active() const { return (_f != 0); }
 private:
    template <class T> void write(const T &x)
    {
        // cerr << x;
        (*_f) << x;
    }
    ofstream *_f;
};

class Problem
{
 public:
    Problem(ErrLog &el) : errlog(el) {}
    void solve(istream& fi, ostream &fo);
 private:
    void solve475(istream& fi, ostream &fo, int t);
    void solve150s(istream& fi, ostream &fo, int t);
    void solve150(istream& fi, ostream &fo);
    bool readline_ints(istream &fi, vi_t &v);
    ErrLog &errlog;
};

bool Problem::readline_ints(istream &fi, vi_t &v)
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

void Problem::solve475(istream& fi, ostream &fo, int t)
{
    vu_t perm;
    permutation_first(perm, 5);
    u5_t perm5;
    perms5.push_back(v2u5(perm5, perm));
    while (permutation_next(perm))
    {
        perms5.push_back(v2u5(perm5, perm));
    }

    string line;
    vi_t ints;
    const char *ABCDE = "ABCDE";
    for (int ti = 0; ti < t; ++ti)
    {
        setu5_t pending(perms5.begin(), perms5.end());
        for (u_t pi = 0; pi < 118; ++pi)
        {
            u5_t p;
            u_t sum = 0;
            for (u_t i = 0; i < 4; ++i)
            {
                int guess = 5*pi + i + 1;
                fo << guess << "\n"; fo.flush();
                getline(fi, line);
                char c = line[0];
                p[i] = strchr(ABCDE, c) - ABCDE;
                sum += p[i];
            }
            p[4] = (0+1+2+3+4) - sum;
            u_t n_erased = pending.erase(p);
            if (n_erased != 1)
            {
                errlog << "n_erased=" << n_erased << "\n";
                exit(1);
            }
        }
        vu5_t last2(pending.begin(), pending.end());
        vu_t gi;
        for (u_t i = 0; i < 5; ++i)
        {
            if (last2[0][i] != last2[1][i])
            {
                gi.push_back(i);
            }
        }
        u_t gi0 = gi[0];
        int guess = 5*118+ gi0 + 1;
        fo << guess << "\n"; fo.flush();
        getline(fi, line);
        char c = line[0];
        u_t rpi = strchr(ABCDE, c) - ABCDE;
        // 2 dummy
        fo << "1\n"; fo.flush(); getline(fi, line);
        fo << "1\n"; fo.flush(); getline(fi, line);
        u_t imissing = (last2[0][gi0] == rpi ? 1 : 0);
        u5_t pmissing = last2[imissing];
        for (u_t i = 0; i < 5; ++i)
        {
            fo << ABCDE[pmissing[i]];
        }
        fo << "\n"; fo.flush();
        getline(fi, line);
        char verdict = line[0];
        errlog << "ti="<<ti << ", verdict="<<verdict << "\n";
        if (verdict != 'Y')
        {
            exit(1);
        }
    }
}

void Problem::solve150(istream& fi, ostream &fo)
{
    static const char *ABCDE = "ABCDE";
    vu_t candidates(vu_t::size_type(119), 0);
    iota(candidates.begin(), candidates.end(), 0);

    string line;
    u5_t solution;
    u_t located_mask = 0;
    for (u_t pass = 0; pass < 4; ++pass)
    {
        errlog << "pass="<<pass << '\n'; errlog.flush();
        vu_t indices[5];
        u_t max_count = 0;
        for (u_t i = 0, sz = candidates.size(); i < sz; ++i)
        {
            u_t pi = candidates[i];
            u_t gi = 5*pi + pass + 1;
            fo << gi << '\n'; fo.flush();
            getline(fi, line);
            char c = line[0];
            u_t v = strchr(ABCDE, c) - ABCDE;
            errlog << "i="<<i << ", pi="<<pi << ", gi="<<gi << ", v="<<v << 
                '\n'; // errlog.flush();
            indices[v].push_back(pi);
            if (max_count < indices[v].size())
            {
                max_count = indices[v].size();
            }
        }
        u_t imiss = 5;
        errlog << "pass="<<pass << ", max_count="<<max_count << '\n';
        errlog.flush();
        for (u_t i = 0; i < 5; ++i)
        {
            errlog << " #C["<<i<<"]="<<indices[i].size();
            if (((located_mask & (1u << i)) == 0) && 
                indices[i].size() != max_count)
            {
                imiss = i;
                // i = 5; // exit loop
            }
        }
        errlog << "\npass="<<pass << ", imiss="<<imiss << '\n'; errlog.flush();
        if (imiss == 5)
        {
            errlog << "imiss not found\n"; errlog.flush();
            exit(1);
        }
        solution[pass] = imiss;
        located_mask |= (1u << imiss);
        swap(candidates, indices[imiss]);
    }
    u_t s4 = 0+1+2+3+4;
    for (u_t i = 0; i < 4; ++i)
    {
        u_t v = solution[i];
        fo << ABCDE[v];
        s4 -= v;
    }
    fo << ABCDE[s4] << "\n"; fo.flush();
    getline(fi, line);
    char verdict = line[0];
    errlog << "verdict="<<verdict << "\n";
    if (verdict != 'Y')
    {
        exit(1);
    }
}

void Problem::solve150s(istream& fi, ostream &fo, int t)
{
    for (int ti = 0; ti < t; ++ti)
    {
        errlog << "ti=" << ti << '\n'; errlog.flush();
        solve150(fi, fo);
    }
}

void Problem::solve(istream& fi, ostream &fo)
{
    vi_t ints;
    readline_ints(fi, ints);
    int t = ints[0], f = ints[1];
    errlog << "t=" << t << ", f="<< f << '\n';

    if (f == 475)
    {
        solve475(fi, fo, t);
    }
    else
    {
        solve150s(fi, fo, t);
    }
}

int main(int argc, char ** argv)
{
    const string dash("-");

    int rc = 0, ai;

    for (ai = 1; (rc == 0) && (ai < argc) && (argv[ai][0] == '-') &&
        argv[ai][1] != '\0'; ++ai)
    {
        const string opt(argv[ai]);
        if (opt == string("-debug"))
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
    if (dbg_flags & 0x2) 
    {
        errlog << "pid = " << getpid() << "\n"; errlog.flush();
        int slept = 0;
        while (slept < 90)
        {
            sleep(1);
            ++slept;
        }
    }

    string ignore;
    Problem problem(errlog);
    problem.solve(*pfi, *pfo);

    if (pfi != &cin) { delete pfi; }
    if (pfo != &cout) { delete pfo; }
    return 0;
}
