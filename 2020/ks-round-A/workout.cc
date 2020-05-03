// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com --

#include <iostream>
#include <fstream>
#include <algorithm>
#include <functional>
#include <map>
#include <vector>
#include <utility>
#include <cstdlib>

using namespace std;

typedef unsigned u_t;
typedef unsigned long ul_t;
typedef unsigned long long ull_t;
typedef vector<u_t> vu_t;
typedef pair<u_t, u_t> uu_t;
typedef map<uu_t, u_t> uu2u_t;

class Delta
{
  public:
     Delta(u_t d=0, u_t m=0, u_t a=0): diff(d), mult(m), add(a) {}
     // u_t gap() const { return (diff - add)/(add + 1); }
     u_t gap() const { return (diff - 1)/(add + 1) + 1; }
     u_t diff;
     u_t mult;
     u_t add;
};
bool operator<(const Delta& d0, const Delta& d1)
{
    bool lt = (d0.diff < d1.diff);
    if ((!lt) && (d0.diff == d1.diff))
    {
        lt = (d0.mult < d1.mult);
        if ((!lt) && (d0.mult == d1.mult))
        {
            lt = (d0.add > d1.add);
        }
    }
    return lt;
}
typedef vector<Delta> vdelta_t;;
typedef map<u_t, vdelta_t> u2vdelta_t;;

static unsigned dbg_flags;

class Workout
{
 public:
    Workout(istream& fi);
    void solve_k1();
    void solve_naive();
    void solve();
    void print_solution(ostream&) const;
 private:
    void init_gap_deltas();
    void relocate(const vdelta_t&);
    u_t sub_solve(u_t di, u_t pending);
    u_t n, k;
    vu_t m;
    vu_t rdiffs;
    u_t solution;
    u_t extra;
    u2vdelta_t gap2vd, zgap2vd;
    uu2u_t memo;
};

Workout::Workout(istream& fi) : solution(0)
{
    fi >> n >> k;
    m.reserve(n);
    for (u_t mi = 0; mi < n; ++mi)
    {
        u_t x;
        fi >> x;
        m.push_back(x);
    }
}

void Workout::solve_k1()
{
    vu_t diffs;
    diffs.reserve(n - 1);
    for (u_t mi = 1; mi < n; ++mi)
    {
        diffs.push_back(m[mi] - m[mi - 1]);
    }
    sort(diffs.begin(), diffs.end());
    // assumuing k == 1
    u_t nd = diffs.size();
    solution = (diffs[nd - 1] + 1) / 2;
    if ((nd > 1) && (solution < diffs[nd- 2]))
    {
        solution = diffs[nd - 2];
    }
}

void Workout::solve_naive()
{
    if (k == 1)
    {
        solve_k1();
    }
    else
    {
        rdiffs.reserve(n - 1);
        for (u_t mi = 1; mi < n; ++mi)
        {
            rdiffs.push_back(m[mi] - m[mi - 1]);
        }
        sort(rdiffs.begin(), rdiffs.end(), greater<u_t>());
        solution = sub_solve(0, k);
    }
}

u_t Workout::sub_solve(u_t di, u_t pending)
{
    u_t best = u_t(-1);
    const uu_t key{di, pending};
    auto er = memo.equal_range(key);
    uu2u_t::iterator iter = er.first;
    if (er.first == er.second)
    {
        if (di == rdiffs.size() - 1)
        {
            best = (rdiffs[di] + pending) / (pending + 1);
        }
        else
        {
            for (u_t pi = 0; pi <= pending; ++pi)
            {
                u_t candid = (rdiffs[di] + pi) / (pi + 1);
                u_t tail_best = sub_solve(di + 1, pending - pi);
                if (candid < tail_best)
                {
                    candid = tail_best;
                }
                if (best > candid)
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

void Workout::solve()
{
    if (k == 1)
    {
        solve_k1();
    }
    else
    {
        solution = u_t(-1);
        init_gap_deltas();
        bool reducing = true;
        bool zlast_max = false;
        while (reducing && (solution > 1))
        {
            reducing = false;
            u2vdelta_t* pg2vd = &gap2vd;
            u2vdelta_t::reverse_iterator riter = gap2vd.rbegin();
            u_t gap_max = riter->first;
            u2vdelta_t::reverse_iterator zriter = zgap2vd.rbegin();
            const vdelta_t* pdeltas = &riter->second;
            bool zmax =(zriter != zgap2vd.rend()) && (gap_max < zriter->first);
            if (zmax)
            {
                pg2vd = &zgap2vd;
                gap_max = zriter->first;
                pdeltas = &zriter->second;
            }
            if (solution > gap_max)
            {
                solution = gap_max;
                reducing = true;
            }
            else
            {
                reducing = zmax && !zlast_max;
            }
            zlast_max = zmax;
            u_t need = 0;
            for (const Delta& delta: *pdeltas)
            {
                need += delta.mult;
            }
            
            u_t borrow = 0;
            u2vdelta_t::iterator iter = gap2vd.begin(), itere = gap2vd.end();
            vdelta_t modified_deltas;
            for (; (iter != itere) && (extra + borrow < need); ++iter)
            {
                if (iter->first != gap_max)
                {
                    vdelta_t& deltas = iter->second;
                    while ((!deltas.empty()) && (extra + borrow < need))
                    {
                        Delta& delta = deltas.back();
                        borrow += delta.mult;
                        --delta.add;
                        modified_deltas.push_back(delta);
                        deltas.pop_back();
                    }
                }
            }

            if (extra + borrow >= need)
            {
                if (need >= borrow)
                {
                    extra -= (need - borrow);
                }
                else
                {
                    extra += (borrow - need);
                }
                u2vdelta_t::iterator iter_max = pg2vd->find(gap_max);
                for (Delta& delta: iter_max->second)
                {
                    ++(delta.add);
                    modified_deltas.push_back(delta);
                }
                pg2vd->erase(iter_max);
            }
            relocate(modified_deltas);
        }
    }
}

void Workout::init_gap_deltas()
{
    rdiffs.reserve(n - 1);
    for (u_t mi = 1; mi < n; ++mi)
    {
        rdiffs.push_back(m[mi] - m[mi - 1]);
    }
    sort(rdiffs.begin(), rdiffs.end());
    // solution = sub_solve(0, k);
    u_t q = (m.back() - m.front())/(k + 1);
    if (q == 0) { q = 1; }
    // init
    extra = k;
    for (vu_t::const_iterator ri = rdiffs.begin(), rj = ri, re = rdiffs.end();
        (ri != re); ri = rj)
    {
        u_t diff = *ri;
        for (rj = ri; (rj != re) && (*rj == diff); ++rj) {}
        u_t mult = rj - ri;
        u_t add = diff/q;
        while (add * mult > extra)
        {
            --add;
        }
        Delta delta(diff, mult, add);
        extra -= add * mult;
        u_t gap = delta.gap();
        u2vdelta_t* pg2vd = (add == 0 ? &zgap2vd : &gap2vd);
        auto er = pg2vd->equal_range(gap);
        u2vdelta_t::iterator iter = er.first;
        if (er.first == er.second)
        {
            u2vdelta_t::value_type kv(gap, vdelta_t());
            iter = pg2vd->insert(iter, kv);
        }
        iter->second.push_back(delta);
    }
}

void Workout::relocate(const vdelta_t& deltas)
{
    for (const Delta& delta: deltas)
    {
        u_t gap = delta.gap();
        u2vdelta_t* pg2vd = (delta.add == 0 ? &zgap2vd : &gap2vd);
        auto er = pg2vd->equal_range(gap);
        u2vdelta_t::iterator iter = er.first;
        if (er.first == er.second)
        {
            u2vdelta_t::value_type kv(gap, vdelta_t());
            iter = pg2vd->insert(iter, kv);
        }
        iter->second.push_back(delta);
    }
}

void Workout::print_solution(ostream &fo) const
{
    fo << ' ' << solution;
}

int main(int argc, char ** argv)
{
    const string dash("-");

    bool naive = false;
    bool k1 = false;
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
        else if (opt == string("-k1"))
        {
            k1 = true;
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

    void (Workout::*psolve)() =
        (naive ? &Workout::solve_naive : &Workout::solve);
    if (k1)
    {
        psolve = &Workout::solve_k1;
    }
    ostream &fout = *pfo;
    ul_t fpos_last = pfi->tellg();
    for (unsigned ci = 0; ci < n_cases; ci++)
    {
        Workout workout(*pfi);
        getline(*pfi, ignore);
        if (tellg)
        {
            ul_t fpos = pfi->tellg();
            cerr << "Case (ci+1)="<<(ci+1) << ", tellg=[" <<
                fpos_last << " " << fpos << ") size=" <<
                (fpos - fpos_last) << "\n";
            fpos_last = fpos;
        }

        (workout.*psolve)();
        fout << "Case #"<< ci+1 << ":";
        workout.print_solution(fout);
        fout << "\n";
        fout.flush();
    }

    if (pfi != &cin) { delete pfi; }
    if (pfo != &cout) { delete pfo; }
    return 0;
}
