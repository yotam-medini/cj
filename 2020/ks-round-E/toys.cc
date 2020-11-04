// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com --

#include <algorithm>
#include <fstream>
#include <iostream>
#include <array>
#include <string>
#include <utility>
#include <set>
#include <vector>

#include <cstdlib>

using namespace std;

typedef unsigned u_t;
typedef unsigned long ul_t;
typedef long long ll_t;
typedef unsigned long long ull_t;
typedef vector<u_t> vu_t;
typedef vector<ull_t> vull_t;

// typedef array<u_t, 2> au2_t;
// typedef vector<au2_t, 2> vau2_t;
typedef pair<ll_t, u_t> extra_idx_t;
typedef vector<extra_idx_t> vextra_idx_t;

static unsigned dbg_flags;

class Toy
{
 public:
    Toy(u_t _e=0, u_t _r=0) : e(_e), r(_r) {}
    ull_t e, r;
};
typedef vector<Toy> vtoy_t;

static const ull_t infi(-1);

class Toys
{
 public:
    Toys(istream& fi);
    void solve_naive();
    void solve();
    void print_solution(ostream&) const;
 private:
    ull_t time(const vtoy_t& subtoys) const;
    bool gready_infi();
    void gready_max();
    u_t n;
    vtoy_t toys;
    u_t y;
    ull_t z;
};

Toys::Toys(istream& fi) : y(0), z(0)
{
    fi >> n;
    toys.reserve(n);
    for (u_t i = 0; i < n; ++i)
    {
        u_t e, r;
        fi >> e >> r;
        toys.push_back(Toy(e, r));
    }
}

void Toys::solve_naive()
{
    y = 0;
    z = time(toys);
    for (u_t r = 1; (r < n) && (z != infi); ++r)
    {
        for (u_t mask = 0; (mask < (1u << n)) && (z != infi); ++mask)
        {
            vtoy_t subtoys;
            u_t n_bits = 0;
            for (u_t bi = 0; bi < n; ++bi)
            {
                if (mask & (1u << bi))
                {
                    ++n_bits;
                }
                else
                {
                    subtoys.push_back(toys[bi]);
                }
            }
            if (n_bits == r)
            {
                const ull_t t = time(subtoys);
                if ((t == infi) || (z < t))
                {
                    y = n_bits;
                    z = t;
                }
            }
        }
    }
}

ull_t Toys::time(const vtoy_t& subtoys) const
{
    ull_t t = infi;
    ull_t etotal = 0;
    for (const Toy& toy: subtoys)
    {
        etotal += toy.e;
    }
    bool will_cry = false;
    for (const Toy& toy: subtoys)
    {
        will_cry = will_cry || (etotal - toy.e < toy.r);
    }
    if (will_cry)
    {
        // simulate
        t = 0;
        bool cry = false;
        vull_t tleft(subtoys.size(), 0);
        u_t i = 0;
        while (!cry)
        {
            const Toy& toy = subtoys[i];
            cry = (tleft[i] > 0) && (t - tleft[i] < toy.r);
            if (!cry)
            {
                t += toy.e;
                tleft[i] = t;
            }
            i = (i + 1) % (subtoys.size());
        }
    }
    return t;
}

void Toys::solve()
{
    y = 0;
    if (!gready_infi())
    {
        gready_max();
    }
}

bool Toys::gready_infi()
{
    ull_t etotal = 0;
    for (const Toy& toy: toys)
    {
        etotal += toy.e;
    }
    bool will_cry = false;
    for (const Toy& toy: toys)
    {
        will_cry = will_cry || (etotal - toy.e < toy.r);
    }
    bool can = !will_cry;
    if (!can)
    {
        vextra_idx_t extra_idx; extra_idx.reserve(n);
        for (u_t i = 0; i < n; ++i)
        {
            const Toy& toy = toys[i];
            ll_t e = etotal - toy.e;
            ll_t extra = e - ll_t(toy.r);
            extra_idx.push_back(extra_idx_t(extra, i));
        }
        sort(extra_idx.begin(), extra_idx.end());
        u_t ndel = 0;
        ll_t extra_del = 0;
        for (ndel = 0; (ndel + 2 < n) && !can; ++ndel)
        {
            const u_t idel = extra_idx[ndel].second;
            const Toy& toy_del = toys[idel];
            extra_del += toy_del.e;
            const extra_idx_t& ei_check = extra_idx[ndel + 1];
            can = ei_check.first >= extra_del;
        }
        if (can) { y = ndel; }
    }
    if (can)
    {
        z = infi;
    }

    return can;
}

void Toys::gready_max()
{
    ull_t e_total = 0;
    for (const Toy& toy: toys)
    {
        e_total += toy.e;
    }
    z = e_total;
    ll_t e_deleted = 0;
    ull_t tcandid = z;
    set<extra_idx_t> considered;
    for (u_t i = 0; i < n; ++i)
    {
        const Toy& toy = toys[i];
        ll_t extra = (e_total - toy.e) - toy.r;
        if (extra >= 0)
        {
            tcandid += toy.e;
            if (z < tcandid)
            {
                z = tcandid;
            }
            considered.insert(considered.end(), extra_idx_t(extra, i));
        }
        else
        {
            // If we drop toy. we should decrease tcandid for pre-failing
            tcandid -= toy.e;
            e_deleted += toy.e;
            bool erasing = true;
            while (erasing)
            {
                erasing = false;
                for (set<extra_idx_t>::iterator iter = considered.begin(),
                    iter_next = iter;
                    (iter != considered.end()); iter = iter_next)
                {
                     iter_next = iter; ++iter_next;
                     const extra_idx_t& ei = *iter;
                     if (ei.first < e_deleted)
                     {
                         const Toy& toy_del = toys[ei.second];
                         tcandid -= 2*toy_del.e;
                         e_deleted += toy_del.e;
                         considered.erase(iter);
                         erasing = true;
                     }
                     else
                     {
                         iter_next = considered.end(); // exit-loop
                     }
                }
            }
        }
    }
}

void Toys::print_solution(ostream &fo) const
{
    fo << ' ' << y << ' ';
    if (z == infi)
    {
        fo << "INDEFINITELY";
    }
    else
    {
        fo << z;
    }
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

    void (Toys::*psolve)() =
        (naive ? &Toys::solve_naive : &Toys::solve);
    ostream &fout = *pfo;
    ul_t fpos_last = pfi->tellg();
    for (unsigned ci = 0; ci < n_cases; ci++)
    {
        Toys toys(*pfi);
        getline(*pfi, ignore);
        if (tellg)
        {
            ul_t fpos = pfi->tellg();
            cerr << "Case (ci+1)="<<(ci+1) << ", tellg=[" <<
                fpos_last << " " << fpos << ") size=" <<
                (fpos - fpos_last) << "\n";
            fpos_last = fpos;
        }

        (toys.*psolve)();
        fout << "Case #"<< ci+1 << ":";
        toys.print_solution(fout);
        fout << "\n";
        fout.flush();
    }

    if (pfi != &cin) { delete pfi; }
    if (pfo != &cout) { delete pfo; }
    return 0;
}
