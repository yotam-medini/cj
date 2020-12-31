// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com --

#include <algorithm>
#include <fstream>
#include <iostream>
#include <iterator>
#include <numeric>
#include <string>
#include <utility>
#include <vector>

#include <cstdlib>

using namespace std;

typedef unsigned u_t;
typedef unsigned long ul_t;
typedef unsigned long long ull_t;
typedef vector<ul_t> vu_t;
typedef vector<int> vi_t;

static unsigned dbg_flags;

class Pre
{
 public:
    Pre() : 
        n(0),
        _si{size_t(-1), size_t(-1)},
        _p{size_t(-1), size_t(-1)}
    {}
    void set(size_t si, size_t p)
    {
        if (n < 2)
        {
            _si[n] = si;
            _p[n] = p;
            ++n;
        }
    }
    size_t get(size_t i) const
    {
        u_t j;
        for (j = 0; (j < n) && (_si[j] != i); ++j) {}
        size_t ret = (j < n ? _p[j] : size_t(-1));
        return ret;
    }
 private:
    u_t n;
    size_t _si[2];
    size_t _p[2];
};

class LaisComp
{
 public:
    LaisComp(const vu_t& _a) : a(_a), p(_a.size()) {}
    size_t solve_naive(vu_t& res);
    size_t solve(vu_t& res);
 private:
    bool is_lais(const vu_t& b) const
    {
        bool lais = true;
        for (size_t i = 1, sz = b.size(); lais && (i + 1 < sz); ++i)
        {
            u_t min_pre = min(b[i - 1], b[i]);
            u_t min_cur = min(b[i], b[i + 1]);
            lais = (min_pre <= min_cur);
        }
        return lais;
    }
    u_t bmin(size_t si) const
    {
        u_t ret = 0;
        if (si > 0)
        {
            size_t i = m[si];
            size_t ipre = p[i].get(si);
            u_t ai = a[i];
            u_t aipre = a[ipre];
            ret = min(ai, aipre);
        }
        return ret;
    }
    vu_t a;
    vi_t m;
    vector<Pre> p;
};

size_t LaisComp::solve_naive(vu_t& res)
{
    res.clear();
    u_t asz = a.size();
    for (size_t n = asz; res.empty(); --n)
    {
        for (u_t mask = 1; res.empty() && (mask < (1u << asz)); ++mask)
        {
            vu_t b;
            for (size_t i = 0; i < asz; ++i)
            {
                if (mask & (1u << i))
                {
                    b.push_back(a[i]);
                }
            }
            if ((b.size() == n) && is_lais(b))
            {
                res = b;
                if (dbg_flags & 0x1) { 
                    cerr << "mask=0x" << hex << mask << '\n'; }
                if (dbg_flags & 0x2) {
                   for (u_t x: b) { cerr << ' ' << x; } cerr << '\n'; }
            }
        }
    }
    return res.size();
}

size_t LaisComp::solve(vu_t& res)
{
    m.push_back(0);
    m.push_back(1);
    p[1].set(1, 0);
    for (size_t i = 2, sz = a.size(); i < sz; ++i)
    {
        const u_t ai = a[i];
        const size_t msz = m.size();
        {
            const size_t mback = m.back();
            const u_t amback = a[mback];
            const u_t candid_bmin = min(ai, amback);
            const u_t back_bmin = bmin(msz - 1);
            if (back_bmin <= candid_bmin)
            {
                p[i].set(msz, mback);
                m.push_back(i);
            }
        }
        size_t ilow = 1, ihigh = msz;
        bool possible = false;
        while (ilow < ihigh)
        {
            const size_t imid = (ilow + ihigh)/2;
            const size_t imid_m1 = imid - 1;
            const u_t cur_bmin = bmin(imid);
            const u_t pre_bmin = bmin(imid_m1);
            const size_t m_imid_m1 = m[imid_m1];
            const u_t am_imid_m1 = a[m_imid_m1];
            const u_t candid_bmin = min(ai, am_imid_m1);
            if ((pre_bmin <= candid_bmin) && (candid_bmin < cur_bmin))
            {
                possible = true;
                if (ilow == imid)
                {
                    ihigh = ilow; // exit while-loop
                }
                ilow = imid;
            }
            else
            {
                ihigh = ilow;
            }
        }
        if (possible)
        {
            p[i].set(ilow, m[ilow - 1]);
            m[ilow] = i;
        }
    }
    const size_t msz = m.size();
    res.assign(msz, 0);
    for (size_t i = m.back(), mi = i - 1; i > 0; )
    {
        size_t mmi = m[mi];
        res[--i] = a[mmi];
        mi = p[mmi].get(i);
    }
    return res.size();
}

ostream& vshow(ostream& os, const string& msg, const vu_t& x)
{
    os << msg;
    for (u_t e: x)
    {
        os << ' ' << e;
    }
    os << '\n';
    return os;
}

int test_vu(const vu_t& a)
{
    int rc = 0;
    LaisComp lcmp(a);
    vu_t b, b_naive;
    size_t bsz = lcmp.solve(b);
    size_t bsz_naive = lcmp.solve_naive(b_naive);
    if (bsz != bsz_naive)
    {
        rc = 1;
        cerr << "bsz="<<bsz << " != bsz_naive="<<bsz_naive << '\n'; 
        vshow(cerr, "b=", b);
        vshow(cerr, "b_naive=", b_naive);
        vshow(cerr, "test_specific", a);
    }
    return rc;
}

// Not to convert to combinationms with repetitions
int test_permutations()
{
    int rc = 0;
    for (size_t sz = 2; (rc == 0) && (sz < 10); ++sz)
    {
        vu_t a(sz);
	iota(a.begin(), a.end(), 0);
	for (bool more = true; more && (rc == 0);
	    more = next_permutation(a.begin(), a.end()))
	{
            rc = test_vu(a);
 	}
    }
    return rc;
}

int test_specific(int argc, char** argv)
{
    vu_t a(size_t(argc), 0);
    for (int ai = 0; ai < argc; ++ai)
    {
         a[ai] = stoi(argv[ai]);
    }
    return test_vu(a);
}

class Lais
{
 public:
    Lais(istream& fi);
    void solve_naive();
    void solve();
    void print_solution(ostream&) const;
 private:
    bool is_lais(const vu_t& b) const;
    size_t N;
    vu_t a;
    u_t solution;
};

Lais::Lais(istream& fi) : solution(0)
{
    fi >> N;
    copy_n(istream_iterator<u_t>(fi), N, back_inserter(a));
}

void Lais::solve_naive()
{
    for (size_t n = N; solution == 0; --n)
    {
        for (u_t mask = 1; (solution == 0) && (mask < (1u << N)); ++mask)
        {
            vu_t b;
            for (size_t i = 0; i < N; ++i)
            {
                if (mask & (1u << i))
                {
                    b.push_back(a[i]);
                }
            }
            if ((b.size() == n) && is_lais(b))
            {
                solution = n;
                if (dbg_flags & 0x1) { 
                    cerr << "mask=0x" << hex << mask << '\n'; }
                if (dbg_flags & 0x2) {
                   for (u_t x: b) { cerr << ' ' << x; } cerr << '\n'; }
            }
        }
    }
}

bool Lais::is_lais(const vu_t& b) const
{
   bool lais = true;
   for (size_t i = 1, sz = b.size(); lais && (i + 1 < sz); ++i)
   {
       u_t min_pre = min(b[i - 1], b[i]);
       u_t min_cur = min(b[i], b[i + 1]);
       lais = (min_pre <= min_cur);
   }
   return lais;
}

#define NEWPREV 1

void Lais::solve()
{
    // vector<Pos> p;
    // p.push_back(Pos(0, u_t(-1)));
    // p.push_back(Pos(1, 0));
    vu_t m; // m.push_back(0); // 0 -> dummy
#if defined(NEWPREV)
    vector<Pre> p(N, Pre());
#else
    vector<Pre> pp(N, Pre());
    vi_t p(N, -1);
#endif
    m.push_back(0);
    m.push_back(1);
#if defined(NEWPREV)
    p[0].set(0, size_t(-1));
    p[1].set(1, 0);
#else
    pp[0].set(0, size_t(-1));
    pp[1].set(1, 0);
    p[0] = -1;
    p[1] = 0;
#endif
    for (size_t i = 2; i < N; ++i)
    {
        const u_t ai = a[i];
        size_t mback = m.back();
#if defined(NEWPREV)
        size_t pi = p[mback].get(m.size() - 1);
#else
        size_t ppi = pp[mback].get(m.size() - 1);
        size_t pi = p[mback];
        if (pi != ppi) {
           cerr << "BADDDDDDDDDDDDDD\n";
        }
#endif
        u_t last_min = min<u_t>(a[mback], a[pi]);
        u_t curr_min = min<u_t>(ai, a[mback]);
        size_t ihigh = m.size(); //  impossible
        if (last_min <= curr_min)
        {
#if defined(NEWPREV)
            p[i].set(m.size(), mback);
#else
            pp[i].set(m.size(), mback);
            p[i] = mback;
#endif
            m.push_back(i);
        }
        // else
        {
            size_t ilow = 1;  //  possible
            while (ilow < ihigh)
            {
                size_t imid = (ilow + ihigh)/2;
                bool possible = (imid == 1);
                if (!possible)
                {
                    u_t a_pre = a[m[imid - 1]];
                    size_t imid_pre = imid - 1;
#if defined(NEWPREV)
                    size_t pm_imid_pre = p[m[imid_pre]].get(imid_pre);
#else
                    size_t pm_imid_ppre = pp[m[imid_pre]].get(imid_pre);
                    size_t pm_imid_pre = p[m[imid_pre]];
                    if (pm_imid_pre != pm_imid_ppre) {
                        cerr << "BADDDDDD\n";
                    }
#endif
                    u_t a_prepre = a[pm_imid_pre];
                    u_t mid_min = min(a_pre, a_prepre);
                    curr_min = min(ai, a_pre);
                    possible = (mid_min <= curr_min);
                }
                if (possible)
                {
                    if (ilow == imid)
                    {
                        ihigh = imid; // to exit loop
                    }
                    else
                    {
                        ilow = imid;
                    }
                }
                else
                {
                    ihigh = imid;
                }
            }
            if (ilow == 0)
            {
                if (ai <= a[m[ilow]])
                {
                    m[ilow] = i;
#if defined(NEWPREV)
                    p[i].set(0, size_t(-1));
#else
                    pp[i].set(0, size_t(-1));
                    p[i] = -1;
#endif
                }
            }
            else
            {
#if defined(NEWPREV)
                size_t ipre = p[m[ilow]].get(ilow);
                u_t preval = a[ipre];
                u_t oldval = min<u_t>(a[m[ilow]], preval);
                u_t newval = min(ai, preval);
                if (newval <= oldval)
#else                
                if (ai < a[m[ilow]]) // ??
#endif
                {
#if defined(NEWPREV)
                    p[i].set(ilow, m[ilow - 1]);
#else
                    pp[i].set(ilow, m[ilow - 1]);
                    p[i] = m[ilow - 1];
#endif
                    m[ilow] = i;
                }
            }
        }
    }
    solution = m.size();
}

void Lais::print_solution(ostream &fo) const
{
    fo << solution << '\n';
}

int main(int argc, char ** argv)
{
    const string test_cmd(argc > 1 ? argv[1] : "");
    if (test_cmd == string("test_permutations"))
    {
        return test_permutations();
    }
    else if (test_cmd == string("test_specific")) // {1, 2, 0, 3}
    {
        return test_specific(argc - 2, argv + 2);
    }
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

    void (Lais::*psolve)() =
        (naive ? &Lais::solve_naive : &Lais::solve);
    ostream &fout = *pfo;
    ul_t fpos_last = pfi->tellg();
    for (unsigned ci = 0; ci < n_cases; ci++)
    {
        Lais lais(*pfi);
        getline(*pfi, ignore);
        if (tellg)
        {
            ul_t fpos = pfi->tellg();
            cerr << "Case (ci+1)="<<(ci+1) << ", tellg=[" <<
                fpos_last << " " << fpos << ") size=" <<
                (fpos - fpos_last) << "\n";
            fpos_last = fpos;
        }

        (lais.*psolve)();
        lais.print_solution(fout);
        fout.flush();
    }

    if (pfi != &cin) { delete pfi; }
    if (pfo != &cout) { delete pfo; }
    return 0;
}
