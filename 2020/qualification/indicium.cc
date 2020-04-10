// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com --

#include <algorithm>
#include <numeric>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <utility>

#include <cstdlib>

using namespace std;

typedef unsigned u_t;
typedef unsigned long ul_t;
typedef unsigned long long ull_t;
typedef vector<u_t> vu_t;
typedef vector<vu_t> vvu_t;
typedef vector<vvu_t> vvvu_t;
typedef vector<ul_t> vul_t;

static unsigned dbg_flags;

void permutation_first(vu_t &p, u_t n)
{
    p = vu_t(n);
    iota(p.begin(), p.end(), 0);
}

bool permutation_next(vu_t &p)
{
    u_t n = p.size();

    // find the last 'in order'
    int j = n - 1 - 1;
    while ((j >= 0) && (p[j] > p[j + 1]))
    {
        --j;
    }
    bool ret = j >= 0;
    if (ret)
    {
        // Find last > p[j]. Must find since p[j] < p[j+1]
        u_t l;
        for (l = n - 1; p[j] >= p[l]; --l);
        swap(p[j], p[l]);
        reverse(p.begin() + j + 1, p.end());
    }
    return ret;
}

static vvvu_t g_perms;
void init_perms()
{
    g_perms.clear();
    g_perms.push_back(vvu_t());
    for (u_t n = 1; n <= 6; ++n)
    {
        vvu_t perms;
        vu_t p;
        permutation_first(p, n);
        perms.push_back(p);
        while (permutation_next(p))
        {
            perms.push_back(p);
        }
        sort(perms.begin(), perms.end()); // not needed ?
        g_perms.push_back(perms);
    }
}

class Indicium
{
 public:
    Indicium(istream& fi);
    void solve_naive();
    void solve();
    void print_solution(ostream&) const;
 private:
    bool complete_mat();
    bool next_mat();
    bool match_row(const vu_t& p) const;
    bool fill_mat(u_t r0, u_t c9);
    u_t N, K;
    bool possible;
    vvu_t mat;
    vul_t row_mask;
    vul_t col_mask;
};

Indicium::Indicium(istream& fi) :  possible(true)
{
    fi >> N >> K;
}

void Indicium::solve_naive()
{
    if (g_perms.empty())
    {
        init_perms();
    }
    const vvu_t& perms = g_perms[N];
    const u_t np = perms.size();
    possible = false;
    for (u_t pi0 = 0; (pi0 < np) && !possible; ++pi0)
    {
        mat.clear();
        mat.push_back(perms[pi0]);
        bool done = !complete_mat();
        while (!possible && !done)
        {
            u_t trace = 0;
            for (u_t i = 0; i < N; ++i)
            {
                trace += mat[i][i];
            }
            u_t ktrace = trace + N; // since actually we should count fomr 1
            possible = ktrace == K;
            if (!possible)
            {
                done = !next_mat();
            }
        }        
    }
}

void Indicium::solve()
{
   if (N <= 5)
   {
       solve_naive();
   }
   else
   {
       possible = ((K != 2) && (K != N*N - 1));
       if (possible)
       {
           mat.reserve(N);
           u_t k = K - N;
           u_t q = k / N;
           row_mask = vul_t(size_t(N), 0);
           col_mask = vul_t(size_t(N), 0);
           for (u_t r = 0; r < N; ++r)
           {
               mat.push_back(vu_t(size_t(N), N));
               mat[r][r] = q;
               row_mask[r] = (1ul << q);
               col_mask[r] = (1ul << q);
           }
           u_t last = q + (k - N*q);
           mat[N - 1][N - 1] = last;
           row_mask[N - 1] = col_mask[N - 1] = (1ul << last);
           possible = fill_mat(0, 1);
       }
   }
}

bool Indicium::complete_mat()
{
    const vvu_t& perms = g_perms[N];
    const u_t np = perms.size();
    bool completed = false;
    for (u_t pi = 0; (pi < np) && !completed; ++pi)
    {
        const vu_t& p = perms[pi];
        bool match = match_row(p);
        if (match)
        {
            mat.push_back(p);
            completed = (mat.size() == N);
            if (!completed)
            {
                completed = complete_mat();
                if (!completed)
                {
                    mat.pop_back();
                }
            }
        }
    }
    completed = (mat.size() == N);
    return completed;
}

bool Indicium::next_mat()
{
    bool next = false;
    // const vvu_t& perms = g_perms[N]; (better to save pi ?)
    // const u_t np = perms.size();
    mat.pop_back();
    while ((mat.size() > 0) && !next)
    {
        vu_t p = mat.back();
        mat.pop_back();
        bool found_next_row = false;
        while ((!found_next_row) && permutation_next(p))
        {
            found_next_row = match_row(p);
        }
        if (found_next_row)
        {
            mat.push_back(p);
            next = complete_mat();
        }
    }
    return next;
}

bool Indicium::match_row(const vu_t& p) const
{
    u_t sz = mat.size();
    u_t sub_trace = p[sz];
    for (u_t i = 0; i < mat.size(); ++i)
    {
        sub_trace += mat[i][i];
    }
    u_t k = K - N;
    u_t add_potential = (N - (sz + 1))*(N - 1);
    bool match = (sub_trace <= k) && (k <= sub_trace + add_potential);
    for (u_t r = 0, nr = mat.size(); match && (r < nr); ++r)
    {
        const vu_t& row = mat[r];
        for (u_t c = 0; match && (c < N); ++c)
        {
            match = (p[c] != row[c]);
        }
    }
    return match;
}

bool Indicium::fill_mat(u_t r, u_t c)
{
    bool done = false;
    ul_t& rmask = row_mask[r];
    ul_t& cmask = col_mask[c];
    for (u_t v = 0; (v != N) && !done; ++v)
    {
        ul_t vbit = (1ul << v);
        if (((rmask & vbit) == 0) && ((cmask & vbit) == 0))
        {
            mat[r][c] = v;
            rmask |= vbit;
            cmask |= vbit;
            if ((r == N - 1) && (c == N - 2))
            {
                done = true;
            }
            else
            {
                ++c;
                if (c == r)
                {
                   ++c;
                }
                if (c == N)
                {
                    ++r;
                    c = 0;
                }
                done = fill_mat(r, c);
            }
            rmask &= ~vbit;
            cmask &= ~vbit;
        }
    }
    return done;
}

void Indicium::print_solution(ostream &fo) const
{
    if (possible)
    {
        fo << " POSSIBLE";
        for (const vu_t& row: mat)
        {
            fo << '\n';
            const char* sep = "";
            for (u_t x: row)
            {
                fo << sep << (x + 1);
                sep = " ";
            }
        }
    }
    else
    {
        fo << " IMPOSSIBLE";
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

    void (Indicium::*psolve)() =
        (naive ? &Indicium::solve_naive : &Indicium::solve);
    ostream &fout = *pfo;
    ul_t fpos_last = pfi->tellg();
    for (unsigned ci = 0; ci < n_cases; ci++)
    {
        Indicium indicium(*pfi);
        getline(*pfi, ignore);
        if (tellg)
        {
            ul_t fpos = pfi->tellg();
            cerr << "Case (ci+1)="<<(ci+1) << ", tellg=[" <<
                fpos_last << " " << fpos << ") size=" <<
                (fpos - fpos_last) << "\n";
            fpos_last = fpos;
        }

        (indicium.*psolve)();
        fout << "Case #"<< ci+1 << ":";
        indicium.print_solution(fout);
        fout << "\n";
        fout.flush();
    }

    if (pfi != &cin) { delete pfi; }
    if (pfo != &cout) { delete pfo; }
    return 0;
}
