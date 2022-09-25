// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com --

#include <algorithm>
#include <fstream>
#include <iostream>
#include <string>
#include <utility>
#include <vector>

#include <cstdlib>

using namespace std;

typedef unsigned u_t;
typedef unsigned long ul_t;
typedef unsigned long long ull_t;
typedef vector<u_t> vu_t;

static unsigned dbg_flags;

class FabricData
{
 public:
    FabricData() : D(0), U(0) {}
    string C;
    u_t D;
    u_t U;
};
typedef vector<FabricData> vfd_t;

class Fabric
{
 public:
    Fabric(istream& fi);
    Fabric(const vu_t&) {}; // TBD for test_case
    void solve_naive();
    void solve();
    void print_solution(ostream&) const;
    ull_t get_solution() const { return 0; }
 private:
    u_t N;
    vfd_t fabrics;
    u_t solution;
};

Fabric::Fabric(istream& fi) : solution(0)
{
    fi >> N;
    fabrics.reserve(N);
    while (fabrics.size() < N)
    {
        FabricData fd;
        fi >> fd.C >> fd.D >> fd.U;
        fabrics.push_back(fd);
    }
}

void Fabric::solve_naive()
{
    vfd_t cfabrics(fabrics);
    sort(cfabrics.begin(), cfabrics.end(), 
        [](const FabricData& fd0, const FabricData& fd1) -> bool
        {
            return (fd0.C < fd1.C) || ((fd0.C == fd1.C) && (fd0.U < fd1.U));
        });
    vfd_t dfabrics(fabrics);
    sort(dfabrics.begin(), dfabrics.end(), 
        [](const FabricData& fd0, const FabricData& fd1) -> bool
        {
            return (fd0.D < fd1.D) || ((fd0.D == fd1.D) && (fd0.U < fd1.U));
        });
    for (u_t i = 0; i < N; ++i)
    {
        if (cfabrics[i].U == dfabrics[i].U)
        {
            ++solution;
        }
    }
}

void Fabric::solve()
{
    solve_naive();
}

void Fabric::print_solution(ostream &fo) const
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

    void (Fabric::*psolve)() =
        (naive ? &Fabric::solve_naive : &Fabric::solve);
    if (solve_ver == 1) { psolve = &Fabric::solve; } // solve1
    ostream &fout = *pfo;
    ul_t fpos_last = pfi->tellg();
    for (unsigned ci = 0; ci < n_cases; ci++)
    {
        Fabric fabric(*pfi);
        getline(*pfi, ignore);
        if (tellg)
        {
            ul_t fpos = pfi->tellg();
            cerr << "Case (ci+1)="<<(ci+1) << ", tellg=[" <<
                fpos_last << " " << fpos << ") size=" <<
                (fpos - fpos_last) << "\n";
            fpos_last = fpos;
        }

        (fabric.*psolve)();
        fout << "Case #"<< ci+1 << ":";
        fabric.print_solution(fout);
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

static int test_case(int argc, char ** argv)
{
    int rc = rand_range(0, 1);
    ull_t solution(-1), solution_naive(-1);
    bool small = rc == 0;
    if (small)
    {
        Fabric p{vu_t()};
        p.solve_naive();
        solution_naive = p.get_solution();
    }
    {
        Fabric p{vu_t()};
        p.solve();
        solution = p.get_solution();
    }
    if (small && (solution != solution_naive))
    {
        rc = 1;
        cerr << "Failed: solution="<<solution << " != " <<
            solution_naive << " = solution_naive\n";
        ofstream f("fabric-fail.in");
        f << "1\n";
        f.close();
    }
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
