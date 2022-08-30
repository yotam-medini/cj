// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com --

#include <array>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <utility>
#include <vector>
#include <unordered_map>

#include <cstdlib>
#include <cstring>

using namespace std;

typedef unsigned u_t;
typedef unsigned long ul_t;
typedef unsigned long long ull_t;
typedef vector<u_t> vu_t;
typedef array<int, 2> ai2_t;
typedef array<int, 4> ai4_t; // NSEW
typedef unordered_map<ull_t, ai4_t> ull_to_ai4_t;
typedef vector<bool> vb_t;
typedef vector<vb_t> vvb_t;

static unsigned dbg_flags;

class Wiggle
{
 public:
    Wiggle(istream& fi);
    Wiggle(int _R, int _C, const ai2_t& _S,  const string& p): 
        N(p.size()), R(_R), C(_C), S(_S), prog(p) {}
    void solve_naive();
    void solve();
    void print_solution(ostream&) const;
    const ai2_t& get_solution() const { return solution; }
 private:
    u_t nsew_idx(char nsew) const
    {
        static const char* NSEW = "NSEW";
        const u_t i = strchr(NSEW, nsew) - NSEW;
        return i;
    }
    ai2_t nsew_to_drdc(char nsew)
    {
        static const ai2_t drdc[4] = {
            { -1,  0}, // N
            {  1,  0}, // S
            {  0,  1}, // E
            {  0, -1}  // W
        };
        const u_t i = nsew_idx(nsew);
        return drdc[i];
    }
    void skip_me(const ai2_t& me);
    ull_to_ai4_t::iterator rc_to_skip_init(int r, int c);
    ull_to_ai4_t::iterator rc_get_skip_iter(const ai2_t& rc)
    {
        return rc_get_skip_iter(rc[0], rc[1]);
    }
    ull_to_ai4_t::iterator rc_get_skip_iter(int r, int c)
    {
        return rc_to_skip.find(rc_key(r, c));
    }
    ull_t rc_key(u_t r, u_t c) const { return (r << 16) | c; }
    ull_t rc_key(const ai2_t& rc) const { return rc_key(rc[0], rc[1]); }
    u_t N;
    int R, C;
    ai2_t S;
    string prog;
    ai2_t solution;
    ull_to_ai4_t rc_to_skip;
};

string a2s(const ai2_t& a)
{
    ostringstream oss;
    oss << '(' << a[0] << ", " << a[1] << ')';
    return oss.str();
}

Wiggle::Wiggle(istream& fi) : solution{-1, -1}
{
    fi >> N >> R >> C >> S[0] >> S[1];
    fi >> prog;
}

void Wiggle::solve_naive()
{
    vvb_t grid(R, vb_t(C, false)); // visited ?
    ai2_t curr{S[0] - 1, S[1] - 1};
    if (dbg_flags & 0x1) { cerr << "S=" << a2s(S) << '\n'; }
    grid[curr[0]][curr[1]] = true;
    for (char c: prog)
    {
        ai2_t step = nsew_to_drdc(c);
        curr[0] += step[0];
        curr[1] += step[1];
        while (grid[curr[0]][curr[1]])
        {
            if (dbg_flags & 0x2) { cerr << "skipping:"<< a2s(curr) << '\n'; }
            curr[0] += step[0];
            curr[1] += step[1];
        }
        if (dbg_flags & 0x1) { cerr << "curr=" << a2s(curr) << '\n'; }
        grid[curr[0]][curr[1]] = true;
    }
    solution = ai2_t{curr[0] + 1, curr[1] + 1};
}

void Wiggle::solve()
{
    if (dbg_flags & 0x1) { cerr << "S=" << a2s(S) << '\n'; }
    ai2_t curr = S;
    skip_me(curr);
    for (char c: prog)
    {
        ai2_t step = nsew_to_drdc(c);
        curr[0] += step[0];
        curr[1] += step[1];
        ull_to_ai4_t::iterator iter = rc_get_skip_iter(curr);
        if (iter != rc_to_skip.end())
        {
            if (dbg_flags & 0x2) { cerr << "skipping:"<< a2s(curr) << '\n'; }
            u_t di = nsew_idx(c);
            const ai4_t& skip = iter->second;
            curr[di / 2] += skip[di];
        }
        if (dbg_flags & 0x1) { cerr << "curr=" << a2s(curr) << '\n'; }
        skip_me(curr);
    }
    solution = curr;
}

void Wiggle::skip_me(const ai2_t& me)
{
    typedef ull_to_ai4_t::iterator iter_t;
    static const ai4_t step_one{-1, +1, +1, -1};
    iter_t iter_me = rc_to_skip.insert(rc_to_skip.end(),
        ull_to_ai4_t::value_type{rc_key(me), step_one});
    iter_t iters[4];

    // may go outside grid - but no probkem
    iters[0] = rc_get_skip_iter(me[0] - 1, me[1]); // N
    iters[1] = rc_get_skip_iter(me[0] + 1, me[1]); // S
    iters[2] = rc_get_skip_iter(me[0], me[1] + 1); // E
    iters[3] = rc_get_skip_iter(me[0], me[1] - 1); // W

    // North & South
    if ((iters[0] != rc_to_skip.end()) && iters[1] != rc_to_skip.end())
    {
        iter_me->second[0] = iters[0]->second[0] - 1;
        iter_me->second[1] = iters[1]->second[1] + 1;
        iter_t iterN = rc_get_skip_iter(me[0] + iters[0]->second[0], me[1]);
        iterN->second[1] += iter_me->second[1];
        iter_t iterS = rc_get_skip_iter(me[0] + iters[1]->second[1], me[1]);
        iterS->second[0] += iter_me->second[0];
    }
    else if (iters[0] != rc_to_skip.end())
    {
        iter_me->second[0] = iters[0]->second[0] - 1;
        iter_t iterN = rc_get_skip_iter(me[0] + iters[0]->second[0], me[1]);
        iterN->second[1]++;
    }
    else if (iters[1] != rc_to_skip.end())
    {
        iter_me->second[1] = iters[1]->second[1] + 1;
        iter_t iterS = rc_get_skip_iter(me[0] + iters[1]->second[1], me[1]);
        iterS->second[0]--;
    }

    // East & West
    if ((iters[2] != rc_to_skip.end()) && iters[3] != rc_to_skip.end())
    {
        iter_me->second[2] = iters[2]->second[2] + 1;
        iter_me->second[3] = iters[3]->second[3] - 1;
        iter_t iterE = rc_get_skip_iter(me[0], me[1] + iters[2]->second[2]);
        iterE->second[3] += iter_me->second[3];
        iter_t iterW = rc_get_skip_iter(me[0], me[1] + iters[3]->second[3]);
        iterW->second[2] += iter_me->second[2];
    }
    else if (iters[2] != rc_to_skip.end())
    {
        iter_me->second[2] = iters[2]->second[2] + 1;
        iter_t iterE = rc_get_skip_iter(me[0], me[1] + iters[2]->second[2]);
        iterE->second[3]--;
    }
    else if (iters[3] != rc_to_skip.end())
    {
        iter_me->second[3] = iters[3]->second[3] - 1;
        iter_t iterW = rc_get_skip_iter(me[0], me[1] + iters[3]->second[3]);
        iterW->second[2]++;
    }
}

ull_to_ai4_t::iterator Wiggle::rc_to_skip_init(int r, int c)
{
    ull_to_ai4_t::iterator iter = rc_to_skip.end();
    if (0 <= r && r < R && 0 <= c && c < C)
    {
        ull_t key = (ull_t(r) << 16) | ull_t(r);
        iter = rc_to_skip.find(key);
        if (iter == rc_to_skip.end())
        {
            ai4_t step{-1, +1, +1, -1};  // NSEW
            iter = rc_to_skip.insert(iter, ull_to_ai4_t::value_type{key, step});
        }
    }
    return iter;
}

void Wiggle::print_solution(ostream &fo) const
{
    fo << ' ' << solution[0] << ' ' << solution[1];
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

    void (Wiggle::*psolve)() =
        (naive ? &Wiggle::solve_naive : &Wiggle::solve);
    if (solve_ver == 1) { psolve = &Wiggle::solve; } // solve1
    ostream &fout = *pfo;
    ul_t fpos_last = pfi->tellg();
    for (unsigned ci = 0; ci < n_cases; ci++)
    {
        Wiggle wiggle(*pfi);
        getline(*pfi, ignore);
        if (tellg)
        {
            ul_t fpos = pfi->tellg();
            cerr << "Case (ci+1)="<<(ci+1) << ", tellg=[" <<
                fpos_last << " " << fpos << ") size=" <<
                (fpos - fpos_last) << "\n";
            fpos_last = fpos;
        }

        (wiggle.*psolve)();
        fout << "Case #"<< ci+1 << ":";
        wiggle.print_solution(fout);
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

static void save_case(const char* fn, int R, int C, const string& prog)
{
    const ai2_t S{R/2, C/2};
    ofstream f(fn);
    f << "1\n" << prog.size() << ' ' << R << ' ' << C << ' ' <<
        S[0] << ' ' << S[1] <<
        '\n' << prog << '\n';
    f.close();
}

static int test_case(int R, int C, const string& prog)
{
    int rc = 0;
    ai2_t solution{-1, -1}, solution_naive{-1, -1};
    const ai2_t S{R/2, C/2};
    const u_t N = prog.size();
    bool small = N < 0x10;
    // save_case("wiggle-curr.in", R, C, prog);
    if (small)
    {
        Wiggle p(R, C, S, prog);
        p.solve_naive();
        solution_naive = p.get_solution();
    }
    {
        Wiggle p(R, C, S, prog);
        p.solve();
        solution = p.get_solution();
    }
    if (small && (solution != solution_naive))
    {
        rc = 1;
        cerr << "Failed: solution="<<a2s(solution) << " != " <<
            a2s(solution_naive) << " = solution_naive\n";
        save_case("wiggle-fail.in", R, C, prog);
    }
    const string summary = N < 0x10 ? prog 
        : prog.substr(0, 4) + string("...") + prog.substr(N - 4);

    cerr << "   S=" << a2s(S) << ' ' << summary << ' ' << a2s(solution) << '\n';
    return rc;
}

static int test_random(int argc, char ** argv)
{
    int rc = 0;
    int ai = 0;
    u_t n_tests = strtoul(argv[ai++], 0, 0);
    u_t Rmin = strtoul(argv[ai++], 0, 0);
    u_t Rmax = strtoul(argv[ai++], 0, 0);
    u_t Cmin = strtoul(argv[ai++], 0, 0);
    u_t Cmax = strtoul(argv[ai++], 0, 0);
    u_t Nmin = strtoul(argv[ai++], 0, 0);
    u_t Nmax = strtoul(argv[ai++], 0, 0);
    cerr << "n_tests="<<n_tests << 
        ", Rmin="<<Rmin << ", Rmax="<<Rmax <<
        ", Cmin="<<Cmin << ", Cmax="<<Cmax <<
        ", Nmin="<<Nmin << ", Nmax="<<Nmax <<
        '\n';
    for (u_t ti = 0; (rc == 0) && (ti < n_tests); ++ti)
    {
        cerr << "Tested: " << ti << '/' << n_tests << '\n';
        u_t R = rand_range(Rmin, Rmax);
        u_t C = rand_range(Cmin, Cmax);
        u_t N = rand_range(Nmin, Nmax);
        string prog;
        while (prog.size() < N)
        {
            prog.push_back("NSEW"[rand() % 4]);
        }
        rc = test_case(R, C, prog);
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
