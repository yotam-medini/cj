// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com --

#include <algorithm>
#include <fstream>
#include <iostream>
#include <iterator>
#include <map>
#include <string>
#include <tuple>
#include <unordered_map>
#include <utility>
#include <vector>

#include <cstdlib>

using namespace std;

typedef unsigned u_t;
typedef unsigned long ul_t;
typedef unsigned long long ull_t;
typedef vector<u_t> vu_t;
typedef vector<vu_t> vvu_t;

static unsigned dbg_flags;

typedef enum { SMART, NAIVE, QUEUE} method_t;

class Goods
{
 public:
    Goods(u_t _lilies=0, u_t _mark=0) : lilies(_lilies), mark(_mark) {}
    ull_t key() const
    {
        return (ull_t(mark) << 20) | ull_t(lilies);
    }
    static u_t key_to_lilies(ull_t _key) { return u_t(_key & 0xfffff); }
    static Goods goods(ull_t _key)
    {
        return Goods(_key & 0xfffff, _key >> 20);
    }
    tuple<u_t, u_t> as_tuple() const { return make_tuple(lilies, mark); }
    u_t lilies;
    u_t mark;
};
bool operator<(const Goods& g0, const Goods& g1)
{
    return g0.as_tuple() < g1.as_tuple();
}

class Coins
{
 public:
    Coins(u_t _n=0, const vu_t& _seq=vu_t()) : n(_n), seq(_seq) {}
    Coins(u_t _n, const vu_t& _seq, u_t push_me) : n(_n), seq(_seq)
    {
        if (dbg_flags & 0x2) { seq.push_back(push_me); }
    }
    u_t n;
    vu_t seq;
};

#define ORDERED_MAP 1
#if defined(ORDERED_MAP)
typedef map<Goods, Coins> goods_to_coins_t;
#else
typedef unordered_map<ull_t, Coins> goods_to_coins_t;
#endif


class Lilies
{
 public:
    Lilies(istream& fi, method_t _method);
    Lilies(const vu_t& _N, method_t _method) : method(_method), N(_N)
    {
         solve(); 
    };
    void solve();
    void print_solution(ostream& fo) const;
    const vu_t& get_solution() const { return solution; }
 private:
    u_t solve_naive(u_t n);
    void solve_queue();
    void solve_smart();
    method_t method;
    vu_t N;
    vu_t solution;
};

Lilies::Lilies(istream& fi, method_t _method) : method(_method)
{
    u_t n_cases;
    fi >> n_cases;
    copy_n(istream_iterator<u_t>(fi), n_cases, back_inserter(N));
    solve();
}

void Lilies::solve()
{
    solution.reserve(N.size());
    switch (method)
    {
     case NAIVE:
        for (u_t i = 0; i < N.size(); ++i)
        {
            u_t n = N[i];
            u_t lilies = solve_naive(n);
            solution.push_back(lilies);
        }
        break;
     case QUEUE:
        solve_queue();
        break;
     case SMART:
        solve_smart();
        break;
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
   const u_t max_lilies = *max_element(N.begin(), N.end());
   vu_t coins; coins.reserve(max_lilies + 1);
   for (u_t i = 0; i <= max_lilies; ++i) { coins.push_back(i); }
   for (u_t d = 2; d <= max_lilies; ++d)
   {
       u_t m = coins[d] = min(coins[d], coins[d - 1] + 1);
       for (u_t j = 1, j1d = 0; (j1d = (j + 1)*d) <= max_lilies; ++j)
       {
           u_t alt = min(m + 4 + 2*j, coins[j1d - 1] + 1);
           if (coins[j1d] > alt)
           {
               coins[j1d] = alt;
           }
       }
   }
   for (u_t i: N)
   {
       solution.push_back(coins[i]);
   }
}

void Lilies::solve_queue()
{
    typedef goods_to_coins_t::value_type g2cv_t;
    const u_t max_lilies = *max_element(N.begin(), N.end());
    goods_to_coins_t q, used;
    #if defined(ORDERED_MAP)
    q.insert(q.end(), goods_to_coins_t::value_type(Goods(), 0));
    #else
    q.insert(q.end(), goods_to_coins_t::value_type(Goods().key(), 0));
    #endif
    while (!q.empty())
    {
        if (dbg_flags & 0x4) {
            ull_t usz = used.size();
            if ((usz & (usz - 1)) == 0) { 
               cerr << "used-size="<<usz<<'\n'; cerr.flush(); }
        }
        typedef goods_to_coins_t::iterator iter_t;
        #if defined(ORDERED_MAP)
        const g2cv_t& curr = *q.begin();
        const Goods& goods = curr.first;
        #else
        iter_t biter = q.begin();
        const g2cv_t& curr = *biter;
        const Goods goods = Goods::goods(curr.first);
        #endif
        const u_t lilies = goods.lilies;
        const u_t mark = goods.mark;
        const u_t coins = curr.second.n;
        const vu_t& seq = curr.second.seq;
        vector<g2cv_t> values;
        if (lilies + 1 <= max_lilies)
        {
            #if defined(ORDERED_MAP)
            goods_to_coins_t::key_type key(lilies + 1, mark);
            #else
            goods_to_coins_t::key_type key = Goods(lilies + 1, mark).key();
            #endif
            values.push_back(g2cv_t(key, Coins(coins + 1, seq, 1)));
        }
        if (lilies + mark <= max_lilies)
        {
            #if defined(ORDERED_MAP)
            goods_to_coins_t::key_type key(lilies + mark, mark);
            #else
            goods_to_coins_t::key_type key = Goods(lilies + mark, mark).key();
            #endif
            values.push_back(g2cv_t(key, Coins(coins + 2, seq, 2)));
        }
        if (lilies + lilies <= max_lilies)
        {
            #if defined(ORDERED_MAP)
            goods_to_coins_t::key_type key(lilies, lilies);
            #else
            goods_to_coins_t::key_type key = Goods(lilies, lilies).key();
            #endif
            values.push_back(g2cv_t(key, Coins(coins + 4, seq, 4)));
        }
        for (const g2cv_t& v: values)
        {
            #if defined(ORDERED_MAP)
            const Goods& goods_next = v.first;
            iter_t qiter = q.find(goods_next);
            iter_t uiter = used.find(goods_next);
            #else
            const Goods goods_next = Goods::goods(v.first);
            const ull_t key_next = goods_next.key();
            iter_t qiter = q.find(goods_next.key());
            iter_t uiter = used.find(goods_next.key());
            #endif
            const u_t coins_next = v.second.n;
            if ((qiter != q.end()) && (uiter != used.end()))
            {
                cerr << "Error @ " << __FILE__ << ':' << __LINE__ << '\n';
                exit(1);
            }
            if ((qiter != q.end()) || (uiter != used.end()))
            {
                if (qiter != q.end())
                {
                    if (coins_next < qiter->second.n)
                    {
                        q.erase(qiter);
                        #if defined(ORDERED_MAP)
                        q.insert(q.end(), g2cv_t(goods_next, v.second));
                        #else
                        q.insert(q.end(), g2cv_t(key_next, v.second));
                        #endif
                    }
                }
                else // uiter != used.end()
                {
                    if (coins_next < uiter->second.n)
                    {
                        used.erase(uiter);
                        #if defined(ORDERED_MAP)
                        q.insert(q.end(), g2cv_t(goods_next, v.second));
                        #else
                        q.insert(q.end(), g2cv_t(key_next, v.second));
                        #endif
                    }
                }
            }
            else
            {
                #if defined(ORDERED_MAP)
                q.insert(q.end(), g2cv_t(goods_next, v.second));
                #else
                q.insert(q.end(), g2cv_t(key_next, v.second));
                #endif
            }
        }
        used.insert(used.end(), curr);
        #if defined(ORDERED_MAP)
        q.erase(q.begin());
        #else
        q.erase(goods.key());
        #endif
    }
    vu_t solution_till_max(max_lilies + 1, u_t(-1));
    vvu_t seqs;
    if (dbg_flags & 0x2) { seqs = vvu_t(max_lilies + 1, vu_t()); }
    for (const g2cv_t& v: used)
    {
        #if defined(ORDERED_MAP)
        const u_t lilies = v.first.lilies;
        #else
        const u_t lilies = Goods::goods(v.first).lilies;
        #endif
        const u_t coins = v.second.n;
        if (solution_till_max[lilies] > coins)
        {
            solution_till_max[lilies] = coins;
            if (dbg_flags & 0x2) { seqs[lilies] = v.second.seq; }
        }
    }
    for (u_t lilies: N)
    {
        solution.push_back(solution_till_max[lilies]);
    }
    if (dbg_flags & 0x2) {
        for (u_t i = 0; i < N.size(); ++i) {
            u_t lilies = N[i];
            cerr << "["<<i<<"] lilies="<<lilies<< ", coins="<<solution[i] <<
                ", seq=";
            for (u_t c: seqs[lilies]) { cerr << ' ' << c; }  cerr << '\n';
        }
    }
}

static int real_main(int argc, char ** argv)
{
    const string dash("-");

    method_t method = SMART;
    int rc = 0, ai;

    for (ai = 1; (rc == 0) && (ai < argc) && (argv[ai][0] == '-') &&
        argv[ai][1] != '\0'; ++ai)
    {
        const string opt(argv[ai]);
        if (opt == string("-naive"))
        {
            method = NAIVE;
        }
        else if (opt == string("-queue"))
        {
            method = QUEUE;
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

    Lilies lilies(*pfi, method);
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
    vu_t solution, solution_naive, solution_queue;
    bool small = *max_element(N.begin(), N.end()) <= 20;
    if (dbg_flags & 0x100) { save_case("lilies-curr.in", N); }
    if (small)
    {
        Lilies p(N, NAIVE);
        p.solve();
        solution_naive = p.get_solution();
    }
    {
        Lilies p(N, QUEUE);
        p.solve();
        solution = p.get_solution();
    }
    {
        Lilies p(N, SMART);
        p.solve();
        solution = p.get_solution();
    }
    if (small && (solution != solution_naive))
    {
        rc = 1;
        cerr << "Failed: solution != naive\n";
        save_case("lilies-fail.in", N);
    }
    if ((rc == 0) && (solution != solution_queue))
    {
        rc = 1;
        cerr << "Failed: solution != queue\n";
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
