// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com -- Created: 2013/April/20

#include <iostream>
#include <fstream>
#include <string>
// #include <set>
// #include <map>
// #include <vector>
#include <utility>
#include <algorithm>

#include <cstdlib>
#include <array>
#include <time.h>

using namespace std;

// typedef mpz_class mpzc_t;
typedef unsigned long ul_t;
typedef unsigned long long ull_t;
typedef array<int, 2> a2_t;
typedef vector<ul_t> vul_t;

static unsigned dbg_flags;

class Triangles
{
 public:
    Triangles(istream& fi);
    void solve_naive();
    void solve();
    void print_solution(ostream&) const;
    static void init(bool small);
    static void tini() { delete [] prods; }
 private:
    static a2_t *prods;
    static ul_t N_MAX;
    static ul_t M_MAX;
    ul_t N;
    ul_t M;
    ul_t a;
    ul_t x1, y1, x2, y2;
    bool possible;
};

a2_t* Triangles::prods;
ul_t Triangles::N_MAX = 10000;
ul_t Triangles::M_MAX = 10000;

void Triangles::init(bool small)
{
    if (small)
    {
        N_MAX = 50;
        M_MAX = 50;
    }
    ul_t NM_MAX = N_MAX * M_MAX;

    prods = new a2_t[NM_MAX + 1];
    std::streamsize bin_size = (NM_MAX + 1)*sizeof(a2_t);
    const char* fn = (small ? "small.prods" : "large.prods");
    ifstream f(fn, ios::binary);
    if (f.fail())
    {
        cerr << "Failed open " << fn << ", creating t=" << time(0) << "\n";
        a2_t *p = prods;
        for (ul_t x = 1; x <= N_MAX; ++x)
        {
            for (ul_t y = 1; y <= M_MAX; ++y, ++p)
            {
                a2_t &prod = *p;
                prod[0] = x*y;
                prod[1] = x;
            }
        }
        a2_t &after_last = *p;
        after_last[0] = NM_MAX + 1;
        after_last[1] = N_MAX + 1;
        cerr << "sorting: t=" << time(0) << "\n";
        sort(prods, prods + NM_MAX + 1);
        cerr << "sorted: t=" << time(0) << "\n";
        ofstream save(fn, ofstream::binary);
        // save.seekg(0, ios::beg);
        save.write((char *)(prods), bin_size);
        save.close();
        cerr << "saved: t=" << time(0) << "\n";
    }
    else
    {
        cerr << "Loeding: t=" << time(0) << "\n";
        f.seekg(0, ios::beg);
        f.read((char *)(prods), bin_size);
        f.close();
        cerr << "loaded: t=" << time(0) << "\n";
    }
}

Triangles::Triangles(istream& fi) :
    x1(0), y1(0), x2(0), y2(0), 
    possible(false)
{
    fi >> N >> M >> a;
}

void Triangles::solve_naive()
{
    for (unsigned cx1 = 0; cx1 <= N && !possible; ++cx1)
    {
        for (unsigned cx2 = 0; cx2 <= N && !possible; ++cx2)
        {
            for (unsigned cy1 = 0; cy1 <= M && !possible; ++cy1)
            {
                for (unsigned cy2 = 0; cy2 <= M && !possible; ++cy2)
                {
                    ul_t x1u2 = cx1 * cy2;
                    ul_t x2y1 = cx2 * cy1;
                    ul_t ca = (x1u2 > x2y1 ? x1u2 - x2y1 : x2y1 - x1u2);
                    if (a == ca)
                    {
                         possible = true;
                         x1 = cx1;
                         y1 = cy1;
                         x2 = cx2;
                         y2 = cy2;
                    }
                }
            }
        }
    }
}

void Triangles::solve()
{
    const ul_t NM = M*M;
    a2_t px{0, 0};
    for (ul_t p = a; p < NM && !possible; ++p)
    {
        px[0] = p;
        auto prod_er = equal_range(prods, prods + N*M, px,
            [](const a2_t& px0, const a2_t& px1) { return px0[0] < px1[1]; });
        if (prod_er.first < prod_er.second)
        {
            possible = true;
        }
    }
}

void Triangles::print_solution(ostream &fo) const
{
    if (possible)
    {
        fo << " 0 0 " << x1 << ' ' << y1 << ' ' << x2 << ' ' << y2;
    }
    else
    {
        fo << " IMPOSSIBLE";
    }
}

int main(int argc, char ** argv)
{
    const string dash("-");

    int n_opts = 0;
    bool naive = false;
    bool small = false;

    if ((argc > n_opts + 1) && (string(argv[n_opts + 1]) == "-naive"))
    {
        naive = true;
        n_opts += 1;
    }
    if ((argc > n_opts + 1) && (string(argv[n_opts + 1]) == "-small"))
    {
        small = true;
        n_opts += 1;
    }
    int ai_in = n_opts + 1;
    int ai_out = ai_in + 1;
    int ai_dbg = ai_out + 1;
    if (!naive)
    {
        Triangles::init(small);
    }
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

    if (ai_dbg < argc) { dbg_flags = strtoul(argv[ai_dbg], 0, 0); }

    unsigned n_cases;
    *pfi >> n_cases;

    void (Triangles::*psolve)() =
        (naive ? &Triangles::solve_naive : &Triangles::solve);

    ostream &fout = *pfo;
    bool tellg = getenv("TELLG");
    ul_t fpos_last = pfi->tellg();
    for (unsigned ci = 0; ci < n_cases; ci++)
    {
        Triangles triangles(*pfi);
        if (tellg)
        {
            ul_t fpos = pfi->tellg();
            cerr << "Case (ci+1)="<<(ci+1) << ", tellg=[" <<
                fpos_last << ", " << fpos << ")\n";
            fpos_last = fpos;
        }

        (triangles.*psolve)();
        fout << "Case #"<< ci+1 << ":";
        triangles.print_solution(fout);
        fout << "\n";
        fout.flush();

    }

    if (pfi != &cin) { delete pfi; }
    if (pfo != &cout) { delete pfo; }
    Triangles::tini();
    return 0;
}
