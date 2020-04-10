// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com --

#include <algorithm>
#include <numeric>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <set>
#include <utility>

#include <cstdlib>

using namespace std;

typedef unsigned u_t;
typedef unsigned long ul_t;
typedef unsigned long long ull_t;
typedef vector<u_t> vu_t;
typedef vector<vu_t> vvu_t;
typedef vector<vvu_t> vvvu_t;
typedef vector<ull_t> vull_t;

typedef set<u_t> setu_t;
typedef vector<setu_t> vsetu_t;
typedef vector<vsetu_t> vvsetu_t;

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
    void get_trace(vu_t& trace) const;
    bool complete_mat();
    bool next_mat();
    bool match_row(const vu_t& p) const;
    bool fill_mat(u_t r0, u_t c9);
    bool fill_pmat(u_t r0, u_t c9);
    void init_possible_mat();
    u_t N, K;
    bool possible;
    vvu_t mat;
    vull_t row_mask;
    vull_t col_mask;
    vvsetu_t possible_mat;
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
            u_t ktrace = trace + N; // since actually we should count from 1
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
   if (false && (N <= 5))
   {
       solve_naive();
   }
   else
   {
       possible = ((K != N + 1) && (K != N*N - 1));
       if (possible)
       {
           mat.reserve(N);
           row_mask = vull_t(size_t(N), 0);
           col_mask = vull_t(size_t(N), 0);
           vu_t trace0;
           get_trace(trace0);
           for (u_t r = 0; r < N; ++r)
           {
               mat.push_back(vu_t(size_t(N), N));
               u_t v = trace0[r];               
               mat[r][r] = v;
               row_mask[r] = (1ul << v);
               col_mask[r] = (1ul << v);
           }
           init_possible_mat();
           possible = fill_pmat(0, 1);
       }
   }
}

void Indicium::get_trace(vu_t& trace) const
{
    u_t t = K - N;
    u_t q = t/N;
    trace = vu_t(size_t(N), q);
    u_t add = t - N*q;
    if (add > 0)
    {
        u_t a1 = add/2;
        if ((a1 == 0) && (trace[0] > 0))
        {
            trace[0] -= 1;
            ++a1;
            ++add;
        }
        u_t a2 = add - a1;
        if (a2 < a1)
        {
            swap(a1, a2);
        }
        trace[N - 2] += a1;
        trace[N - 1] += a2;
        u_t sub = 0;
        if (trace[N - 1] > N - 1)
        {
            sub = trace[N - 1] - (N - 1);
            trace[N - 1] = N - 1;
        }
        if (trace[N - 2] > N - 1)
        {
            sub += trace[N - 2] - (N - 1);
            trace[N - 2] = N - 1;
        }
        for (u_t i = N - 3; sub > 0; --i, --sub)
        {
            trace[i] += 1;
        }
    }
}

void Indicium::init_possible_mat()
{
    setu_t all;
    for (u_t i = 0; i < N; ++i)
    {
        all.insert(all.end(), i);
    }
    vsetu_t row = vsetu_t({size_t(N), all});
    possible_mat = vvsetu_t({size_t(N), row});
    for (u_t r = 0; r < N; ++r)
    {
        for (u_t c = 0; c < N; ++c)
        {
            setu_t& p = possible_mat[r][c];
            if (r == c)
            {
                p.clear();
            }
            else
            {
                p.erase(mat[r][r]);
                p.erase(mat[c][c]);
            }
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
    ull_t& rmask = row_mask[r];
    ull_t& cmask = col_mask[c];
    for (u_t v = 0; (v != N) && !done; ++v)
    {
        ull_t vbit = (1ul << v);
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

bool Indicium::fill_pmat(u_t r, u_t c)
{
    bool done = (r == N);
    if (done) { r = c = 0; }
    const setu_t& prc = possible_mat[r][c];
    for (setu_t::const_iterator i = prc.begin(), e = prc.end();
         (i != e) && !done; ++i)
    {
        const u_t v = *i;
        mat[r][c] = v;
        vsetu_t save_row; save_row.reserve(N - c - 1 );
        for (u_t k = c + 1; k < N; ++k)
        {
            save_row.push_back(possible_mat[r][k]);
            possible_mat[r][k].erase(v);
        }
        vsetu_t save_col; save_col.reserve(N - r - 1); 
        for (u_t k = r + 1; k < N; ++k)
        {
            save_col.push_back(possible_mat[k][c]);
            possible_mat[k][c].erase(v);
        }
        
        u_t rnext = r, cnext = c;        
        ++cnext;
        if (cnext == r)
        {
           ++cnext;
        }
        if (cnext == N)
        {
            ++rnext;
            cnext = 0;
        }
        done = fill_pmat(rnext, cnext);

        // restore
        for (u_t k = c + 1, si = 0; k < N; ++k, ++si)
        {
            swap(possible_mat[r][k], save_row[si]);
        }
        for (u_t k = r + 1, si = 0; k < N; ++k, ++si)
        {
            swap(possible_mat[k][c], save_col[si]);
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
