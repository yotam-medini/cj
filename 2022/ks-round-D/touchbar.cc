// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com --

#include <algorithm>
#include <fstream>
#include <iostream>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>
#include <iterator>

#include <cstdlib>

using namespace std;

typedef unsigned u_t;
typedef unsigned long ul_t;
typedef unsigned long long ull_t;
typedef long long ll_t;
typedef vector<u_t> vu_t;
typedef vector<vu_t> vvu_t;
typedef vector<ull_t> vull_t;
typedef vector<vull_t> vvull_t;
// typedef unordered_map<ull_t, vu_t> ull_to_vu_t;
class Option
{
 public:
    Option(ull_t t=0, u_t n=0) : time(t), next(n) {}
    ull_t time;
    u_t next;
};
typedef vector<Option> voption_t;
typedef vector<voption_t> vvoption_t;
typedef unordered_map<ull_t, Option> memo_t;

static unsigned dbg_flags;

class Touchbar
{
 public:
    Touchbar(istream& fi);
    Touchbar(const vu_t& _S, const vu_t& _K) :
        N(_S.size()),  S(_S), M(_K.size()), K(_K), solution(0) {}; 
    void solve_naive();
    void solve();
    void print_solution(ostream&) const;
    ull_t get_solution() const { return solution; }
 private:
    void set_dupless();
    void set_key2pos();
    ull_t get_time_from(u_t si, u_t ki);
    void show_solution_path(u_t p0) const;
    void set_best_times(
        vull_t& times, 
        const vu_t& positions,
        const vull_t& next_times,
        const vu_t& next_positions);
        
    u_t N;
    vu_t S;
    u_t M;
    vu_t K;
    ull_t solution;
    vu_t s_dupless;
    vvu_t key2pos;
    memo_t memo;
};

Touchbar::Touchbar(istream& fi) : solution(0)
{
    fi >> N;
    copy_n(istream_iterator<u_t>(fi), N, back_inserter(S));
    fi >> M;
    copy_n(istream_iterator<u_t>(fi), M, back_inserter(K));
}

void Touchbar::solve_naive()
{
    set_dupless();
    set_key2pos();
    solution = ull_t(N) * ull_t(M);
    u_t k0 = s_dupless[0];
    u_t solution_path_0 = N; // undef
    for (u_t ki: key2pos[k0])
    {
        ull_t t = get_time_from(0, ki);
        if (solution > t)
        {
            solution = t;
            solution_path_0 = ki;
        }
    }
    if (dbg_flags & 0x1) { show_solution_path(solution_path_0); }
}

void Touchbar::solve()
{
    set_dupless();
    set_key2pos();
    const u_t sz = s_dupless.size();
    const ull_t infty = ull_t(N) * ull_t(M);
    vvull_t spos_times; spos_times.reserve(sz);
    for (u_t si = 0; si < sz; ++si)
    {
        u_t c = s_dupless[si];
        spos_times.push_back(vull_t(key2pos[c].size(), infty));
    }
    const u_t c_last = s_dupless.back();
    for (u_t i = 0; i < key2pos[c_last].size(); ++i)
    {
        spos_times[sz - 1][i] = 0;
    }
    for (int sj = sz - 1, si = sj - 1; sj > 0; sj = si--)
    {
        vull_t& curr_times = spos_times[si];
        const vull_t& next_times = spos_times[sj];
        const u_t c = s_dupless[si];
        const u_t cnext = s_dupless[sj];
        const vu_t& c_positions = key2pos[c];
        const vu_t& cnext_positions = key2pos[cnext];
      if (dbg_flags & 0x200) {
        const u_t ie = curr_times.size();
        const u_t je = next_times.size();
        for (u_t i = 0; i < ie; ++i)
        {
            const u_t ki = c_positions[i];
            for (u_t j = 0; j < je; ++j)
            {
                const u_t kj = cnext_positions[j];
                ull_t dt = (ki < kj ? kj - ki : ki - kj);
                ull_t t = dt + next_times[j];
                if (curr_times[i] > t)
                {
                    curr_times[i] = t;
                }
            }
        }
      } else { // future using heap
        set_best_times(curr_times, c_positions, next_times, cnext_positions);
      }
    }
    solution = *min_element(spos_times[0].begin(), spos_times[0].end());
}

class TimeIdx
{
 public:
    TimeIdx(ll_t t=0, u_t i=0) : time_shifted(t), idx(i) {}
    ll_t time_shifted;
    u_t idx;
};
bool operator<(const TimeIdx& ti0, const TimeIdx& ti1)
{
    bool lt = (ti0.time_shifted < ti1.time_shifted); // ||
        // ((ti0.time_shifted == ti1.time_shifted) && (ti0.idx < ti1.idx));
    return lt;
}

typedef vector<TimeIdx> vtimeidx_t;

void Touchbar::set_best_times(
    vull_t& times, 
    const vu_t& positions,
    const vull_t& next_times,
    const vu_t& next_positions)
{
    // assuming:
    //    positions, next_positions sorted, 
    //    times.size() == positions.size() 
    //    next_times.size() == next_positions.size()
    //    times initialized to infinity.
    vtimeidx_t heap;

    const int sz_curr = positions.size();
    const int sz_next = next_positions.size();

    int icurr, inext;
    u_t kcurr;

    // left to right
    for (icurr = 0; (icurr < sz_curr) && (positions[icurr] < next_positions[0]);
        ++icurr) {}
    for (inext = 0; (icurr < sz_curr) && (inext <= sz_next); ++inext)
    {
        if (!heap.empty())
        {
            u_t knext = (inext < sz_next ? next_positions[inext] : u_t(-1));
            for (; (icurr < sz_curr) && ((kcurr = positions[icurr]) < knext);
                ++icurr)
            {
                const u_t kj = heap[0].idx;
                times[icurr] = (kcurr - next_positions[kj]) + next_times[kj];
            }
        }
        if (inext < sz_next)
        {
            const ll_t tshift = next_times[inext] - ll_t(next_positions[inext]);
            TimeIdx ti(-tshift, inext);
            heap.push_back(ti);
            push_heap(heap.begin(), heap.end());
        }
    }

    // right to left
    heap.clear();
    for (icurr = sz_curr - 1;
        (icurr >= 0) && (positions[icurr] > next_positions[sz_next - 1]);
         --icurr) {}
    for (inext = sz_next - 1; (icurr >= 0) && (inext >= -1); --inext)
    {
        if (!heap.empty())
        {
            u_t knext = (inext >= 0 ? next_positions[inext] : 0);
            for (; (icurr >= 0) && ((kcurr = positions[icurr]) >= knext);
                --icurr)
            {
                const u_t kj = heap[0].idx;
                const ull_t candidate =
                    (next_positions[kj] - kcurr) + next_times[kj];
                times[icurr] = min(times[icurr], candidate);
            }
        }
        if (inext >= 0)
        {
            const ll_t tshift = next_times[inext] + next_positions[inext];
            TimeIdx ti(-tshift, inext);
            heap.push_back(ti);
            push_heap(heap.begin(), heap.end());
        }
    }
}

void Touchbar::print_solution(ostream &fo) const
{
    fo << ' ' << solution;
}

void Touchbar::set_dupless()
{
    s_dupless.clear();
    s_dupless.push_back(S[0]);
    for (u_t n: S)
    {
        if (s_dupless.back() != n)
        {
            s_dupless.push_back(n);
        }
    }
}

void Touchbar::set_key2pos()
{
    u_t kmax = *max_element(K.begin(), K.end());
    key2pos = vvu_t(kmax + 1, vu_t());
    for (u_t i = 0; i < K.size(); ++i)
    {
        key2pos[K[i]].push_back(i);
    }
}

ull_t Touchbar::get_time_from(u_t si, u_t ki)
{
    ull_t time = 0;
    if (si < s_dupless.size())
    {
        ull_t memo_key = (ull_t(si) << 16) | ull_t(ki);
        memo_t::iterator iter = memo.find(memo_key);
        if (iter == memo.end())
        {
            u_t skey = s_dupless[si];
            Option best(ull_t(N) * ull_t(M), M); // undef
            for (u_t kj: key2pos[skey])
            {
                const ull_t ij_delta = (ki < kj ? kj - ki : ki - kj);
                const ull_t kj_time = get_time_from(si + 1, kj);
                const ull_t ki_time = ij_delta + kj_time;
                if (best.time > ki_time)
                {
                    best.time = ki_time;
                    best.next = kj;
                }
            }
            time = best.time;
            memo.insert(iter, memo_t::value_type(memo_key, best));
        }
        else
        {
            time = iter->second.time;
        }
    }
    return time;
}

void Touchbar::show_solution_path(u_t ki) const
{
    cerr << "Path:";
    for (u_t si = 0; si < s_dupless.size(); ++si)
    {
        ull_t memo_key = (ull_t(si) << 16) | ull_t(ki);
        memo_t::const_iterator iter = memo.find(memo_key);
        ki = iter->second.next;
        cerr << ' ' << ki;
    }
    cerr << '\n';
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

    void (Touchbar::*psolve)() =
        (naive ? &Touchbar::solve_naive : &Touchbar::solve);
    if (solve_ver == 1) { psolve = &Touchbar::solve; } // solve1
    ostream &fout = *pfo;
    ul_t fpos_last = pfi->tellg();
    for (unsigned ci = 0; ci < n_cases; ci++)
    {
        Touchbar touchbar(*pfi);
        getline(*pfi, ignore);
        if (tellg)
        {
            ul_t fpos = pfi->tellg();
            cerr << "Case (ci+1)="<<(ci+1) << ", tellg=[" <<
                fpos_last << " " << fpos << ") size=" <<
                (fpos - fpos_last) << "\n";
            fpos_last = fpos;
        }

        (touchbar.*psolve)();
        fout << "Case #"<< ci+1 << ":";
        touchbar.print_solution(fout);
        fout << "\n";
        fout.flush();
    }

    if (pfi != &cin) { delete pfi; }
    if (pfo != &cout) { delete pfo; }
    return 0;
}

static u_t rand_range(u_t nmin, u_t nmax)
{
    u_t r = nmin + rand() % (nmax + 1 - nmin);
    return r;
}

static void save_case(const char* fn, const vu_t& S, const vu_t& K)
{
    ofstream f(fn);
    f << "1\n" << S.size() << '\n';
    const char* sep = "";
    for (u_t c: S) { f << sep << c; sep = " "; } f << '\n';
    f << K.size() << '\n';
    sep = "";
    for (u_t c: K) { f << sep << c; sep = " "; } f << '\n';
    f.close();
}

static int test_case(const vu_t& S, const vu_t& K)
{
    int rc = 0;
    ull_t solution(-1), solution_naive(-1);
    bool small = rc == 0;
    if (dbg_flags & 0x100) { save_case("touchbar-curr.in", S, K); }
    if (small)
    {
        Touchbar p{S, K};
        p.solve_naive();
        solution_naive = p.get_solution();
    }
    {
        Touchbar p{S, K};
        p.solve();
        solution = p.get_solution();
    }
    if (small && (solution != solution_naive))
    {
        rc = 1;
        cerr << "Failed: solution="<<solution << " != " <<
            solution_naive << " = solution_naive\n";
        save_case("touchbar-fail.in", S, K);
    }
    if (rc == 0)
    {
        cerr << "N=" << S.size() << ", M=" << K.size() <<
            (small ? " (small) " : " (large) ") <<
            " --> " << solution << '\n';
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
    const u_t N_min = strtoul(argv[ai++], nullptr, 0);
    const u_t N_max = strtoul(argv[ai++], nullptr, 0);
    const u_t M_min = strtoul(argv[ai++], nullptr, 0);
    const u_t M_max = strtoul(argv[ai++], nullptr, 0);
    const u_t K_max = strtoul(argv[ai++], nullptr, 0);
    cerr << "n_tests=" << n_tests <<
        ", N_min=" << N_min << ", N_max=" << N_max <<
        ", M_min=" << M_min << ", M_max=" << M_max <<
        ", K_max=" << K_max <<
        '\n';
    for (u_t ti = 0; (rc == 0) && (ti < n_tests); ++ti)
    {
        cerr << "Tested: " << ti << '/' << n_tests << '\n';
        u_t N = rand_range(N_min, N_max);
        u_t M = rand_range(M_min, M_max);
        vu_t S, K; S.reserve(N); K.reserve(M);
        unordered_set<u_t> kset;
        vu_t akset;
        while (S.size() < N)
        {
            int c = 0;
            if (K.size() == M)
            {
                c = K[rand() % M];
            }
            else
            {
                c = rand_range(1, K_max);
                if (kset.find(c) == kset.end())
                {
                    kset.insert(kset.end(), c);
                    K.push_back(c);
                }
            }
            S.push_back(c);
        }
        K.insert(K.end(), M - K.size(), 0);
        random_shuffle(K.begin(), K.end());
        for (u_t i = 0; i < M; ++i)
        {
            if (K[i] == 0)
            {
                K[i] = rand_range(1, K_max);
            }
        }
        rc = test_case(S, K);
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
