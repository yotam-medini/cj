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
    bool complete_mat();
    bool next_mat();
    bool match_row(const vu_t& p) const;
    const vu_t& set_row(vu_t& row, const vu_t& row0, u_t shift) const;
    void low_even(u_t n, u_t k);
    void low_odd(u_t n, u_t k);
    void fill_shift_rows(const vu_t& row0, u_t flip);
    void rev_mat();
    u_t N, K;
    bool possible;
    vvu_t mat;
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

class QR
{
 public:
    QR(u_t n, u_t k) : q(k / n), r(k % n), q0(q), q1(q)
    {
        if (r > 0)
        {
            u_t a0 = r/2;
            u_t a1 = r - a0;
            q0 = q + a0;
            q1 = q + a1;
            if (q0 == q)
            {
                --q0;
                ++q1;
            }
        }
        
    }
    u_t q, r, q0, q1;
};

void Indicium::solve()
{
    const u_t NNm1 = N*(N-1);
    const u_t k = K - N;
    possible = (k != 1) && (k + 1 != NNm1);
    possible = possible && ((N != 3) || ((k != 2) && (k != 4)));
    //possible = (k != 1) && (k + 1 != N) && 
    //    (k + N - 1 != NNm1) && (k + 1 != NNm1);
    if (possible)
    {
        const bool rev = 2*k > NNm1/2;
        u_t krev = (rev ? NNm1 - k : k);
        if (N % 2 == 0)
        {
            low_even(N, krev);
        }
        else
        {
            low_odd(N, krev);
        }
        if (rev)
        {
            rev_mat();
        }
    }
}

void Indicium::low_even(u_t n, u_t k)
{
    const QR qr(n, k);
    vu_t row0;
    row0.reserve(n);
    for (u_t i = 0; i != n; ++i)
    {
        row0.push_back((qr.q + i) % n);
    }
    u_t flip = 0;
    if (qr.r > 0)
    {
        u_t w0 = find(row0.begin(), row0.end(), qr.q0) - row0.begin();
        if (qr.q0 == qr.q1)
        {
            flip = n/2;
            swap(row0[flip], row0[w0]);
        }
        else
        {
            flip = 1;
            u_t w1 = find(row0.begin(), row0.end(), qr.q1) - row0.begin();
            if (w0 > w1)
            {
                swap(w0, w1);
            }
            swap(row0[1], row0[w0]);
            swap(row0[n - 1], row0[w1]);
        }
    }
    fill_shift_rows(row0, flip);
}

void Indicium::low_odd(u_t n, u_t k)
{
    const QR qr(n, k);
    vu_t row0;
    row0.reserve(n);
    for (u_t i = 0; i != n; ++i)
    {
        row0.push_back((qr.q + i) % n);
    }
    u_t flip = 0;
    bool done = false;
    if (qr.r > 0)
    {
        if (qr.q0 == qr.q1)
        {
            u_t kmax = (n - 1)*(n - 2);
            bool rev = k > kmax/2;
            low_even(n - 1, rev ? kmax - k : k);
            if (rev)
            {
                rev_mat();
            }
            for (u_t ri = 0; ri != n - 1; ++ri)
            {
                vu_t& row = mat[ri];
                row.insert(row.begin() + ri + 1, n - 1);
            }
            vu_t last_row;
            last_row.reserve(n);
            const u_t goal = (n*(n - 1))/2;
            for (u_t ci = 0; ci < n; ++ci)
            {
                u_t colsum = 0;
                for (u_t ri = 0; ri != n - 1; ++ri)
                {
                    colsum += mat[ri][ci];
                }
                const u_t v = goal - colsum;
                last_row.push_back(v);
            }
            mat.push_back(last_row);
            done = true;
        }
        else
        {
            flip = 1;
            u_t w0 = find(row0.begin(), row0.end(), qr.q0) - row0.begin();
            u_t w1 = find(row0.begin(), row0.end(), qr.q1) - row0.begin();
            if (w0 > w1)
            {
                swap(w0, w1);
            }
            swap(row0[1], row0[w0]);
            swap(row0[n - 1], row0[w1]);
        }
        
    }
    if (!done)
    {
        fill_shift_rows(row0, flip);
    }
}

void Indicium::fill_shift_rows(const vu_t& row0, u_t flip)
{
    const u_t n = row0.size();
    mat.clear();
    mat.reserve(n);
    for (u_t ri = 0; ri != n; ++ri)
    {
        vu_t row;
        mat.push_back(set_row(row, row0, ri));
    }
    swap(mat[0], mat[flip]);
}

const vu_t& Indicium::set_row(vu_t& row, const vu_t& row0, u_t shift) const
{
    const u_t n = row0.size();
    row.reserve(n);
    for (u_t i = 0; i < n; ++i)
    {
        row.push_back(row0[(n - shift + i) % n]);
    }
    return row;
}

void Indicium::rev_mat()
{
    const u_t n = mat.size();
    const u_t nm1 = n - 1;
    for (vu_t& row: mat)
    {
        for (u_t ci = 0; ci != n; ++ci)
        {
            row[ci] = nm1 - row[ci];
        }
    }
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
