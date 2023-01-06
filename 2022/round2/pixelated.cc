// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com --

// #include <algorithm>
#include <fstream>
#include <iostream>
#include <string>
#include <utility>
#include <vector>
#include <cmath>
// #include <iterator>
// #include <map>
// #include <set>

#include <cstdlib>

using namespace std;

typedef unsigned u_t;
typedef unsigned long ul_t;
typedef long long ll_t;
typedef unsigned long long ull_t;
typedef vector<u_t> vu_t;
typedef vector<bool> vb_t;
typedef vector<vb_t> vvb_t;
typedef vector<string> vs_t;

static unsigned dbg_flags;

int isqrt(int n)
{
    int r = 0;
    if (n > 0)
    {
        const ll_t nll = n;
        ll_t low = 1;
        ll_t high = nll + 1;
        while (low + 1 < high)
        {
            ll_t mid = (low + high)/2;
            if (mid*mid <= nll)
            {
                low = mid;
            }
            else
            {
                high = mid;
            }
            
        }
        r = low;
    }
    return r;
}

class Pixelated
{
 public:
    Pixelated(istream& fi);
    Pixelated(const vu_t&) {}; // TBD for test_case
    void solve_naive();
    void solve();
    void print_solution(ostream&) const;
    ull_t get_solution() const { return 0; }
 private:
    void draw_perimeter(int r);
    void draw_filled();
    void draw_filled_wrong();
    void pixels_print(const string& title, const vvb_t& p) const;
    int R;
    ull_t solution;
    vvb_t pixels;
    vvb_t pixels_wrong;
    vs_t pixels_radiis;
};

Pixelated::Pixelated(istream& fi) : solution(0)
{
    fi >> R;
}

void Pixelated::solve_naive()
{
    pixels = pixels_wrong = vvb_t(2*R + 1, vb_t(2*R + 1, false));
    draw_filled();
    draw_filled_wrong();
    for (int x = -R; x <= R; ++x)
    {
        for (int y = -R; y <= R; ++y)
        {
            if (pixels[x + R][y + R] != pixels_wrong[x + R][y + R])
            {
                ++solution;
            }
        }
    }
}

void Pixelated::draw_perimeter(int r)
{
    const int r2 = r*r;
    for (int x = -r; x <= r; ++x)
    {
        const int ix = x + R;
        const int x2 = x*x;
        const int delta = r2 - x2;
        int y = int(sqrt(delta) + 1./2.);
        pixels_wrong[-y + R][ix] = true;
        pixels_wrong[ y + R][ix] = true;
        pixels_wrong[ix][-y + R] = true;
        pixels_wrong[ix][ y + R] = true;
        if (dbg_flags & 0x2) {
            char c = '0' + (r % 10);
            pixels_radiis[-y + R][ix] = c;
            pixels_radiis[ y + R][ix] = c;
            pixels_radiis[ix][-y + R] = c;
            pixels_radiis[ix][ y + R] = c;
        }
    }
}

void Pixelated::draw_filled()
{
    for (int x = -R; x <= R; ++x)
    {
        for (int y = -R; y <= R; ++y)
        {
            int sqr_x2y2 = int(sqrt(x*x + y*y) + 1./2.);
            if (sqr_x2y2 <= R)
            {
                pixels[y + R][x + R] = true;
            }
        }
    }
    if (dbg_flags & 0x1) { pixels_print(__func__, pixels); }
}

void Pixelated::draw_filled_wrong()
{
    if (dbg_flags & 0x2) {
        pixels_radiis = vs_t(2*R + 1, string(2*R + 1, ' ')); }
    for (int r = 0; r <= R; ++r)
    {
        draw_perimeter(r);
    }
    if (dbg_flags & 0x1) { pixels_print(__func__, pixels_wrong); }
    if (dbg_flags & 0x2) {
        for (const string& row: pixels_radiis) { cerr << row << '\n'; }
    }
}

void Pixelated::pixels_print(const string& title, const vvb_t& p) const
{
    cerr << title << '\n';
    for (const vb_t& row: p)
    {
        for (bool b: row) { cerr << (b ? '*' : ' '); }
        cerr << '\n';
    }
}

void Pixelated::solve()
{
    solve_naive();
}

void Pixelated::print_solution(ostream &fo) const
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
            dbg_flags = strtoul(argv[++ai], nullptr, 0);
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

    void (Pixelated::*psolve)() =
        (naive ? &Pixelated::solve_naive : &Pixelated::solve);
    if (solve_ver == 1) { psolve = &Pixelated::solve; } // solve1
    ostream &fout = *pfo;
    ul_t fpos_last = pfi->tellg();
    for (unsigned ci = 0; ci < n_cases; ci++)
    {
        Pixelated pixelated(*pfi);
        getline(*pfi, ignore);
        if (tellg)
        {
            ul_t fpos = pfi->tellg();
            cerr << "Case (ci+1)="<<(ci+1) << ", tellg=[" <<
                fpos_last << " " << fpos << ") size=" <<
                (fpos - fpos_last) << "\n";
            fpos_last = fpos;
        }

        (pixelated.*psolve)();
        fout << "Case #"<< ci+1 << ":";
        pixelated.print_solution(fout);
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

static void save_case(const char* fn)
{
    ofstream f(fn);
    f << "1\n";
    f.close();
}

static int test_case(int argc, char ** argv)
{
    int rc = rand_range(0, 1);
    ull_t solution(-1), solution_naive(-1);
    bool small = rc == 0;
    if (dbg_flags & 0x100) { save_case("pixelated-curr.in"); }
    if (small)
    {
        Pixelated p{vu_t()};
        p.solve_naive();
        solution_naive = p.get_solution();
    }
    {
        Pixelated p{vu_t()};
        p.solve();
        solution = p.get_solution();
    }
    if (small && (solution != solution_naive))
    {
        rc = 1;
        cerr << "Failed: solution = " << solution << " != " <<
            solution_naive << " = solution_naive\n";
        save_case("pixelated-fail.in");
    }
    if (rc == 0) { cerr << "  ..." <<
        (small ? " (small) " : " (large) ") << " --> " <<
        solution << '\n'; }
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
        rc = test_case(argc, argv);
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
