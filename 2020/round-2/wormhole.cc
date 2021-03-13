// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com --

// #include <algorithm>
#include <fstream>
#include <iostream>
#include <string>
#include <utility>
#include <array>
#include <vector>
#include <unordered_map>

#include <cstdlib>

using namespace std;

typedef unsigned u_t;
typedef unsigned long ul_t;
typedef unsigned long long ull_t;
typedef long long ll_t;
typedef array<ll_t, 2> all2_t;
typedef array<u_t, 2> au2_t;
typedef vector<all2_t> vall2_t;
typedef vector<au2_t> vau2_t;

static unsigned dbg_flags;

ull_t gcd(ull_t m, ull_t n)
{
   while (n)
   {
      ull_t t = n;
      n = m % n;
      m = t;
   }
   return m;
}

class HashLL2 {
 public:
  size_t operator()(const all2_t& a) const noexcept {
    int a0 = a[0];
    int a1 = a[1];
    a1 = ((a1 & 0xffff) << 16) | ((a1 >> 16) & 0xffff);
    return hash<ll_t>()(a0 ^ a1);
  }
};

class Wormhole
{
 public:
    Wormhole(istream& fi);
    void solve_naive();
    void solve();
    void print_solution(ostream&) const;
 private:
    typedef unordered_map<all2_t, au2_t, HashLL2> tang2ij_t;
    void add_pairs(vau2_t& pairs, u_t h_used);
    void run_pairs(const vau2_t& pairs);
    void run_pairs_ds(const vau2_t& pairs, const au2_t& ij_dir, u_t pi, u_t ep);
    u_t next_hole(u_t hi, const au2_t& ij_dir) const;
    u_t N;
    vall2_t holes;
    u_t solution;
    tang2ij_t tang2ij;
};

Wormhole::Wormhole(istream& fi) : solution(0)
{
    fi >> N;
    holes.reserve(N);
    for (u_t i = 0; i < N; ++i)
    {
        all2_t h;
        fi >> h[0] >> h[1];
        holes.push_back(h);
    }
}

void Wormhole::solve_naive()
{
    solution = 1; // solves case of N=1
    vau2_t pairs;
    pairs.reserve(N / 2);
    add_pairs(pairs, 0);
}

void Wormhole::add_pairs(vau2_t& pairs, u_t h_used)
{
    if (pairs.size() == N/2)
    {
        run_pairs(pairs);
    }
    else
    {
        const u_t i0 = pairs.empty() ? 0 : pairs.back()[0] + 1;
        for (u_t i = i0; i < N; ++i)
        {
            const u_t ibit = 1u << i;
            if ((h_used & ibit) == 0)
            {
                h_used |= ibit;
                for (u_t j = i + 1; j < N; ++j)
                {
                    const u_t jbit = 1u << j;
                    if ((h_used & jbit) == 0)
                    {
                        h_used |= jbit;
                        au2_t p{i, j};
                        pairs.push_back(p);
                        add_pairs(pairs, h_used);
                        pairs.pop_back();
                        h_used &= ~jbit;
                    }
                }
                h_used &= ~ibit;
            }
        }
    }
}

void Wormhole::run_pairs(const vau2_t& pairs)
{
    for (u_t i = 0; i < N; ++i)
    {
        for (u_t j = 0; j < N; ++j)
        {
            if (i != j)
            {
                const au2_t direction{i, j};
                for (u_t pi = 0, np = pairs.size(); pi < np; ++pi)
                {
                    for (u_t ep: {0, 1})
                    {
                        run_pairs_ds(pairs, direction, pi, ep);
                    }
                }
            }
        }
    }
}

void 
Wormhole::run_pairs_ds(const vau2_t& pairs, const au2_t& ij_dir, u_t pi, u_t ep)
{
    if (dbg_flags & 0x1)
    {
        cerr << "pairs: {";
        for (const au2_t& p: pairs)
        {
            cerr << " (" << p[0] << ", " << p[1] << ")";
        }
        cerr << "}, dir: ([" << ij_dir[0] << "],[" << ij_dir[1] << "]), "
            "pi=" << pi << ", ep=" << ep << '\n';
    }
    u_t score = 2, used = (1u << pairs[pi][0]) | (1u << pairs[pi][1]);
    u_t hi = pairs[pi][1 - ep];
    for (u_t loops = 0; (hi < N) && (loops < N); ++loops)
    {
        hi = next_hole(hi, ij_dir);
        if (hi < N)
        {
             u_t bit = 1u << hi;
             if ((used & bit) == 0)
             {
                 used |= bit;
                 ++score;
             }
             bool pair_found = false;
             for (u_t qi = 0; (qi < N/2) && !pair_found; ++qi)
             {
                 if ((hi == pairs[qi][0]) || (hi == pairs[qi][1]))
                 {
                     pair_found = true;
                     hi = pairs[qi][(hi == pairs[qi][0]) ? 1 : 0];
                     bit = 1u << hi;
                     if ((used & bit) == 0)
                     {
                         used |= bit;
                         ++score;
                     }
                 }
             }
             if (!pair_found)
             {
                 loops = N; // just to exit-for
             }
        }
    }
    if (solution < score)
    {
        solution = score;
    }
}

u_t Wormhole::next_hole(u_t si, const au2_t& ij_dir) const
{
    u_t ret = N;
    const u_t i = ij_dir[0], j = ij_dir[1];
    const all2_t dir{holes[j][0] - holes[i][0], holes[j][1] - holes[i][1]};
    const all2_t& shole = holes[si];
    for (u_t ti = 0; ti < N; ++ti)
    {
        if (ti != si)
        {
            const all2_t& thole = holes[ti];
            if ((thole[0] - shole[0]) * dir[1] == (thole[1] - shole[1]) * dir[0])
            {
                u_t di = (dir[0] == 0 ? 1 : 0); // choose non-zero dimension
                if ((dir[di] > 0) == (thole[di] - shole[di] > 0))
                {
                    if (ret == N)
                    {
                        ret = ti;
                    }
                    else
                    {
                        if ((dir[di] > 0) == (thole[di] < holes[ret][di]))
                        {
                            ret = ti;
                        }
                    }
                }
            }
        }
    }
    return ret;
}

void Wormhole::solve()
{
    solve_naive();
}

void Wormhole::print_solution(ostream &fo) const
{
    fo << ' ' << solution;
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

    void (Wormhole::*psolve)() =
        (naive ? &Wormhole::solve_naive : &Wormhole::solve);
    ostream &fout = *pfo;
    ul_t fpos_last = pfi->tellg();
    for (unsigned ci = 0; ci < n_cases; ci++)
    {
        Wormhole wormhole(*pfi);
        getline(*pfi, ignore);
        if (tellg)
        {
            ul_t fpos = pfi->tellg();
            cerr << "Case (ci+1)="<<(ci+1) << ", tellg=[" <<
                fpos_last << " " << fpos << ") size=" <<
                (fpos - fpos_last) << "\n";
            fpos_last = fpos;
        }

        (wormhole.*psolve)();
        fout << "Case #"<< ci+1 << ":";
        wormhole.print_solution(fout);
        fout << "\n";
        fout.flush();
    }

    if (pfi != &cin) { delete pfi; }
    if (pfo != &cout) { delete pfo; }
    return 0;
}
