// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com -- Created: 2018/April/07

#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <utility>

#include <cstdlib>
#include <cmath>
#include <limits>

using namespace std;

typedef unsigned long ul_t;

static unsigned dbg_flags;

typedef long double r_t;

class V2
{
 public:
    V2(r_t x=0, r_t y=0) : a{x, y} {}
    r_t a[2];
};

static r_t trapeze_area(const V2 &v0, const V2 &v1)
{
    r_t a = (1./2.)*((v0.a[1] + v1.a[1])*(v1.a[0] - v0.a[0]));
    return a;
}

static r_t triangle_area(const V2 &v0, const V2 &v1, const V2 &v2, bool debug)
{
    if (debug) {
        cerr << "-line " << setprecision(3) <<
           v0.a[0] << ' ' << v0.a[1] << ' ' << 
           v1.a[0] << ' ' << v1.a[1] << ' ' << 
           v2.a[0] << ' ' << v2.a[1] << "\n"; }
        
    r_t a01 = trapeze_area(v0, v1);
    r_t a12 = trapeze_area(v1, v2);
    r_t a02 = trapeze_area(v0, v2);
    r_t a = fabs(a01 + a12 - a02);
    return a;
}

class V3
{
 public:
    V3(r_t x=0, r_t y=0, r_t z=0) : a{x, y, z} {}
    r_t a[3];
};

class Mat3x3
{
 public:
    Mat3x3(
        r_t a11=0, r_t a12=0, r_t a13=0,
        r_t a21=0, r_t a22=0, r_t a23=0,
        r_t a31=0, r_t a32=0, r_t a33=0
    ) :
        a{a11, a12, a13,  a21, a22, a23,  a31, a32, a33}
    {}
    r_t a[3][3];
};

static void matvec_mult(V3 &result, const Mat3x3 &m, const V3 &v)
{
    for (unsigned i = 0; i != 3; ++i)
    {
        r_t s = 0.;
        for (unsigned j = 0; j != 3; ++j)
        {
            s += m.a[i][j]*v.a[j];
        }
        result.a[i] = s;
    }
}

static void matmat_mult(Mat3x3 &result, const Mat3x3 &ml, const Mat3x3 &mr)
{
    for (unsigned i = 0; i != 3; ++i)
    {
        for (unsigned j = 0; j != 3; ++j)
        {
            r_t s = 0.;
            for (unsigned k = 0; k != 3; ++k)
            {
                s += ml.a[i][k] * mr.a[k][j];
            }
            result.a[i][j] = s;
        }
    }
}

class CubicUFO
{
 public:
    CubicUFO(istream& fi);
    void solve_naive();
    void solve();
    void print_solution(ostream&) const;
 private:
    void canon();
    void init_mats_vertices();
    void compute_mat_alpha(r_t alpha);
    r_t proj_area_alpha(r_t alpha);
    r_t area;
    static const r_t h;
    static const r_t sq2d2;
    static const Mat3x3 m45xz;
    static const V3 v[8];
    static const V3 vmid[3];
    static const unsigned tri_idx[6][3];
    V3 sol[3];
    V3 xv[8];
    Mat3x3 malpha;
};

const r_t CubicUFO::h = 1./2.;
const r_t CubicUFO::sq2d2 = M_SQRT2/2.;

const Mat3x3 CubicUFO::m45xz{
    sq2d2,  0, sq2d2,
    0,      1, 0,
    -sq2d2, 0, sq2d2
};

const V3 CubicUFO::v[8] =
{
    V3{+h, +h, +h},
    V3{+h, +h, -h},
    V3{+h, -h, +h},
    V3{+h, -h, -h},
    V3{-h, +h, +h},
    V3{-h, +h, -h},
    V3{-h, -h, +h},
    V3{-h, -h, -h}
};

const unsigned CubicUFO::tri_idx[6][3] =
{
    {0, 1, 2},
    {1, 2, 3},
    {0, 1, 4},
    {1, 4, 5},
    {0, 2, 4},
    {2, 4, 6},
};

const V3 CubicUFO::vmid[3] =
{
    V3{h, 0 ,0},
    V3{0, h, 0},
    V3{0, 0, h}
};

CubicUFO::CubicUFO(istream& fi)
{
    fi >> area;
}

void CubicUFO::solve_naive()
{
    // cos a + sin a = area
    // 2 cos(a) sin(a) = sin(2a) = area^2 - 1
    // a = arcsin(area^2 - 1)/2
    r_t  area2 = area*area;
    r_t  alpha2 = asinl(area2 - 1.);
    r_t  alpha = alpha2/2;
    r_t  cosa = cosl(alpha);
    r_t  sina = sinl(alpha);
    r_t  cosa_d2 = cosa/2.;
    r_t  sina_d2 = sina/2.;
    if (dbg_flags & 0x1) {
        r_t  err = cosa + sina - area;
        cerr << "err="<<err << "\n";
    }

    //  [  cosa   -sina  ]  [1/2  =  [cosa/2
    //  [  sina    cosa  [   0 ]  =   sina/2]

    //  [  cosa   -sina  ]  [0    =  [-sins/2
    //  [  sina    cosa  [   1/2] =   cosa/2]

    sol[0].a[0] =  cosa_d2;  sol[0].a[1] = sina_d2;  sol[0].a[2] = 0;
    sol[1].a[0] = -sina_d2;  sol[1].a[1] = cosa_d2;  sol[1].a[2] = 0;
    sol[2].a[0] = 0;         sol[2].a[1] = 0;        sol[2].a[2] = 1./2.;
    canon();
}

void CubicUFO::init_mats_vertices()
{
    for (unsigned i = 0; i != 8; ++i)
    {
        matvec_mult(xv[i], m45xz, v[i]);
    }
}

void CubicUFO::compute_mat_alpha(r_t alpha)
{
    r_t cosa = cos(alpha);
    r_t sina = sin(alpha);
    malpha = Mat3x3{
       cosa, -sina, 0,
       sina,  cosa, 0,
          0,     0, 1
    };
}

r_t CubicUFO::proj_area_alpha(r_t alpha)
{
    static int call = 0;
    bool debug = (dbg_flags & 0x2) && (call < 3);
    if (debug) { cerr << __func__ << " alpha="<<alpha << "\n"; }
    compute_mat_alpha(alpha);
    V3 xxv[7];
    V2 vxz[7];
    for (unsigned i = 0; i != 7; ++i)
    {
        matvec_mult(xxv[i], malpha, xv[i]);
        vxz[i] = V2{xxv[i].a[0], xxv[i].a[2]};
    }
    r_t ta[6];
    r_t ret = 0.;
    for (unsigned i = 0; i != 6; ++i)
    {
        const unsigned *ti = tri_idx[i];
        ta[i] = triangle_area(vxz[ti[0]], vxz[ti[1]], vxz[ti[2]], debug);
        ret += ta[i];
    }
    if (debug) { cerr << "area("<<alpha << ")="<<ret <<  "\n"; }
    ++call;
    return ret;
}

void CubicUFO::solve()
{
    if (area*area <= 2.)
    {
        solve_naive();
    }
    else
    {
        const r_t eps = std::numeric_limits<double>::epsilon();
        init_mats_vertices();
        r_t alpha_low = 0.;
        r_t alpha_high = M_PI/4.;
        if (dbg_flags & 0x1) {
            cerr << "area(0)=" << proj_area_alpha(0) << "\n";
            cerr << "area(Pi/4)=" << proj_area_alpha(alpha_high) << "\n"; }
        while (alpha_high - alpha_low > eps)
        {
            r_t alpha_mid = (alpha_low + alpha_high)/2.;
            r_t area_mid = proj_area_alpha(alpha_mid);
            r_t *alpha_who = (area_mid < area ? &alpha_low : &alpha_high);
            *alpha_who = alpha_mid;
        }
        Mat3x3 rot;
        matmat_mult(rot, malpha, m45xz);
        for (unsigned i = 0; i != 3; ++i)
        {
            matvec_mult(sol[i], rot, vmid[i]);
        }
    }
}

void CubicUFO::canon()
{
    const r_t eps = std::numeric_limits<float>::epsilon();
    for (unsigned i = 0; i < 3; ++i)
    {
        for (unsigned j = 0; j < 3; ++j)
        {
            r_t  v = sol[i].a[j];
            if (fabs(v) < eps)
            {
                sol[i].a[j] = 0;
            }
        }
    }

}

void CubicUFO::print_solution(ostream &fo) const
{
    for (unsigned i = 0; i < 3; ++i)
    {
        const char *sep = "\n";
        for (unsigned j = 0; j < 3; ++j)
        {
            fo << sep << setprecision(16) << sol[i].a[j];
            sep = " ";
        }
    }
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

    void (CubicUFO::*psolve)() =
        (naive ? &CubicUFO::solve_naive : &CubicUFO::solve);
    ostream &fout = *pfo;
    ul_t fpos_last = pfi->tellg();
    for (unsigned ci = 0; ci < n_cases; ci++)
    {
        CubicUFO cubicUFO(*pfi);
        getline(*pfi, ignore);
        if (tellg)
        {
            ul_t fpos = pfi->tellg();
            cerr << "Case (ci+1)="<<(ci+1) << ", tellg=[" <<
                fpos_last << " " << fpos << ") size=" <<
                (fpos - fpos_last) << "\n";
            fpos_last = fpos;
        }

        (cubicUFO.*psolve)();
        fout << "Case #"<< ci+1 << ":";
        cubicUFO.print_solution(fout);
        fout << "\n";
        fout.flush();
    }

    if (pfi != &cin) { delete pfi; }
    if (pfo != &cout) { delete pfo; }
    return 0;
}
