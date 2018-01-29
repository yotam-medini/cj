// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com -- Created: 2013/April/20

#include <iostream>
#include <fstream>
#include <string>
#include <utility>
#include <algorithm>

#include <cstdlib>
#include <array>
#include <time.h>

using namespace std;

typedef unsigned long ul_t;
typedef array<long, 2> a2_t;
typedef pair<const a2_t*, const a2_t*> a2_range_t;

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
    static bool lt_prod(const a2_t &px0, const a2_t &px1)
        { return px0[0] < px1[0]; }
    static bool lt_x(const a2_t &px0, const a2_t &px1)
        { return px0[1] < px1[1]; }
    static a2_t *prods;
    static long N_MAX;
    static long M_MAX;
    bool prod_range_get_xy(long &x, long &y, const a2_range_t& ar) const;
    long N;
    long M;
    long a;
    long x1, y1, x2, y2;
    bool possible;
};

a2_t* Triangles::prods;
long Triangles::N_MAX = 10000;
long Triangles::M_MAX = 10000;

void Triangles::init(bool small)
{
    if (small)
    {
        N_MAX = 50;
        M_MAX = 50;
    }
    long NM_MAX = N_MAX * M_MAX;

    prods = new a2_t[NM_MAX + 1];
    std::streamsize bin_size = (NM_MAX + 1)*sizeof(a2_t);
    const char* fn = (small ? "small.prods" : "large.prods");
    ifstream f(fn, ios::binary);
    if (f.fail())
    {
        cerr << "Failed open " << fn << ", creating t=" << time(0) << "\n";
        a2_t *p = prods;
        for (long x = 1; x <= N_MAX; ++x)
        {
            for (long y = 1; y <= M_MAX; ++y, ++p)
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
    long x2y1_min = N*M;
    for (long cx1 = 0; cx1 <= N; ++cx1)
    {
        for (long cx2 = cx1; cx2 <= N; ++cx2)
        {
            for (long cy1 = 0; cy1 <= M; ++cy1)
            {
                for (long cy2 = 0; cy2 <= cy1; ++cy2)
                {
                    long x1u2 = cx1 * cy2;
                    long x2y1 = cx2 * cy1;
                    long ca = (x1u2 > x2y1 ? x1u2 - x2y1 : x2y1 - x1u2);
                    if (a == ca && ((!possible) || (x2y1 < x2y1_min)))
                    {
                         possible = true;
                         x2y1_min = x2y1;
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
    const long NM = N*M;
    const long NM_MAX = N_MAX * M_MAX;
    const a2_t *prod_b = prods, *prod_e = prod_b + min(NM * NM, NM_MAX) + 1;
    const a2_t *sub_prod_b = prods, *sub_prod_e = prod_e;
    a2_t px{0, 0};
    for (long p = a; p <= NM && !possible; ++p)
    {
        px[0] = p;
        auto prod_er = equal_range(prod_b, prod_e, px, lt_prod);
        prod_b = prod_er.second;
        if (prod_range_get_xy(x2, y1, prod_er))
        {
            if (p == a)
            {
                possible = true;
                x1 = 0;
                y2 = 0;
            }
            else
            {
                const a2_t sub_px{p - a, 0};
                sub_prod_e = prod_b;
                auto sub_prod_er = equal_range(sub_prod_b, sub_prod_e, sub_px, 
                    lt_prod);
                sub_prod_b = sub_prod_er.second;
                possible = prod_range_get_xy(x1, y2, sub_prod_er);
            }
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

bool Triangles::prod_range_get_xy(long &x, long &y, const a2_range_t &ar)
    const
{
    bool ret = false;
    const a2_t* pb = ar.first;
    const a2_t* pe = ar.second;
    // find (x,y), 0<=x<=N  and 0<=y<=M.
    //  x within [pb, pe) where x is increasing.
    // Now y = p/x where p = (*pb)[0]. y is decreasing in [pb, pe).
    // ==>  x>= p/M.   Thus  p/M <= x <= N
    if (pb < pe)
    {
        const long p = (*pb)[0];
        const long ymax = (p + (M - 1))/M;
        const a2_t p_ymax{p, ymax};
        const a2_t *low = lower_bound(pb, pe, p_ymax, lt_x);
        if (low < pe)
        {
            const a2_t &px = *low;
            x = px[1];
            y = p/x;
            ret = (x <= N) && (y <= M);
        }
    }
    return ret;
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
