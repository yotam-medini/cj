// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com --

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

class Moons
{
 public:
    Moons(istream& fi);
    void solve_naive();
    void solve();
    void print_solution(ostream&) const;
 private:
    static char cj_flip(char c)
    {
        return (c == 'C' ? 'J' : 'C');
    }
    int pay(const string& s) const;
    int pay(const string& s, int cj_cost, int jc_cost) const;
    void solve_xy_neg();
    void solve_cj_neg(const string& sinv, int cj_cost, int jc_cost);
    int X, Y;
    string S;
    int solution;
};

Moons::Moons(istream& fi) : solution(0)
{
    fi >> X >> Y >> S;
}

void Moons::solve_naive()
{
    const u_t len = S.size();
    vu_t qis;
    for (u_t i = 0; i < len; ++i)
    {
        if (S[i] == '?')
        {
            qis.push_back(i);
        }
    }
    if (qis.empty())
    {
         solution = pay(S);
    }
    else
    {
        const u_t nq = qis.size();
        solution = len * (abs(X) < abs(Y) ? abs(Y) : abs(X));
        string s(S);
        for (u_t jmask = 0; jmask < (1u << nq); ++jmask)
        {
            for (u_t j = 0; j < nq; ++j)
            {
                s[qis[j]] = ((1u << j) & jmask ? 'J' : 'C');
            }
            int cpay = pay(s);
            if (solution > cpay)
            {
                solution = cpay;
            }
        }
    }
}

void Moons::solve()
{
    if ((X >= 0) && (Y >= 0))
    {
        string s;
        for (char c: S)
        {
            if (c != '?')
            {
                s.push_back(c);
            }
        }
        solution = pay(s);
    }
    else if ((X <= 0) && (Y <= 0))
    {
        solve_xy_neg();
    }
    else
    {
        if (X < 0)
        {
            solve_cj_neg(S, X, Y);
        }
        else
        {
            string sinv(S);
            for (u_t ci = 0, len = sinv.size(); ci < len; ++ci)
            {
                char c = sinv[ci];
                if (c != '?')
                {
                    sinv[ci] = cj_flip(c);
                }
            }
            solve_cj_neg(sinv, Y, X);
        }
    }
}

void Moons::solve_xy_neg()
{
    const size_t len = S.size();
    size_t i = 0;
    for (; (i < len) && (S[i] == '?'); ++i) {}
    if (i == len)
    {
        const u_t n_pairs = len - 1;
        const u_t half = n_pairs/2;
        const u_t half1 = n_pairs - half;
        if (X < Y)
        {
            solution = half * Y + half1 * X;
        }
        else
        {
            solution = half * X + half1 * Y;
        }
    }
    else // not all ?
    {
        string s(S);
        char last = S[i];
        for (int k = i - 1; k >= 0; --k)
        {
            char c = cj_flip(last);
            s[k] = last = c;
        }
        last = s[i];
        while (i < len)
        {
            for (; (i < len) && (s[i] == '?'); ++i)
            {
                char c = cj_flip(last);
                s[i] = last = c;
            }
            for (; (i < len) && (s[i] != '?'); ++i)
            {
                last = s[i];
            }
        }
        solution = pay(s);
    }
}

void Moons::solve_cj_neg(const string& salt, int cj_cost, int jc_cost)
{
    const size_t len = salt.size();
    if (len > 1)
    {
        string build(salt);
        size_t i = 0;
        for (; (i < len) && (salt[i] == '?'); ++i) {}
        if (i == len)
        {
            if ((-cj_cost) < jc_cost)
            {
                /// CJJJJJJJJJ  || CCCCCCCCCJ
                build = string(size_t(len), 'C');
                build[len - 1] = 'J';
            } 
            else // CJCJjjj ,,,,
            {
                for (u_t ci = 0; ci + 1 < len; ++ci)
                {
                    build[ci] = "CJ"[ci % 2];
                }
                build[len - 1] = 'J';
            }
        }
        else
        {
            if ((-cj_cost) >= jc_cost)
            {
                //  CJCJCJ ... 
                for (u_t ci = 0; ci < i; ++ci)
                {
                    build[ci] = "CJ"[ci % 2];
                }
            }
            else
            {
                for (u_t ci = 0; ci < i; ++ci)
                {
                    build[ci] = 'C';
                }
            }
            while (i < len)
            {
                for (; (i < len) && (salt[i] != '?'); ++i) {}
                const size_t ib = i;
                for (; (i < len) && (salt[i] == '?'); ++i) {}
                const size_t ie = i;
                const u_t nq = ie - ib;
                if (nq > 0)
                {
                    if (ie < len) // inner segments
                    {
                        if ((-cj_cost) >= jc_cost)
                        {
                            //if ((salt[ib] == salt[ie - 1]) || (salt[ib] =='C'))
                            for (u_t ci = ib; ci < ie; ++ci)
                            {
                                build[ci] = cj_flip(build[ci - 1]);
                            }
                        }
                        else
                        {
                            for (u_t ci = ib; ci < ie; ++ci)
                            {
                                build[ci] = salt[ib - 1];
                            }
                        }
                    }
                    else // ie=len, end of string  
                    {
                        if ((-cj_cost) >= jc_cost)
                        {
                            for (u_t ci = ib; ci < ie; ++ci)
                            {
                                build[ci] = cj_flip(build[ci - 1]);
                            }
                            build[ie - 1] = 'J';
                        }
                        else
                        {
                            for (u_t ci = ib; ci < ie; ++ci)
                            {
                                build[ci] = 'J';
                            }
                        }
                    }
                }
            }
        }
        solution = pay(build, cj_cost, jc_cost);
    }
}

int Moons::pay(const string& s) const
{
    return pay(s, X, Y);
}

int Moons::pay(const string& s, int cj_cost, int jc_cost) const
{
    int ret = 0;
    const size_t len = s.size();
    for (size_t i = 0, i1 = 1; i1 < len; i = i1++)
    {
        if (s[i] != s[i1]) // CJ || JC
        {
            ret += (s[i] == 'C' ? cj_cost : jc_cost);
        }
    }
    return ret;
}

void Moons::print_solution(ostream &fo) const
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

    void (Moons::*psolve)() =
        (naive ? &Moons::solve_naive : &Moons::solve);
    ostream &fout = *pfo;
    ul_t fpos_last = pfi->tellg();
    for (unsigned ci = 0; ci < n_cases; ci++)
    {
        Moons moons(*pfi);
        getline(*pfi, ignore);
        if (tellg)
        {
            ul_t fpos = pfi->tellg();
            cerr << "Case (ci+1)="<<(ci+1) << ", tellg=[" <<
                fpos_last << " " << fpos << ") size=" <<
                (fpos - fpos_last) << "\n";
            fpos_last = fpos;
        }

        (moons.*psolve)();
        fout << "Case #"<< ci+1 << ":";
        moons.print_solution(fout);
        fout << "\n";
        fout.flush();
    }

    if (pfi != &cin) { delete pfi; }
    if (pfo != &cout) { delete pfo; }
    return 0;
}
