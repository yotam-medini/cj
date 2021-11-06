// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com --

#include <algorithm>
#include <numeric>
#include <fstream>
#include <iostream>
#include <iterator>
#include <string>
#include <utility>
#include <vector>
#include <array>
#include <map>
#include <set>

#include <cstdlib>

using namespace std;

typedef unsigned u_t;
typedef unsigned long ul_t;
typedef unsigned long long ull_t;
typedef vector<u_t> vu_t;
typedef array<u_t, 2> au2_t;

static unsigned dbg_flags;

// typedef set<u_t> setu_t;
typedef array<u_t, 2> au2_t;
// typedef map<u_t, setu_t> u_2setu_t;
typedef map<u_t, au2_t> u_2au2_t;
// typedef map<u_t, u_2setu_t> u_2u_2setu_t;
typedef map<u_t, u_2au2_t> u_2u_2au2_t
;

class Banana
{
 public:
    Banana(istream& fi);
    void solve_naive();
    void solve();
    void print_solution(ostream&) const;
 private:
    // int knip(u_t n, u_t target, const au2_t& last2);
    void candidate(int sol)
    {
        if ((solution == -1) || ((sol < solution)))
        {
            solution = sol;
        }
    }
    void compute_bunch_to_len_to_pos();
    u_t N, K;
    vu_t B;
    int solution;
    u_2u_2au2_t bunch_to_len2pos;
};

Banana::Banana(istream& fi) : solution(-1)
{
    fi >> N >> K;
    B.reserve(N);
    copy_n(istream_iterator<u_t>(fi), N, back_inserter(B));
}

void Banana::solve_naive()
{
    // au2_t last2 = {N, N + 2};
    // solution = knip(N, K, last2);
    if (N == 1)
    {
        if (B[0] == K)
        {
            solution = 1;
        }
    }
    else
    {
        const vu_t::const_iterator bb = B.begin();
        for (u_t b0 = 0; b0 < N; ++b0)
        {
            for (u_t b1 = b0 + 1; b1 < N; ++b1)
            {
                for (u_t e0 = b0; e0 <= b1; ++e0)
                {
                    const u_t k0 = accumulate(bb + b0, bb + e0, 0);
                    for (u_t e1 = b1; e1 <= N; ++e1)
                    {
                        const u_t k1 = accumulate(bb + b1, bb + e1, 0);
                        if (k0 + k1 == K)
                        {
                            candidate((e0 - b0) + (e1 - b1));
                        }
                    }
                }
            }
        }
    }
}   

#if 0
int Banana::knip(u_t n, u_t target, const au2_t& last2)
{
    int ret = -1;
    if (n > 0)
    {
        if (n == 1)
        {
            ret = (target == B[0] ? 1 : -1);
        }
        else
        {
            if (target == B[n - 1])
            {
                ret = 1;
            }
            else if (target > B[n - 1])
            {
                int sol_with = -1;
                if ((n != last2[0]) || (n + 1 != last2[1]))
                {
                    au2_t sub_last2 = {n - 1, last2[0]};
                    sol_with = knip(n - 1, target - B[n - 1], sub_last2);
                }
                int sol_without = knip(n - 1, target, last2);
                if (sol_with >= 0)
                {
                    sol_with += 1;
                    if (sol_without >= 0)
                    {
                        ret = min(sol_with, sol_without);
                    }
                    else
                    {
                        ret = sol_with;
                    }
                }
                else
                {
                    ret = sol_without;
                }
            }
        }
    }
    return ret;
}
#endif

void Banana::solve()
{
    compute_bunch_to_len_to_pos();
    for (auto const& x: bunch_to_len2pos)
    {
        const u_t lbunch = x.first;
        const u_t rbunch = K - lbunch;
        const u_2u_2au2_t::const_iterator riter = bunch_to_len2pos.find(rbunch);
        if (riter != bunch_to_len2pos.end())
        {
            const u_2au2_t& llen2pos = x.second;
            const u_2au2_t& rlen2pos = riter->second;
            for (auto const& llp: llen2pos)
            {
                u_t llen = llp.first;
                u_t lpos = llp.second[0];
                for (auto const& rlp: rlen2pos)
                {
                    u_t rlen = rlp.first;
                    u_t rpos = rlp.second[1];
                    if (lpos + llen <= rpos)
                    {
                        candidate(llen + rlen);
                    }
                }
            }
        }
    }
}

void Banana::compute_bunch_to_len_to_pos()
{
    // B = [2, 3, 5, 7]
    // subsums = [0, 2, 5, 10, 17]
    vu_t subsums; subsums.reserve(N + 1);
    subsums.push_back(0);
    for (u_t b: B)
    {
        subsums.push_back(subsums.back() + b);
    }
    for (u_t b = 0; b < N; ++b)
    {
        for (u_t e = b; e <= N; ++e)
        {
            const u_t bunch = subsums[e] - subsums[b];
            if (bunch <= K)
            {
                const u_t len = e - b;
                auto er = bunch_to_len2pos.equal_range(bunch);
                u_2u_2au2_t::iterator liter = er.first;
                if (er.first == er.second)
                {
                    u_2u_2au2_t::value_type v(bunch, u_2au2_t());
                    liter = bunch_to_len2pos.insert(liter, v);
                }
                u_2au2_t& len_to_pos = liter->second;
                auto per = len_to_pos.equal_range(len);
                u_2au2_t::iterator piter = per.first;
                if (per.first == per.second)
                {
                    u_2au2_t::value_type v(len, au2_t{b, b});
                    piter = len_to_pos.insert(piter, v);
                }
                else
                {
                    au2_t& pos_min_max = piter->second;
                    // pos_min_max[0] = is good
                    if (pos_min_max[1] >- b)
                    {
                        cerr << __FILE__ << ':'<<  __LINE__ << "Failure\n";
                    }
                    pos_min_max[1] = b;
                }
            }
        }
    }
}

void Banana::print_solution(ostream &fo) const
{
    fo << ' ' << solution;
}

static int test(int argc, char ** argv)
{
    int rc = 0;
    int ai = 0;
    u_t n_tests = strtoul(argv[ai++], 0, 0);
    for (u_t ti = 0; (rc == 0) && (ti < n_tests); ++ti)
    {
        cerr << "Tested: " << ti << '/' << n_tests << '\n';
    }
    return rc;
}

int main(int argc, char ** argv)
{
    const string dash("-");

    bool naive = false;
    bool tellg = false;
    u_t solve_ver = 0;
    int rc = 0, ai;

    for (ai = 1; (rc == 0) && (ai < argc) && (argv[ai][0] == '-') &&
        argv[ai][1] != '\0'; ++ai)
    {
        const string opt(argv[ai]);
        if (opt == string("-naive"))
        {
            naive = true;
        }
        else if (opt == string("-solve1"))
        {
            solve_ver = 1;
        }
        else if (opt == string("-debug"))
        {
            dbg_flags = strtoul(argv[++ai], 0, 0);
        }
        else if (opt == string("-test"))
        {
            rc = test(argc - (ai + 1), argv + (ai + 1));
            return rc;
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

    void (Banana::*psolve)() =
        (naive ? &Banana::solve_naive : &Banana::solve);
    if (solve_ver == 1) { psolve = &Banana::solve; } // solve1
    ostream &fout = *pfo;
    ul_t fpos_last = pfi->tellg();
    for (unsigned ci = 0; ci < n_cases; ci++)
    {
        Banana banana(*pfi);
        getline(*pfi, ignore);
        if (tellg)
        {
            ul_t fpos = pfi->tellg();
            cerr << "Case (ci+1)="<<(ci+1) << ", tellg=[" <<
                fpos_last << " " << fpos << ") size=" <<
                (fpos - fpos_last) << "\n";
            fpos_last = fpos;
        }

        (banana.*psolve)();
        fout << "Case #"<< ci+1 << ":";
        banana.print_solution(fout);
        fout << "\n";
        fout.flush();
    }

    if (pfi != &cin) { delete pfi; }
    if (pfo != &cout) { delete pfo; }
    return 0;
}
