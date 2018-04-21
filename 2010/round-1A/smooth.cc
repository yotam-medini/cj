// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com -- Created: 2013/April/20

#include <iostream>
#include <fstream>
#include <string>
// #include <set>
// #include <map>
#include <vector>
#include <utility>

#include <cstdlib>
// #include <gmpxx.h>

using namespace std;

// typedef mpz_class mpzc_t;
typedef unsigned long u_t;
typedef unsigned long ul_t;
typedef unsigned long long ull_t;
// typedef vector<ul_t> vul_t;
// typedef vector<u_t> vu_t;
typedef vector<int> vi_t;

static unsigned dbg_flags;

class Smooth
{
 public:
    Smooth(istream& fi);
    void solve_naive();
    void solve();
    void print_solution(ostream&) const;
 private:
    u_t sub_solve_naive(const vi_t &sub, u_t depth);
    u_t sub_solve_2(int v0, int v1);
    u_t sub_solve_3(int v0, int v1, int v2);
    u_t sub_solve_naive_insertion(const vi_t &cur, u_t ci, u_t depth);
    bool is_smooth(const vi_t &sub) const;
    u_t D, I, N;
    int M;
    vi_t a;
    u_t solution;
};

Smooth::Smooth(istream& fi) : solution(0)
{
    fi >> D >> I >> M >> N;
    a = vi_t(vi_t::size_type(N), u_t(0));
    for (u_t i = 0; i < N; ++i)
    {
        fi >> a[i];
    }
}

void Smooth::solve_naive()
{
    // solution = sub_solve_naive(a, 0);
    solution = is_smooth(a) ? 0 : u_t(-1);
    if (solution != 0)
    {
        switch (a.size())
        {
         case 1:
            solution = 0;
            break;
         case 2:
            solution = sub_solve_2(a[0], a[1]);
            break;
         case 3:
            solution = sub_solve_3(a[0], a[1], a[2]);
            break;
         default:
            solution = u_t(-1);
        }
    }
}

void Smooth::solve()
{
    solve_naive();
}

bool Smooth::is_smooth(const vi_t &sub) const
{
    bool ret = true;
    vi_t::const_iterator i = sub.begin(), inext = i, e = sub.end();
    ++inext;
    for (; ret && (inext != e); i = inext++)
    {
        int v = *i;
        int vnext = *inext;
        ret = abs(vnext - v) <= M;
    }
    return ret;
}

u_t Smooth::sub_solve_2(int v0, int v1)
{
    u_t ret = 0;
    int delta = v1 - v0;
    int adelta = abs(delta);
    if (abs(delta) > M)
    {
        int d_cost = D;
        int i_cost = I*((adelta - 1)/M);
        int c_cost = adelta - M;
        ret = min(min(d_cost, i_cost), c_cost);
    }
    return ret;
}

static void minby(u_t &v, u_t by)
{
    if (v > by)
    {
        v = by;
    }
}

u_t Smooth::sub_solve_3(int v0, int v1, int v2)
{
    // assuming original is not smooth
    u_t ret = 0, subret = 0;

    int delta01 = v1 - v0;
    int adelta01 = abs(delta01);
    int delta12 = v2 - v1;
    int adelta12 = abs(delta12);
    if ((adelta01 > M) || (adelta12 > M))
    {
        u_t sub01 = sub_solve_2(v0, v1);
        u_t sub02 = sub_solve_2(v0, v2);
        u_t sub12 = sub_solve_2(v1, v2);
        u_t d_cost = D + min(min(sub01, sub02), sub12);
        ret = d_cost;

        if ((adelta01 > M) && (adelta12 > M))
        {
            int min_01 = min(v0, v1);
            int min_012 = min(min_01, v2);
            int max_012 = max(max(v0, v1), v2);
            for (int c = min_012; c <= max_012; ++c)
            {
                subret = abs(c - v0) + sub_solve_2(v0, v1) + sub12;
                minby(ret, subret);
                subret = abs(c - v1) + 
                    sub_solve_2(v0, c) + sub_solve_2(c, v2);
                minby(ret, subret);
                subret = abs(c - v2) + sub01 + sub_solve_2(v1, c);
                minby(ret, subret);
            }
            u_t icost = I*((adelta01 - 1)/M + (adelta12 - 1)/M);
            minby(ret, icost);
        }
        else if (adelta01 > M)
        {
            u_t icost = I*((adelta01 - 1)/M);
            minby(ret, icost);
            minby(ret, adelta01 - M);
        }
        else // adelta12 > M
        {
            u_t icost = I*((adelta12 - 1)/M);
            minby(ret, icost);
            minby(ret, adelta12 - M);
        }
    }
    return ret;
}

u_t Smooth::sub_solve_naive(const vi_t &cur, u_t depth)
{
    u_t ret = 0;
    if (dbg_flags & 0x1) {
       for (unsigned i = 0; i < depth; ++i) { cerr << ' '; }
       for (int v: cur) { cerr << ' ' << v; } cerr << '\n'; }
    if (!is_smooth(cur))
    {
        ret = u_t(-1); // infinity
        const u_t n = cur.size();
        vi_t sub;
        for (unsigned i = 0; i < n; ++i)
        {
            const vi_t::const_iterator curi = cur.begin() + i;
            const int vcuri = *curi;
            bool jump_pre = (i > 0) && (abs(vcuri - cur[i - 1]) > M);
            bool jump_aft = (i + 1 < n) && (abs(cur[i + 1] - vcuri) > M);
            if (jump_pre || jump_aft)
            {
                // Try deletion
                sub.clear();
                sub.insert(sub.end(), cur.begin(), curi);
                sub.insert(sub.end(), curi + 1, cur.end());
                u_t subret = sub_solve_naive(sub, depth + 1);
                if (ret > subret + D)
                {
                    ret = subret + D;
                }

                if ((i > 0) && ((i + 1) < n) && (jump_pre || (i + 1 == n)))
                {
                    subret = sub_solve_naive_insertion(cur, i, depth);
                    if (ret > subret)
                    {
                        ret = subret;
                    }
                }

                // Try change
                int nbr_pre = (i == 0 ? vcuri : cur[i - 1]);
                int nbr_aft = (i + 1 < n ? cur[i + 1] : vcuri);
                int newval;
                if (i == 0)
                {
                    newval = nbr_aft - (vcuri < nbr_aft ?  M : -M);
                }
                else if (i  == n - 1)
                {
                    newval = nbr_pre + (nbr_pre < vcuri ? M : -M);
                }
                else
                {
                    // int cdelta = nbr_aft - nbr_pre;
                    // newval = nbr_pre + ((cdelta/M)/2)*M;
                    newval = (nbr_pre + nbr_aft)/2;
                }
                if (newval != vcuri)
                {
                    sub = cur;
                    sub[i] = newval;
                    subret = sub_solve_naive(sub, depth + 1);
                    ul_t cost = abs(newval - vcuri);
                    if (ret > subret + cost)
                    {
                        ret = subret + cost;
                    }
                }
            }
        }
    }
    return ret;
}

u_t Smooth::sub_solve_naive_insertion(const vi_t &cur, u_t ci, u_t depth)
{
    u_t ins = 0;
    vi_t::const_iterator curi = cur.begin() + ci;
    int vpre = cur[ci - 1];
    int vcur = cur[ci];
    int vper = cur[ci + 1];
    vi_t sub;
    sub.insert(sub.end(), cur.begin(), curi);
    for (int step = (vpre < vcur ? M : -M), v = vpre + step;
         abs(vcur - v) >= M; v += step, ++ins)
    {
        sub.push_back(v);
    }
    sub.push_back(vcur);
    for (int step = (vcur < vper ? M : -M), v = vpre + step;
         abs(vper - v) >= M; v += step, ++ins)
    {
        sub.push_back(v);
    }
    sub.insert(sub.end(), curi + 1, cur.end());
    u_t cost = I * ins;
    u_t subet = sub_solve_naive(sub, depth + 1);
    return subet + cost;
}

void Smooth::print_solution(ostream &fo) const
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

    void (Smooth::*psolve)() =
        (naive ? &Smooth::solve_naive : &Smooth::solve);
    ostream &fout = *pfo;
    ul_t fpos_last = pfi->tellg();
    for (unsigned ci = 0; ci < n_cases; ci++)
    {
        Smooth smooth(*pfi);
        getline(*pfi, ignore);
        if (tellg)
        {
            ul_t fpos = pfi->tellg();
            cerr << "Case (ci+1)="<<(ci+1) << ", tellg=[" <<
                fpos_last << " " << fpos << ") size=" <<
                (fpos - fpos_last) << "\n";
            fpos_last = fpos;
        }

        (smooth.*psolve)();
        fout << "Case #"<< ci+1 << ":";
        smooth.print_solution(fout);
        fout << "\n";
        fout.flush();
    }

    if (pfi != &cin) { delete pfi; }
    if (pfo != &cout) { delete pfo; }
    return 0;
}
