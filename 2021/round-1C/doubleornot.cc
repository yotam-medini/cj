// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com --

#include <algorithm>
#include <fstream>
#include <iostream>
#include <array>
#include <map>
#include <set>
#include <string>
#include <utility>
#include <vector>

#include <cstdlib>

using namespace std;

typedef unsigned u_t;
typedef unsigned long ul_t;
typedef unsigned long long ull_t;
typedef vector<bool> vb_t;
typedef array<u_t, 2> au2_t;
typedef map<au2_t, vb_t> u2vb_t;
typedef map<vb_t, vb_t> vb2vb_t;
typedef set<vb_t> setvb_t;

static unsigned dbg_flags;

static string vb2str(const vb_t& vb)
{
    string s;
    for (vb_t::const_reverse_iterator i = vb.rbegin(), e = vb.rend();
        i != e; ++i)
    {
        s.push_back(*i ? '1' : '0');
    }
    if (s.empty())
    {
        s.push_back('0');
    }
    return s;
}

class DoubleOrNot
{
 public:
    DoubleOrNot(istream& fi);
    DoubleOrNot(const vb_t& _S, const vb_t& _E) : S(_S), E(_E), solution(-1) {}
    void solve_naive();
    void solve();
    void print_solution(ostream&) const;
    int get_solution() const { return solution; }
 private:
    void s2vb(vb_t& vb, const string& s) const;
    void nots_then_doubles();
    void build(u_t n_doubles, bool pre_not);
    void vb_double(vb_t& r, const vb_t& a, size_t n=1) const;
    void vb_not(vb_t& r, const vb_t& a) const;
    void vb_self_double(vb_t& r, size_t n=1) const
    {
        vb_t t;
        vb_double(t, r, n);
        swap(r, t);
    }
    void vb_self_not(vb_t& r) const
    {
        vb_t t;
        vb_not(t, r);
        swap(r, t);
    }
    void vb_just_not(vb_t& r, const vb_t& a) const;
    void improve(u_t steps)
    {
        if ((solution == -1) || (solution > int(steps)))
        {
            solution = steps;
        }
    }
    void show_path(const vb2vb_t& p) const;
    vb_t S, E;
    int solution;
};

DoubleOrNot::DoubleOrNot(istream& fi) : solution(-1)
{
    string s;
    fi >> s;
    s2vb(S, s);
    fi >> s;
    s2vb(E, s);
}

void DoubleOrNot::solve_naive()
{
    u_t bump = 0;
    setvb_t done;
    u2vb_t q;
    vb2vb_t p;
    q.insert(q.end(), u2vb_t::value_type(au2_t{u_t(0), bump++}, S));
    done.insert(done.end(), S);
    while ((!q.empty()) && (solution == -1))
    {
        const u2vb_t::value_type& vmin = *(q.begin());
        const u_t d = vmin.first[0];
        if (dbg_flags & 0x1) {
            cerr << "d="<<d << " -> " << vb2str(vmin.second) << '\n'; }
        if (vmin.second == E)
        {
            solution = d;
            if (dbg_flags & 0x2) { show_path(p); }
        }
        else
        {
            const vb_t node(vmin.second);
            vb_t adjs[2];
            vb_double(adjs[0], vmin.second);
            vb_not(adjs[1], vmin.second);
            q.erase(q.begin()); // pop
            for (u_t i: {0, 1})
            {
                const vb_t& adj = adjs[i];
                if (adj.size() < 16)
                {
                    setvb_t::iterator iter = done.find(adj);
                    if (iter == done.end())
                    {
                        au2_t key{d + 1, bump++};
                        q.insert(q.end(), u2vb_t::value_type(key, adj));
                        done.insert(iter, adj);
                        p.insert(p.end(), vb2vb_t::value_type(adj, node));
                    }
                }
            }
        }
    }
}

void DoubleOrNot::solve()
{
    nots_then_doubles();
    const u_t e_sz = E.size();
    for (u_t n_doubles = 1; n_doubles <= e_sz; ++n_doubles)
    {
        build(n_doubles, false);
        build(n_doubles, true);
    }
}

void DoubleOrNot::build(u_t n_doubles, bool pre_not)
{
    const u_t e_sz = E.size();
    u_t steps = 0;
    vb_t curr(S);
    if (pre_not)
    {
        vb_self_not(curr);
        ++steps;
    }
    for (u_t pending = n_doubles; pending > 0; --pending)
    {
        bool do_not = (pending < n_doubles) &&
            (pending < e_sz) && (E[pending - 1] != E[pending]);
        if (do_not)
        {
            vb_self_not(curr);
            ++steps;
        }
        vb_self_double(curr);
        ++steps;
    }
    while (curr.size() > e_sz)
    {
        vb_self_not(curr);
        ++steps;
    }
    if (curr == E)
    {
        improve(steps);
    }
}


void DoubleOrNot::nots_then_doubles()
{
    u_t steps = 0;
    vb_t curr(S);
    while ((curr.size() > 1) && (curr != E))
    {
        vb_self_not(curr);
        ++steps;
    }
    if ((E.size() == 1) && (E != curr))
    {
        curr = E;
        ++steps;
    }
    else if (curr.back()) // not "0"
    {
        while (curr.size() < E.size())
        {
            vb_self_double(curr);
            ++steps;
        }
    }
    if (curr == E)
    {
        solution = steps;
    }
}

void DoubleOrNot::print_solution(ostream &fo) const
{
    if (solution >= 0)
    {
        fo << ' ' << solution;
    }
    else
    {
        fo << " IMPOSSIBLE";
    }
}

void DoubleOrNot::s2vb(vb_t& vb, const string& s) const
{
    vb.reserve(s.size());
    for (string::const_reverse_iterator i = s.rbegin(), e = s.rend();
        i != e; ++i)
    {
        const char c = *i;
        vb.push_back(c != '0');
    }
}

void DoubleOrNot::vb_double(vb_t& r, const vb_t& a, size_t n) const
{
    r.reserve(a.size() + n);
    r = vb_t(n, false);
    r.insert(r.end(), a.begin(), a.end());
    if ((r.size() > 1) && !r.back())
    {
        r.pop_back();
    }
}

void DoubleOrNot::vb_not(vb_t& r, const vb_t& a) const
{
    vb_just_not(r, a);
    while ((r.size() > 1) && !(r.back()))
    {
        r.pop_back();
    }
}

void DoubleOrNot::vb_just_not(vb_t& r, const vb_t& a) const
{
    r.clear();
    r.reserve(a.size());
    for (bool b: a)
    {
        r.push_back(!b);
    }
}

void DoubleOrNot::show_path(const vb2vb_t& p) const
{
    vector<vb_t> path;
    path.push_back(E);
    while (path.back() != S)
    {
        vb2vb_t::const_iterator i = p.find(path.back());
        path.push_back(i->second);
    }
    for (vector<vb_t>::const_reverse_iterator
        i = path.rbegin(), e = path.rend(); i != e; ++i)
    {
        cerr << ' ' << vb2str(*i);
    }
    cerr << '\n';
}

static void u2vb(vb_t& vb, u_t u)
{
    vb.clear();
    while (u)
    {
        vb.push_back(u & 0x1 ? true : false);
        u >>= 1;
    }
    if (vb.empty())
    {
        vb.push_back(false);
    }
}

static int test(int argc, char **argv)
{
    int rc = 0;
    dbg_flags |= 0x2;
    u_t s0 (argc > 0 ? u_t(strtoul(argv[0], 0, 0)) : 0);
    u_t e0 (argc > 1 ? u_t(strtoul(argv[1], 0, 0)) : 0);
    for (u_t s = s0; s < 0x100; ++s)
    {
        vb_t vbs;
        u2vb(vbs, s);
        for (u_t e = e0; (rc == 0) && (e < 0x100); ++e)
        {
            vb_t vbe;
            u2vb(vbe, e);
            cerr << "S="<< vb2str(vbs) << " E="<< vb2str(vbe) << '\n';
            int solution_naive = -2, solution = -2;
            {
                DoubleOrNot  don(vbs, vbe);
                don.solve_naive();
                solution_naive = don.get_solution();
                don.print_solution(cerr); cerr << '\n';
            }
            {
                DoubleOrNot  don(vbs, vbe);
                don.solve();
                solution = don.get_solution();
            }
            if (solution != solution_naive)
            {
                cerr << "solution="<<solution << " != " <<
                    "solution_naive="<<solution_naive << '\n';
                rc = 1;
            }
        }
    }
    return rc;
}

int main(int argc, char ** argv)
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
        else if (opt == string("-test"))
        {
            rc = test(argc - (ai + 1), argv + (ai + 1));
            exit(rc);
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

    void (DoubleOrNot::*psolve)() =
        (naive ? &DoubleOrNot::solve_naive : &DoubleOrNot::solve);
    if (solve_ver == 1) { psolve = &DoubleOrNot::solve; } // solve1
    ostream &fout = *pfo;
    ul_t fpos_last = pfi->tellg();
    for (unsigned ci = 0; ci < n_cases; ci++)
    {
        DoubleOrNot doubleOrNot(*pfi);
        getline(*pfi, ignore);
        if (tellg)
        {
            ul_t fpos = pfi->tellg();
            cerr << "Case (ci+1)="<<(ci+1) << ", tellg=[" <<
                fpos_last << " " << fpos << ") size=" <<
                (fpos - fpos_last) << "\n";
            fpos_last = fpos;
        }

        (doubleOrNot.*psolve)();
        fout << "Case #"<< ci+1 << ":";
        doubleOrNot.print_solution(fout);
        fout << "\n";
        fout.flush();
    }

    if (pfi != &cin) { delete pfi; }
    if (pfo != &cout) { delete pfo; }
    return 0;
}
