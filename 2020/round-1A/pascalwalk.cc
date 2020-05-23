// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com --

#include <algorithm>
#include <array>
#include <fstream>
#include <iostream>
#include <set>
#include <string>
#include <utility>
#include <vector>

#include <cstdlib>

using namespace std;

typedef unsigned u_t;
typedef unsigned long ul_t;
typedef unsigned long long ull_t;
typedef vector<ull_t> vull_t;
typedef vector<vull_t> vvul_t;
typedef array<u_t, 2> au2_t;
typedef vector<au2_t> vau2_t;
typedef set<au2_t> setu2_t;

static unsigned dbg_flags;

class PascalWalk
{
 public:
    PascalWalk(istream& fi);
    void solve_naive();
    void solve();
    void print_solution(ostream&) const;
 private:
    enum {MAX_STEPS = 500};
    static vvul_t triangle;
    static void init_triangle();
    bool adv();
    void if_unsued_push_back(vau2_t& adjs, const au2_t& pt);
    void path_push(const au2_t& pt);
    void path_pop();
    ull_t N;
    vau2_t solution;
    ull_t path_val;
    vau2_t path;
    setu2_t used;;
};
vvul_t PascalWalk::triangle;

void PascalWalk::init_triangle()
{
    if (triangle.empty())
    {
         triangle.reserve(MAX_STEPS + 1);
         vull_t row;
         row.reserve(MAX_STEPS + 1);
         row.push_back(1);
         triangle.push_back(row);
         while (triangle.size() <= MAX_STEPS)
         {
             const vull_t& prev = triangle.back();
             size_t sz = prev.size() + 1;
             row.clear();
             row.push_back(1);
             for (u_t k = 1; k < sz - 1; ++k)
             {
                 ull_t val = prev[k - 1] + prev[k];
                 row.push_back(val);
             }
             row.push_back(1);
             triangle.push_back(row);
         }
    }
}

PascalWalk::PascalWalk(istream& fi)
{
    init_triangle();
    fi >> N;
    // copy_n(istream_iterator<u_t>(fi), N, back_inserter(a));
}

void PascalWalk::solve_naive()
{
    path_val = 0;
    path_push(au2_t{0, 0});
    if (path_val != N)
    {
        adv();
    }
    solution = path;
}

bool PascalWalk::adv()
{
    bool found = (path_val == N);
    if ((!found) && (path.size() < MAX_STEPS) && (path_val < N))
    {
        const au2_t& pt = path.back();
        vau2_t adjs;
        if (pt[0] > 1)
        {
            u_t row = pt[0] - 1;
            if (pt[1] <= row)
            {
                if_unsued_push_back(adjs, au2_t{row, pt[1]});
            }
            if ((0 < pt[1]) && (pt[1] - 1 <= row))
            {
                if_unsued_push_back(adjs, au2_t{row, pt[1] - 1});
            }
        }
        if (pt[1] > 0)
        {
            if_unsued_push_back(adjs, au2_t{pt[0], pt[1] - 1});
        }
        if (pt[1] + 1 <= pt[0])
        {
            if_unsued_push_back(adjs, au2_t{pt[0], pt[1] + 1});
        }
        if_unsued_push_back(adjs, au2_t{pt[0] + 1, pt[1]});
        if_unsued_push_back(adjs, au2_t{pt[0] + 1, pt[1] + 1});
        for (u_t ai = 0, ae = adjs.size(); (ai != ae) && !found; ++ai)
        {
            path_push(adjs[ai]);
            found = adv();
            if (found)
            {
                solution = path;
            }
            else
            {
                path_pop();
            }
        }
    }
    return found;
}

void PascalWalk::if_unsued_push_back(vau2_t& adjs, const au2_t& pt)
{
    if (used.find(pt) == used.end())
    {
        adjs.push_back(pt);
    }
}

void PascalWalk::path_push(const au2_t& pt)
{
    path.push_back(pt);
    used.insert(used.end(), pt);
    ull_t v = triangle[pt[0]][pt[1]];
    path_val += v;
}

void PascalWalk::path_pop()
{
    const au2_t& pt = path.back();
    used.erase(pt);
    ull_t v = triangle[pt[0]][pt[1]];
    path.pop_back();
    path_val -= v;
}

void PascalWalk::solve()
{
    // solve_naive();
    u_t nz = 0; // originla zero bite
    ull_t n = N;
    u_t n_rows = 0;
    while (n > 0)
    {
        nz += (n % 2 == 0 ? 1 : 0);
        n /= 2;
        ++n_rows;
    }

    ull_t target = N;
    if (nz > 0)
    {
        target = N - nz + 1;
        u_t nz1 = nz + 1;
        while (target + nz1 > N)
        {
            --target;
            n = target;
            nz1 = 0;
            n_rows = 0;
            while (n > 0)
            {
                nz1 += (n % 2 == 0 ? 1 : 0);
                n /= 2;
                ++n_rows;
            }
        }
    }
    ull_t pv = 1;
    solution.push_back(au2_t{0, 0});
    vau2_t row;
    row.reserve(32);
    for (u_t r = 1; r < n_rows; ++r)
    {
        ull_t rp2 = (1llu << r);
        bool left = (solution.back()[1] == 0);
        if ((target & rp2) != 0)
        {
            row.clear();
            for (u_t c = 0; c <= r; ++c)
            {
                row.push_back(au2_t{r, c});
            }
            if (!left)
            {
                reverse(row.begin(), row.end());
            }
            solution.insert(solution.end(), row.begin(), row.end());
            pv += rp2;
        }
        else
        {
            au2_t pt{r, left ? 0 : r};
            solution.push_back(pt);
            pv += 1;
        }
    }
    while (pv < N) // add left or righ diagonal segment
    {
        const au2_t& last = solution.back();
        au2_t pt{last[0] + 1, last[1] == 0 ? 0 : last[0] + 1};
        solution.push_back(pt);
        pv += 1;
    }
}

void PascalWalk::print_solution(ostream &fo) const
{
    for (const au2_t& pt: solution)
    {
        fo << '\n' << (pt[0] + 1) << ' ' << (pt[1] + 1);
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

    void (PascalWalk::*psolve)() =
        (naive ? &PascalWalk::solve_naive : &PascalWalk::solve);
    ostream &fout = *pfo;
    ul_t fpos_last = pfi->tellg();
    for (unsigned ci = 0; ci < n_cases; ci++)
    {
        PascalWalk pascalwalk(*pfi);
        getline(*pfi, ignore);
        if (tellg)
        {
            ul_t fpos = pfi->tellg();
            cerr << "Case (ci+1)="<<(ci+1) << ", tellg=[" <<
                fpos_last << " " << fpos << ") size=" <<
                (fpos - fpos_last) << "\n";
            fpos_last = fpos;
        }

        (pascalwalk.*psolve)();
        fout << "Case #"<< ci+1 << ":";
        pascalwalk.print_solution(fout);
        fout << "\n";
        fout.flush();
    }

    if (pfi != &cin) { delete pfi; }
    if (pfo != &cout) { delete pfo; }
    return 0;
}
