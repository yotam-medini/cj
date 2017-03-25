// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com -- Created: 2013/April/20

#include <iostream>
#include <iomanip>
#include <fstream>
#include <cmath>
#include <vector>
#include <utility>

#include <cstdlib>

using namespace std;

static unsigned dbg_flags;

class FireFly
{
 public:
    FireFly(double x, double y, double z,
        double vx, double vy, double vz) :
        X{x, y, z},
        V{vx, vy, vz}
       {}
    double X[3];
    double V[3];
};
typedef vector<FireFly> vff_t;

class CenterMass
{
 public:
    CenterMass(istream& fi);
    void solve_naive();
    void solve();
    void print_solution(ostream&) const;
 private:
    unsigned n;
    vff_t fireflys;
    double dmin, tmin;
};

CenterMass::CenterMass(istream& fi)
{
    fi >> n;
    fireflys.reserve(n);
    for (unsigned i = 0; i < n; ++i)
    {
        double x, y, z, vx, vy, vz;
        fi >> x >> y >> z >> vx >> vy >> vz;
        fireflys.push_back(FireFly(x, y, z, vx, vy, vz));
    }
}

void CenterMass::solve_naive()
{
}

void CenterMass::solve()
{
    // perpendicular Plane  Sum(V) * X = 0;
    double C[3] = {0, 0, 0};
    double V[3] = {0, 0, 0};
    for (unsigned j = 0; j < 3; ++j)
    {
        C[j] = V[j] = 0.;
        for (unsigned i = 0; i < n; ++i)
        {
            const FireFly &ff = fireflys[i];
            C[j] += ff.X[j];
            V[j] += ff.V[j];
        }
        C[j] /= n;
        V[j] /= n;
    }
    double vx = 0, vv = 0;
    for (unsigned j = 0; j < 3; ++j)
    {
        vx += V[j] * C[j];
        vv += V[j] * V[j];
    }
    tmin = (vv == 0. ? 0. : max(-vx / vv, 0.));
    double Xmin[3], dmin2 = 0;
    for (unsigned j = 0; j < 3; ++j)
    {
        Xmin[j] = C[j] + V[j]*tmin;
        dmin2 += Xmin[j]*Xmin[j];
    }
    dmin = sqrt(dmin2);
}

void CenterMass::print_solution(ostream &fo) const
{
    fo << " " << fixed << setprecision(8) << dmin << " " << tmin;
}

int main(int argc, char ** argv)
{
    const string dash("-");

    unsigned n_opts = 0;
    bool naive = false;

    if ((argc > 1) && (string(argv[1]) == "-naive"))
    {
        naive = true;
        n_opts = 1;
    }
    int ai_in = n_opts + 1;
    int ai_out = ai_in + 1;
    int ai_dbg = ai_out + 1;
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

    void (CenterMass::*psolve)() =
        (naive ? &CenterMass::solve_naive : &CenterMass::solve);

    ostream &fout = *pfo;
    for (unsigned ci = 0; ci < n_cases; ci++)
    {
        if (getenv("TELLG"))
        {
            cerr << "Case (ci+1)="<<(ci+1) << ", tellg="<<pfi->tellg() << "\n";
        }
        CenterMass problem(*pfi);
        (problem.*psolve)();
        fout << "Case #"<< ci+1 << ":";
        problem.print_solution(fout);
        fout << "\n";
        fout.flush();

    }

    if (pfi != &cin) { delete pfi; }
    if (pfo != &cout) { delete pfo; }
    return 0;
}
