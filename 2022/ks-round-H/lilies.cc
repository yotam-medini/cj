// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com --

#include <algorithm>
#include <fstream>
#include <iostream>
#include <iterator>
#include <map>
#include <string>
#include <tuple>
#include <utility>
#include <vector>

#include <cstdlib>

using namespace std;

typedef unsigned u_t;
typedef unsigned long ul_t;
typedef unsigned long long ull_t;
typedef vector<u_t> vu_t;

static unsigned dbg_flags;

class Toss
{
 public:
    Toss(u_t c=0, u_t wn=0) : coins(c), well_note(wn) {}
    u_t coins;
    u_t well_note;
};
typedef map<u_t, u_t> u_to_u;
typedef u_to_u coins_to_mark_t;
typedef vector<coins_to_mark_t> vcoins_to_mark_t;

class Goods
{
 public:
    Goods(u_t _lilies=0, u_t _mark=0) : lilies(_lilies), mark(_mark) {}
    tuple<u_t, u_t> as_tuple() const { return make_tuple(lilies, mark); }
    u_t lilies;
    u_t mark;
};
bool operator<(const Goods& g0, const Goods& g1)
{
    return g0.as_tuple() < g1.as_tuple();
}
typedef map<Goods, u_t> goods_to_coins_t;


class Lilies
{
 public:
    Lilies(istream& fi, bool naive);
    Lilies(const vu_t& _N, bool _naive) : naive(_naive), N(_N) { solve(); };
    void solve();
    void print_solution(ostream& fo) const;
    const vu_t& get_solution() const { return solution; }
 private:
    u_t solve_naive(u_t n);
    void solve_smart();
    bool naive;
    vu_t N;
    vu_t solution;
};

Lilies::Lilies(istream& fi, bool _naive) : naive(_naive)
{
    u_t n_cases;
    fi >> n_cases;
    copy_n(istream_iterator<u_t>(fi), n_cases, back_inserter(N));
    solve();
}

void Lilies::solve()
{
    solution.reserve(N.size());
    if (naive)
    {
        for (u_t i = 0; i < N.size(); ++i)
        {
            u_t n = N[i];
            u_t lilies = solve_naive(n);
            solution.push_back(lilies);
        }
    }
    else
    {
        solve_smart();
    }
}

void Lilies::print_solution(ostream& fo) const
{
    for (u_t i = 0; i < solution.size(); ++i)
    {
        fo << "Case #" << i + 1 << ": " << solution[i] << '\n';
    }
}

u_t Lilies::solve_naive(u_t n)
{
    u_t lilies = 0;
    if (n <= 20)
    {
        if (n < 14)
        {
            lilies = n;
        }
        else
        {
            switch (n)
            {
             case 13:
                 lilies = 6 + 4 + 2 + 1; // 13
                 break;
             case 14:
                 lilies = 7 + 4 + 2; // 13
                 break;
             case 15:
                 lilies = 5 + 4 + 2 + 2; // 13
                 break;
             case 16:
                 lilies = 8 + 4 + 2; // 14
                 break;
             case 17:
                 lilies = 8 + 4 + 2 + 1; // 15
                 lilies = 5 + 4 + 2 + 2 + 1 + 1; // 15
                 break;
             case 18:
                 // lilies = 9 + 4 + 2; // 15
                 lilies = 6 + 4 + 2 + 2; // 14
                 break;
             case 19:
                 // lilies = 9 + 4 + 2 + 1; // 16
                 lilies = 6 + 4 + 2 + 2 + 1; // 15
                 break;
             case 20:
                 lilies = 5 + 4 + 2 + 2 + 2; // 15
                 break;
            }
        }
    }
    return lilies;
}

void Lilies::solve_smart()
{
    typedef goods_to_coins_t::value_type g2cv_t;
    const u_t max_lilies = *max_element(N.begin(), N.end());
    vcoins_to_mark_t  
        lilies_coins_to_mark(max_lilies + 1, coins_to_mark_t());
    goods_to_coins_t q, used;
    q.insert(q.end(), goods_to_coins_t::value_type(Goods(), 0));
    while (!q.empty())
    {
        typedef goods_to_coins_t::iterator iter_t;
        const g2cv_t& curr = *q.begin();
        const Goods& goods = curr.first;
        const u_t lilies = goods.lilies;
        const u_t mark = goods.mark;
        const u_t coins = curr.second;
        vector<g2cv_t> values;
        if (lilies + 1 <= max_lilies)
        {
            goods_to_coins_t::key_type key(lilies + 1, mark);
            values.push_back(g2cv_t(key, coins + 1));
        }
        if (lilies + mark <= max_lilies)
        {
            goods_to_coins_t::key_type key(lilies + mark, mark);
            values.push_back(g2cv_t(key, coins + 2));
        }
        {
            goods_to_coins_t::key_type key(lilies, lilies);
            values.push_back(g2cv_t(key, coins + 4));
        }
        for (const g2cv_t& v: values)
        {
            const Goods& goods_next = v.first;
            const u_t coins_next = v.second;
            iter_t qiter = q.find(goods_next);
            iter_t uiter = used.find(goods_next);
            if ((qiter != q.end()) && (uiter != used.end()))
            {
                cerr << "Error @ " << __FILE__ << ':' << __LINE__ << '\n';
                exit(1);
            }
            if ((qiter != q.end()) || (uiter != used.end()))
            {
                if (qiter != q.end())
                {
                    if (coins_next < qiter->second)
                    {
                        q.erase(qiter);
                        q.insert(q.end(), g2cv_t(goods_next, coins_next));
                    }
                }
                else // uiter != used.end()
                {
                    if (coins_next < uiter->second)
                    {
                        used.erase(uiter);
                        q.insert(q.end(), g2cv_t(goods_next, coins_next));
                    }
                }
            }
            else
            {
                q.insert(q.end(), g2cv_t(goods_next, coins_next));
            }
        }
        used.insert(used.end(), curr);
        q.erase(q.begin());
    }
    vu_t solution_till_max(max_lilies + 1, u_t(-1));
    for (const g2cv_t& v: used)
    {
        const u_t lilies = v.first.lilies;
        const u_t coins = v.second;
        if (solution_till_max[lilies] > coins)
        {
            solution_till_max[lilies] = coins;
        }
    }
    for (u_t lilies: N)
    {
        solution.push_back(solution_till_max[lilies]);
    }
}

static int real_main(int argc, char ** argv)
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
            dbg_flags = strtoul(argv[++ai], nullptr, 0);
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

    Lilies lilies(*pfi, naive);
    lilies.print_solution(*pfo);

    if (pfi != &cin) { delete pfi; }
    if (pfo != &cout) { delete pfo; }
    return 0;
}

static u_t rand_range(u_t nmin, u_t nmax)
{
    u_t r = nmin + rand() % (nmax + 1 - nmin);
    return r;
}

static void save_case(const char* fn, const vu_t& N)
{
    ofstream f(fn);
    f << "1\n" << N.size() << '\n';
    for (u_t n: N) { f << n << '\n'; }
    f.close();
}

static int test_case(const vu_t& N)
{
    int rc = 0;
    vu_t solution, solution_naive;
    bool small = *max_element(N.begin(), N.end()) <= 20;
    if (dbg_flags & 0x100) { save_case("lilies-curr.in", N); }
    if (small)
    {
        Lilies p(N, true);
        p.solve();
        solution_naive = p.get_solution();
    }
    {
        Lilies p(N, false);
        p.solve();
        solution = p.get_solution();
    }
    if (small && (solution != solution_naive))
    {
        rc = 1;
        cerr << "Failed: solution != naive\n";
        save_case("lilies-fail.in", N);
    }
    if (rc == 0) { cerr << "  ..." <<
        (small ? " (small) " : " (large) ") << "\n";
    }
    return rc;
}

static int test_random(int argc, char ** argv)
{
    int rc = 0;
    int ai = 0;
    if (string(argv[ai]) == string("-debug"))
    {
        dbg_flags = strtoul(argv[ai + 1], nullptr, 0);
        ai += 2;
    }
    const u_t n_tests = strtoul(argv[ai++], nullptr, 0);
    const u_t Nmin = strtoul(argv[ai++], nullptr, 0);
    const u_t Nmax = strtoul(argv[ai++], nullptr, 0);
     cerr << "n_tests=" << n_tests <<
        ", Nmin=" << Nmin << ", Nmax=" << Nmax <<
        '\n';
     for (u_t ti = 0; (rc == 0) && (ti < n_tests); ++ti)
    {
        cerr << "Tested: " << ti << '/' << n_tests << '\n';
        vu_t N;
        N.push_back(rand_range(Nmin, Nmax));
        rc = test_case(N);
    }
    return rc;
}

int main(int argc, char **argv)
{
    int rc = ((argc > 1) && (string(argv[1]) == string("test"))
        ? test_random(argc - 2, argv + 2)
        : real_main(argc, argv));
    return rc;
}
