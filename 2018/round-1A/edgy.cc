// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com -- Created: 2013/April/20

#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <set>
#include <vector>
#include <utility>
#include <algorithm>

#include <cstdlib>
#include <cmath>

using namespace std;

typedef unsigned long ul_t;
typedef unsigned long long ull_t;

typedef pair<double, double> dd_t;
typedef set<dd_t> set_dd_t;
typedef vector<dd_t> vdd_t;

class Cookie
{
 public:
    Cookie(double _w=0, double _h=0) : 
        w(_w), h(_h),
        perimeter(2*(_w + _h)),
        cut_cover(2*(min(_w, _h)), 2*(sqrt(_w*_w + _h*_h)))
    {
    }
    double w, h;
    double perimeter;
    dd_t cut_cover;
};
typedef vector<Cookie> vcookie_t;

static unsigned dbg_flags;

class Edgy
{
 public:
    Edgy(istream& fi);
    void solve_naive();
    void solve(bool naive);
    void print_solution(ostream&) const;
 private:
    void add_unite(const dd_t &interval);
    void solve_in_covered();
    unsigned n;
    double p;
    vcookie_t cookies;
    set_dd_t covered;
    double solution;
};

Edgy::Edgy(istream& fi) : solution(0)
{
    fi >> n >> p;
    cookies.reserve(n);
    for (unsigned i = 0; i < n; ++i)
    {
        double w, h;
        fi >> w >> h;
        cookies.push_back(Cookie(w, h));
    }
}

void Edgy::solve_naive()
{
    const Cookie &c0 = cookies[0];
    double perimeters = n * c0.perimeter;
    double extra = p - perimeters;
    solution = perimeters;
    double cut_min = c0.cut_cover.first;
    double cut_max = c0.cut_cover.second;
    if (extra >= cut_min)
    {
        double n_cut_max = n*cut_max;
        if (extra >= n_cut_max)
        {
            solution = perimeters + n_cut_max;
        }
        else
        {
            //  q*cut_max >= (q+1)*cut_min
            //  q(cut_max-cut_min) >= cut_min
            //  q >= cut_min / (cut_max - cut_min)
            unsigned q = cut_min / (cut_max - cut_min);
            if (extra >= q*cut_max)
            {
                solution = p;
            }
            else
            {
               unsigned qmin = extra / cut_min; // >= 1
               if (extra <= qmin * cut_max)
               {
                   solution = perimeters;
               }
               else
               {
                   solution = perimeters + (qmin - 1)*cut_max;
               }
            }
        }
    }
}

void Edgy::solve(bool naive)
{
    bool eq_cookies = true;
    const Cookie &c0 = cookies[0];
    for (unsigned i = 1; eq_cookies && (i < n); ++i)
    {
        const Cookie &c = cookies[i];
        eq_cookies = (c.w == c0.w) && (c.h == c0.h);
    }
    if (eq_cookies && naive)
    {
        solve_naive();
    }
    else
    {
        // cerr << "Naive solution impossible\n";
        vdd_t intervals;
        for (const Cookie &cookie: cookies)
        {
            intervals.push_back(cookie.cut_cover);
        }
        sort(intervals.begin(), intervals.end());
        for (const dd_t &interval: intervals)
        {
            add_unite(interval);
        }
        solve_in_covered();
    }
}

void Edgy::add_unite(const dd_t &new_interval)
{
    vdd_t add_intervals;
    add_intervals.reserve(covered.size());
    for (set_dd_t::const_iterator i = covered.begin(), e = covered.end();
        i != e; )
    {
        const dd_t &interval = *i;
        ++i;
        dd_t a_interval(
            interval.first + new_interval.first,
            interval.second + new_interval.second);
        add_intervals.push_back(a_interval);
    }
    covered.insert(covered.end(), new_interval);
    covered.insert(add_intervals.begin(), add_intervals.end());
    set_dd_t::iterator i = covered.begin(), inext;
    set_dd_t::iterator e = covered.end();
    set_dd_t::iterator ub_i = i;
    // ++i;
    // bool unite = trye;
    add_intervals.clear();
    for (; i != e; i = inext)
    {
        const dd_t &prev_interval = *i;
        inext = i;
        ++inext;
        if ((inext == e) || (prev_interval.second < inext->first))
        {
            if (ub_i != i)
            {
                dd_t u_interval(ub_i->first, i->second);
                covered.erase(ub_i, inext);
                // covered.insert(inext, u_interval);
                add_intervals.push_back(u_interval);
            }
            ub_i = inext;
        }
    }
    covered.insert(add_intervals.begin(), add_intervals.end());
}

void Edgy::solve_in_covered()
{
    double perimeters = 0;
    for (unsigned i = 0; (i < n); ++i)
    {
        const Cookie &c = cookies[i];
        perimeters += c.perimeter;
    }
    solution = perimeters;
    double extra = p - perimeters;
    bool exact = false;
    for (set_dd_t::const_iterator i = covered.begin(), e = covered.end();
        (i != e) && !exact; ++i)
    {
        const dd_t &interval = *i;
        exact = (interval.first <= extra) && (extra <= interval.second);
        if (exact)
        {
            solution = p;
        }
        else if (interval.second <= extra)
        {
            solution = perimeters + interval.second;
        }
    }
}

void Edgy::print_solution(ostream &fo) const
{
    fo << " " << fixed << setprecision(6) << solution;
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

    ostream &fout = *pfo;
    ul_t fpos_last = pfi->tellg();
    for (unsigned ci = 0; ci < n_cases; ci++)
    {
        Edgy edgy(*pfi);
        getline(*pfi, ignore);
        if (tellg)
        {
            ul_t fpos = pfi->tellg();
            cerr << "Case (ci+1)="<<(ci+1) << ", tellg=[" <<
                fpos_last << " " << fpos << ") size=" <<
                (fpos - fpos_last) << "\n";
            fpos_last = fpos;
        }

        edgy.solve(naive);
        fout << "Case #"<< ci+1 << ":";
        edgy.print_solution(fout);
        fout << "\n";
        fout.flush();
    }

    if (pfi != &cin) { delete pfi; }
    if (pfo != &cout) { delete pfo; }
    return 0;
}