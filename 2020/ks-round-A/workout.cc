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
typedef map<u_t, u_t> u2u_t;

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
    u_t sub_solve(u_t di, u_t pending);
    u_t diff_add(u_t d, u_t a) const
    {
        // ceil(((d-1) - a)/(a + 1)) + 1 = int((d-1)/(a+1))+1
        u_t ret = (d - 1)/(a + 1) + 1;
        return ret;
    }
    u_t add_needed(u_t d, u_t target) const;
    u_t n, k;
    vu_t m;
    vu_t rdiffs;
    u_t solution;
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
    vu_t diffs;
    diffs.reserve(n - 1);
    for (u_t mi = 1; mi < n; ++mi)
    {
        diffs.push_back(m[mi] - m[mi - 1]);
    }
    sort(diffs.begin(), diffs.end());
    u2u_t dcount;
    for (u_t i = 0, j = 0; i < n - 1; i = j)
    {
        u_t d = diffs[i];
        for (j = i + 1; (j < n - 1) && (diffs[j] == d); ++j) {}
        u_t count = j - i;
        dcount.insert(dcount.end(), u2u_t::value_type(d, count));
    }
    const u2u_t::const_iterator dcb = dcount.begin(), dce = dcount.end();
    u_t low = 0, high = diffs.back();
    while (low + 1 < high)
    {
        u_t mid = (low + high)/2;
        bool possible = true;
        u_t need = 0;
        for (u2u_t::const_iterator dci = dcb; possible && (dci != dce); ++dci)
        {
            u_t d = dci->first, count = dci->second;
            u_t a = add_needed(d, mid) * count;
            need += a;
            possible = need <= k;
        }
        if (possible)
        {
            high = mid;
        }
        else
        {
            low = mid;
        }
    }
    solution = high;
}

u_t Workout::add_needed(u_t d, u_t target) const
{
    // target d' = ceil((d - a - 1)/(a + 1)) = flooe((d - 1)/(a + 1))
    // (d-1)/(a+1) - 1 < d' <= (d-1)/(a+1)
    // (d - 1)/(d' + 1) - 1 <  a  <=  (d - 1)/d' + 1
    // binary search for (a + 1) - assuming  1 <= d' < d
    int low = (d - 1)/(target + 1) - 1;
    int high = (d - 1)/(target);
    while (low + 1 < high)
    {
        u_t mid = (low + high)/2;
        u_t dp = diff_add(d, mid);
        if (dp > target)
        {
            low = mid;
        }
        else
        {
            high = mid;
        }
    }
    return high;
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
