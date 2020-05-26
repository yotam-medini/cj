// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com --

#include <algorithm>
#include <fstream>
#include <iostream>
#include <iterator>
// #include <map>
// #include <set>
#include <array>
#include <string>
#include <utility>
#include <vector>

#include <cstdlib>

using namespace std;

typedef unsigned u_t;
typedef unsigned long ul_t;
typedef unsigned long long ull_t;
typedef array<u_t, 2> au2_t;
typedef array<int, 2> ai2_t;
typedef vector<au2_t> vau2_t;
typedef vector<u_t> vu_t;
typedef vector<vu_t> vvu_t;
typedef vector<bool> vb_t;
typedef vector<vb_t> vvb_t;

static unsigned dbg_flags;

class Square
{
 public:
   Square() : age(0), link{{0, 0}, {0, 0}}, weak_mark(false) {}
   u_t age;
   int link[2][2]; // row,col prev,next
   bool weak_mark;
};
typedef vector<Square> vsquare_t;
typedef vector<vsquare_t> vvsquare_t;

class SquareDance
{
 public:
    SquareDance(istream& fi);
    void solve_naive();
    void solve();
    void print_solution(ostream&) const;
 private:
    static const ai2_t dirs[4];
    ull_t sum_skills() const;
    void get_eliminate(vau2_t& weaks) const;
    void eliminate(const vau2_t& weaks);
    bool inside(const ai2_t& rc) const;
    bool is_weak(const au2_t& dancer) const;
    void init();
    void age_set(const vau2_t& weaks, u_t round);
    void weaks_next(vau2_t& weaks);
    bool is_link_weak(const au2_t& dancer) const;
    ull_t compute_interest(u_t round) const;
    u_t r, c;
    vvu_t skills;
    vvu_t curr_skills;
    ull_t solution;
    vvsquare_t squares;
};
const ai2_t SquareDance::dirs[4] = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}};

SquareDance::SquareDance(istream& fi) : solution(0)
{
    fi >> r >> c;
    skills.reserve(r);
    for (u_t ri = 0; ri < r; ++ri)
    {
        vu_t row;
        row.reserve(c);
        copy_n(istream_iterator<u_t>(fi), c, back_inserter(row));
        skills.push_back(row);
    }
}

void SquareDance::solve_naive()
{
    bool eliminating = true;
    curr_skills = skills;
    while (eliminating)
    {
        solution += sum_skills();
        vau2_t weaks;
        get_eliminate(weaks);
        eliminating = !weaks.empty();
        eliminate(weaks);
    }
}

ull_t SquareDance::sum_skills() const
{
    ull_t sum = 0;
    for (const vu_t& row: curr_skills)
    {
        for (int x: row)
        {
            sum += x;
        }
    }
    return sum;
}

void SquareDance::get_eliminate(vau2_t& weaks) const
{
    for (u_t ri = 0; ri < r; ++ri)
    {
        for (u_t ci = 0; ci < c; ++ci)
        {
            au2_t dancer{ri, ci};
            if (is_weak(dancer))
            {
                weaks.push_back(dancer);
            }
        }
    }
}

void SquareDance::eliminate(const vau2_t& weaks)
{
    for (const au2_t& w: weaks)
    {
        curr_skills[w[0]][w[1]] = 0;
    }
}

bool SquareDance::inside(const ai2_t& rc) const
{
    bool ret = (0 <= rc[0]) && (0 <= rc[1]) &&
        (rc[0] < int(r)) && (rc[1] < int(c));
    return ret;
}

bool SquareDance::is_weak(const au2_t& dancer) const
{
    const int v = curr_skills[dancer[0]][dancer[1]];
    int sum_compass = 0, n_commpass = 0;
    for (u_t d = (v != 0 ? 0 : 4); d < 4; ++d)
    {
        const ai2_t& dir = dirs[d];
        int av = 0;
        ai2_t a{int(dancer[0]) + dir[0], int(dancer[1]) + dir[1]};
        while (inside(a) && (av == 0))
        {
            av = curr_skills[a[0]][a[1]];
            a[0] += dir[0];
            a[1] += dir[1];
        }
        if (av > 0)
        {
            ++n_commpass;
            sum_compass += av;
        }
    }
    bool ret = (n_commpass * v < sum_compass);
    return ret;
}

void SquareDance::solve()
{
    curr_skills = skills;
    init();
    vau2_t weaks;
    get_eliminate(weaks);
    u_t round = 1;
    while (!weaks.empty())
    {
        if (((dbg_flags & 0x1) && ((round & (round - 1)) == 0)) ||
            (dbg_flags & 0x2))
        {
            cerr << "round: " << round << ", #(weaks)=" << weaks.size() << '\n';
        }
        age_set(weaks, round);
        weaks_next(weaks);
        ++round;
    }
    solution = compute_interest(round);
}

void SquareDance::init()
{
    squares.reserve(r);
    vsquare_t square_row(c, Square());
    for (int ci = 0; ci < int(c); ++ci)
    {
        square_row[ci].link[1][0] = ci - 1;
        square_row[ci].link[1][1] = ci + 1;
    }
    for (int ri = 0; ri < int(r); ++ri)
    {
        for (u_t ci = 0; ci < c; ++ci)
        {
            square_row[ci].link[0][0] = ri - 1;
            square_row[ci].link[0][1] = ri + 1;
        }
        squares.push_back(square_row);
    }
}

void SquareDance::age_set(const vau2_t& weaks, u_t round)
{
    for (const au2_t w: weaks)
    {
        curr_skills[w[0]][w[1]] = 0;
        squares[w[0]][w[1]].age = round;
    }
}

void SquareDance::weaks_next(vau2_t& weaks)
{
    for (const au2_t w: weaks)
    {
        Square& square = squares[w[0]][w[1]];
        square.weak_mark = true;
        for (u_t dim = 0; dim != 2; ++dim)
        {
            int prev = square.link[dim][0];
            int next = square.link[dim][1];
            au2_t aprev(w), anext(w);
            aprev[dim] = u_t(prev);
            anext[dim] = u_t(next);
            if (prev >= 0)
            {
                squares[aprev[0]][aprev[1]].link[dim][1] = next;
            }
            if (next < int(dim == 0 ? r : c))
            {
                squares[anext[0]][anext[1]].link[dim][0] = prev;
            }
        }
    }
    vau2_t weaks_new;
    for (const au2_t w: weaks)
    {
        const Square& square = squares[w[0]][w[1]];
        for (u_t dim = 0; dim != 2; ++dim)
        {
            int prev = square.link[dim][0];
            int next = square.link[dim][1];
            au2_t aprev(w), anext(w);
            aprev[dim] = u_t(prev);
            anext[dim] = u_t(next);
            if ((prev >= 0) && is_link_weak(aprev))
            {
                weaks_new.push_back(aprev);
                squares[aprev[0]][aprev[1]].weak_mark = true;
            }
            if (next < int(dim == 0 ? r : c) && is_link_weak(anext))
            {
                weaks_new.push_back(anext);
                squares[anext[0]][anext[1]].weak_mark = true;
            }
        }
    }
    swap(weaks, weaks_new);
}

bool SquareDance::is_link_weak(const au2_t& dancer) const
{
    bool ret = false;
    const Square& square = squares[dancer[0]][dancer[1]];
    if (!square.weak_mark)
    {
        const int v = curr_skills[dancer[0]][dancer[1]];
        int sum_compass = 0, n_commpass = 0;
        for (u_t dim = 0; dim != 2; ++dim)
        {
            int prev = square.link[dim][0];
            int next = square.link[dim][1];
            au2_t aprev(dancer), anext(dancer);
            aprev[dim] = u_t(prev);
            anext[dim] = u_t(next);
            if (prev >= 0)
            {
                u_t av = curr_skills[aprev[0]][aprev[1]];
                ++n_commpass;
                sum_compass += av;
            }
            if (next < int(dim == 0 ? r : c))
            {
                u_t av = curr_skills[anext[0]][anext[1]];
                ++n_commpass;
                sum_compass += av;
            }
        }
        ret = (n_commpass * v < sum_compass);
    }
    return ret;
}

ull_t SquareDance::compute_interest(u_t round) const
{
    ull_t interest = 0;
    for (u_t ri = 0; ri < r; ++ri)
    {
        for (u_t ci = 0; ci < c; ++ci)
        {
            u_t dage = squares[ri][ci].age;
            if (dage == 0)
            {
                dage = round; // survivor
            }
            interest += dage * skills[ri][ci];
        }
    }
    return interest;
}

void SquareDance::print_solution(ostream &fo) const
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

    void (SquareDance::*psolve)() =
        (naive ? &SquareDance::solve_naive : &SquareDance::solve);
    ostream &fout = *pfo;
    ul_t fpos_last = pfi->tellg();
    for (unsigned ci = 0; ci < n_cases; ci++)
    {
        SquareDance sqdance(*pfi);
        getline(*pfi, ignore);
        if (tellg)
        {
            ul_t fpos = pfi->tellg();
            cerr << "Case (ci+1)="<<(ci+1) << ", tellg=[" <<
                fpos_last << " " << fpos << ") size=" <<
                (fpos - fpos_last) << "\n";
            fpos_last = fpos;
        }

        (sqdance.*psolve)();
        fout << "Case #"<< ci+1 << ":";
        sqdance.print_solution(fout);
        fout << "\n";
        fout.flush();
    }

    if (pfi != &cin) { delete pfi; }
    if (pfo != &cout) { delete pfo; }
    return 0;
}
