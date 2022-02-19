// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com --

#include <algorithm>
#include <fstream>
#include <iostream>
#include <iterator>
#include <array>
#include <set>
#include <tuple>
#include <string>
#include <utility>
#include <vector>

#include <cstdlib>

using namespace std;

typedef unsigned char uc_t;
typedef unsigned u_t;
typedef unsigned long ul_t;
typedef unsigned long long ull_t;
typedef vector<string> vs_t;
typedef unsigned char uc_t;
typedef vector<u_t> vu_t;
typedef vector<uc_t> vuc_t;
typedef vector<vuc_t> vvuc_t;
typedef array<u_t, 2> au2_t;
typedef vector<au2_t> vau2_t;
typedef set<uc_t> setuc_t;
typedef tuple<u_t, setuc_t> usetuc_t;
typedef set<usetuc_t> set_usetuc_t;

static unsigned dbg_flags;

class Milk
{
 public:
    Milk(istream& fi);
    void solve_naive();
    void solve();
    void print_solution(ostream&) const;
 private:
    vuc_t& str2vuc(vuc_t& v, const string& s)
    {
        v.clear(); v.reserve(s.size());
        for (char c: s)
        {
            v.push_back(c != '0' ? 1 : 0);
        }
        return v;
    }
    void compute_option_on_count();
    ul_t price(const vuc_t& offer) const;
    const vuc_t& change_offer(vuc_t& offer, const setuc_t& by) const;
    // ul_t price(const setuc_t& offer) const;
    u_t N, M, P;
    vs_t s_prefs;
    vs_t s_forbiddens;
    vvuc_t prefs;
    vvuc_t forbiddens;
    ul_t solution;
    vu_t option_on_count;
    vu_t penalties;
    vuc_t base_offer;
};

Milk::Milk(istream& fi) : solution(0)
{
    fi >> N >> M >> P;
    s_prefs.reserve(N);
    prefs.reserve(N);
    copy_n(istream_iterator<string>(fi), N, back_inserter(s_prefs));
    s_forbiddens.reserve(M);
    forbiddens.reserve(M);
    copy_n(istream_iterator<string>(fi), M, back_inserter(s_forbiddens));
    vuc_t vuc;
    for (const string& s: s_prefs)
    {
        prefs.push_back(str2vuc(vuc, s));
    }
    for (const string& s: s_forbiddens)
    {
        forbiddens.push_back(str2vuc(vuc, s));
    }
}

void Milk::solve_naive()
{
    solution = N*P + 1;
    for (u_t candid = 0; candid < (1u << P); ++candid)
    {
        bool is_forbidden = false;
        for (u_t fi = 0; (fi < M) && !is_forbidden; ++fi)
        {
             const vuc_t& forbidden = forbiddens[fi];
             is_forbidden = true;
             for (u_t p = 0; (p < P) && is_forbidden; ++p)
             {
                  is_forbidden = (forbidden[p] == ((candid >> p) & 0x1));
             }
        }
        if (!is_forbidden)
        {
            ull_t complaints = 0;
            for (const vuc_t& pref: prefs)
            {
                for (u_t p = 0; (p < P); ++p)
                {
                     bool diff = (pref[p] != ((candid >> p) & 0x1));
                     if (diff)
                     {
                          ++complaints;
                     }
                }
            }
            if (solution > complaints)
            {
                solution = complaints;
            }
        }
    }
}

void Milk::solve()
{
    const u_t half = N/2;
    const set<vuc_t> set_forbiddens(forbiddens.begin(), forbiddens.end());
    compute_option_on_count();
    base_offer.reserve(P);
    for (u_t i = 0; i < P; ++i)
    {
        base_offer.push_back(option_on_count[i] <= half ? 0 : 1);
    }
    set_usetuc_t q;
    q.insert(q.end(), usetuc_t{price(base_offer), setuc_t()});
    vuc_t offer(size_t(P), 0);
    while (set_forbiddens.find(change_offer(offer, get<1>(*q.begin()))) !=
        set_forbiddens.end())
    {
        const setuc_t curr_change = get<1>(*q.begin());
        for (uc_t i = 0; i < P; ++i)
        {
            setuc_t candidate_change(curr_change);
            candidate_change.insert(candidate_change.end(), i);
            u_t penalty = price(change_offer(offer, candidate_change));
            q.insert(q.end(), usetuc_t{penalty, candidate_change});
        }
        q.erase(q.begin());
    }
    solution = get<0>(*(q.begin()));
}

void Milk::compute_option_on_count()
{
    const u_t half = N/2;
    option_on_count.reserve(P);
    penalties.reserve(P);
    for (u_t p = 0; p < P; ++p)
    {
        u_t count = 0;
        for (u_t i = 0; i < N; ++i)
        {
            count += prefs[i][p];
        }
        option_on_count.push_back(count);
        u_t majority = (half < count ? count : N - count);
        u_t minority = N - majority;
        u_t penalty = majority - minority;
        penalties.push_back(penalty);
    }
}

ul_t Milk::price(const vuc_t& offer) const
{
    ul_t total = 0;
    for (u_t i = 0; i < P; ++i)
    {
        u_t want = option_on_count[i];
        u_t complaints = (offer[i] == 0 ? want : N - want);
        total += complaints;
    }
    return total;
}

const vuc_t& Milk::change_offer(vuc_t& offer, const setuc_t& by) const
{
    for (u_t i = 0; i < P; ++i)
    {
        const uc_t boi = base_offer[i];
        offer[i] = (by.find(i) == by.end() ? boi : 1 - boi);
    }
    return offer;
}

void Milk::print_solution(ostream &fo) const
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

    void (Milk::*psolve)() =
        (naive ? &Milk::solve_naive : &Milk::solve);
    if (solve_ver == 1) { psolve = &Milk::solve; } // solve1
    ostream &fout = *pfo;
    ul_t fpos_last = pfi->tellg();
    for (unsigned ci = 0; ci < n_cases; ci++)
    {
        Milk milk(*pfi);
        getline(*pfi, ignore);
        if (tellg)
        {
            ul_t fpos = pfi->tellg();
            cerr << "Case (ci+1)="<<(ci+1) << ", tellg=[" <<
                fpos_last << " " << fpos << ") size=" <<
                (fpos - fpos_last) << "\n";
            fpos_last = fpos;
        }

        (milk.*psolve)();
        fout << "Case #"<< ci+1 << ":";
        milk.print_solution(fout);
        fout << "\n";
        fout.flush();
    }

    if (pfi != &cin) { delete pfi; }
    if (pfo != &cout) { delete pfo; }
    return 0;
}

static int test_specific(int argc, char ** argv)
{
    int rc = 0;
    return rc;
}

static int test_random(int argc, char ** argv)
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

static int test(int argc, char ** argv)
{
    int rc = ((argc > 1) && (string(argv[1]) == string("specific"))
        ? test_specific(argc - 1, argv + 1)
        : test_random(argc, argv));
    return rc;
}

int main(int argc, char **argv)
{
    int rc = ((argc > 1) && (string(argv[1]) == string("test"))
        ? test(argc - 1, argv + 1)
        : real_main(argc, argv));
    return rc;
}
