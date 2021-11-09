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
#include <unordered_map>

#include <cstdlib>

using namespace std;

typedef unsigned u_t;
typedef unsigned long ul_t;
typedef unsigned long long ull_t;
typedef vector<u_t> vu_t;
typedef array<u_t, 2> au2_t;

static unsigned dbg_flags;

typedef array<u_t, 2> au2_t;
typedef unordered_map<u_t, au2_t> u_2au2_t;
typedef unordered_map<u_t, u_2au2_t> u_2u_2au2_t;

class Banana
{
 public:
    Banana(istream& fi);
    Banana(const vu_t _B, u_t _K) : N(_B.size()), K(_K), B(_B), solution(-1) {}
    void solve_naive();
    void solve();
    void print_solution(ostream&) const;
    int get_solution() const { return solution; }
 private:
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
        if ((dbg_flags & 0x1) && ((b & (b - 1)) == 0)) {
            cerr << __func__ << ": b/N = " << b << " / " << N << '\n';
        }
        for (u_t e = b; e <= N; ++e)
        {
            const u_t bunch = subsums[e] - subsums[b];
            if (bunch <= K)
            {
                const u_t len = e - b;
                u_2u_2au2_t::iterator liter = bunch_to_len2pos.find(bunch);
                if (liter == bunch_to_len2pos.end())
                {
                    u_2u_2au2_t::value_type v(bunch, u_2au2_t());
                    liter = bunch_to_len2pos.insert(liter, v);
                }
                u_2au2_t& len_to_pos = liter->second;
                u_2au2_t::iterator piter = len_to_pos.find(len);
                if (piter == len_to_pos.end())
                {
                    u_2au2_t::value_type v(len, au2_t{b, b});
                    piter = len_to_pos.insert(piter, v);
                }
                else
                {
                    au2_t& pos_min_max = piter->second;
                    // pos_min_max[0] = is good
                    if (pos_min_max[1] >= b)
                    {
                        cerr << __FILE__ << ':'<<  __LINE__ << "Failure\n";
                    }
                    pos_min_max[1] = b;
                }
            }
        }
    }
    if (dbg_flags & 0x1) { cerr << __func__ << " end\n"; }
}

void Banana::print_solution(ostream &fo) const
{
    fo << ' ' << solution;
}

static int real_main(int argc, char ** argv)
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

int test_case(const vu_t B, u_t K)
{
    int rc = 0;
    Banana banana(B, K);
    banana.solve();
    int solution = banana.get_solution();
    if (B.size() < 0x10)
    {
        Banana banana_naive(B, K);
        banana_naive.solve_naive();
        int solution_naive = banana_naive.get_solution();
        if (solution != solution_naive)
        {
            rc = 1;
            cerr << "Inconsistent solution: "<<solution <<
                ", naive: " << solution_naive << '\n';
            cerr << "specific " << K;
            for (u_t b: B) { cerr << ' ' << b; }
            cerr << '\n';
        }
    }
    return rc;
}

static int test_specific(int argc, char ** argv)
{
    int ai = 0;
    u_t K = strtoul(argv[ai++], 0, 0);
    vu_t B;
    for ( ; ai < argc; ++ai)
    {
        u_t b = strtoul(argv[ai++], 0, 0);
        B.push_back(b);
    }
    return test_case(B, K);
}

static u_t minmax_rand(u_t vmin, u_t vmax)
{
    u_t d = vmax - vmin;
    int ret = vmin + (rand() % (d + 1));
    return ret;
}

static int test_random(int argc, char ** argv)
{
    int rc = 0;
    int ai = 0;
    u_t n_tests = strtoul(argv[ai++], 0, 0);
    u_t n_min = strtoul(argv[ai++], 0, 0);
    u_t n_max = strtoul(argv[ai++], 0, 0);
    u_t b_min = strtoul(argv[ai++], 0, 0);
    u_t b_max = strtoul(argv[ai++], 0, 0);
    u_t k_min = strtoul(argv[ai++], 0, 0);
    u_t k_max = strtoul(argv[ai++], 0, 0);
    for (u_t ti = 0; (rc == 0) && (ti < n_tests); ++ti)
    {
        cerr << "Tested: " << ti << '/' << n_tests << '\n';
        u_t N = minmax_rand(n_min, n_max);
        u_t K = minmax_rand(k_min, k_max);
        vu_t B;
        while (B.size() < N)
        {
            B.push_back(minmax_rand(b_min, b_max));
        }
        rc = test_case(B, K);
    }
    return rc;
}

static int test(int argc, char ** argv)
{
    dbg_flags = 0x1;
    int rc = ((argc > 1) && (string(argv[1]) == string("specific"))
        ? test_specific(argc - 1, argv + 1)
        : test_random(argc, argv));
    return rc;
}

int main(int argc, char **argv)
{
    int rc = ((argc > 1) && (string(argv[1]) == string("test"))
        ? test(argc - 2, argv + 2)
        : real_main(argc, argv));
    return rc;
}
