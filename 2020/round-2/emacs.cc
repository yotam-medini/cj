// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com --

#include <algorithm>
#include <fstream>
#include <sstream>
#include <iostream>
#include <iterator>
#include <array>
#include <queue>
#include <string>
#include <utility>
#include <vector>

#include <cstdlib>

using namespace std;

typedef unsigned u_t;
typedef unsigned long ul_t;
typedef unsigned long long ull_t;
typedef vector<int> vi_t;
typedef vector<u_t> vu_t;
typedef vector<vu_t> vvu_t;
typedef vector<ull_t> vull_t;
typedef array<int, 2> ai2_t;
typedef array<u_t, 2> au2_t;
typedef array<ull_t, 2> aull2_t;
typedef vector<ai2_t> vai2_t;
typedef vector<au2_t> vau2_t;

static unsigned dbg_flags;

template <typename T, std::size_t N >
ostream& operator<<(ostream& os, const array<T, N>& a)
{
    os << '(';
    const char *sep = "";
    for (const T& x: a)
    {
        os << sep << x; sep = ", ";
    }
    os << ')';
    return os;
}

template <typename V>
string v2str(const V& v)
{
    ostringstream os;
    os << "[";
    const char *sep = "";
    for (const auto& x: v)
    {
        os << sep << x; sep = " ";
    }
    os << "]";
    return os.str();
}

ostream& operator<<(ostream& os, const au2_t& a)
{
    return os << '{' << a[0] << ", " << a[1] << '}';
}

void minby(u_t& v, u_t x)
{
    if (v > x)
    {
        v = x;
    }
}

static const ull_t INF_DIST = 100000ull * 1000000ull; // 10^5 x 10^6

class Emacs
{
 public:
    Emacs(istream& fi);
    void solve_naive();
    void solve();
    void print_solution(ostream&) const;
    ull_t get_solution() const { return solution; }
 private:
    void compute_pmatch();
    ull_t dijkstra(u_t si, u_t ei);
    u_t left(u_t i) const { return (prog[i] == '(' ? i : pmatch[i]); }
    u_t K, Q;
    string prog;
    vull_t L, R, P;
    vu_t S, E;
    ull_t solution;
    vu_t pmatch;
};

Emacs::Emacs(istream& fi) : solution(0)
{
    fi >> K >> Q;
    prog.reserve(K + 2 + 1);
    fi >> prog;
    prog = string("(") + prog + string(")");
    L.reserve(K + 2); R.reserve(K + 2); P.reserve(K + 2);
    static ull_t const INFINITE_PRICE = 1000000000000ull;
    L.push_back(INFINITE_PRICE);
    R.push_back(INFINITE_PRICE);
    P.push_back(INFINITE_PRICE);
    copy_n(istream_iterator<ull_t>(fi), K, back_inserter(L));
    copy_n(istream_iterator<ull_t>(fi), K, back_inserter(R));
    copy_n(istream_iterator<ull_t>(fi), K, back_inserter(P));
    L.push_back(INFINITE_PRICE);
    R.push_back(INFINITE_PRICE);
    P.push_back(INFINITE_PRICE);
    S.reserve(Q); E.reserve(Q);
    copy_n(istream_iterator<u_t>(fi), Q, back_inserter(S));
    copy_n(istream_iterator<u_t>(fi), Q, back_inserter(E));
}

void Emacs::solve_naive()
{
    compute_pmatch();
    for (u_t qi = 0; qi < Q; ++qi)
    {
        ull_t t = dijkstra(S[qi], E[qi]);
        solution += t;
    }
}

void Emacs::compute_pmatch()
{
    pmatch = vu_t(size_t(K + 2), 0);
    vu_t left_pos;
    for (u_t ci = 0; ci < K + 2; ++ci)
    {
        if (prog[ci] == '(')
        {
            left_pos.push_back(ci);
        }
        else // ')'
        {
            u_t li = left_pos.back();
            pmatch[li] = ci;
            pmatch[ci] = li;
            left_pos.pop_back();
        }
    }
}

ull_t Emacs::dijkstra(u_t si, u_t ei)
{
    typedef pair<ull_t, u_t> dist_idx_t;
    typedef priority_queue<dist_idx_t> q_t;
    q_t q;
    vull_t dists(size_t(K + 2), ull_t(-1)); // infinity
    dists[si] = 0;
    q.push(dist_idx_t(0, si));
    while (!q.empty())
    {
        const dist_idx_t& top = q.top();
        ull_t d = top.first;
        u_t ci = top.second;
        q.pop();
        if (d == dists[ci])
        {
            vector<dist_idx_t> dis; dis.reserve(3);
            if (ci > 0)
            {
                dis.push_back(dist_idx_t{d + L[ci], ci - 1});
            }
            if (ci + 1 < K + 2)
            {
                dis.push_back(dist_idx_t{d + R[ci], ci + 1});
            }
            dis.push_back(dist_idx_t(d + P[ci], pmatch[ci]));
            for (const dist_idx_t& di: dis)
            {
                if (dists[di.second] > di.first)
                {
                    dists[di.second] = di.first;
                    q.push(di);
                }
            }
        }
    }
    return dists[ei];
}

void Emacs::solve()
{
}

void Emacs::print_solution(ostream &fo) const
{
    fo << ' ' << solution;
}

static int real_main(int argc, char** argv)
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

    void (Emacs::*psolve)() =
        (naive ? &Emacs::solve_naive : &Emacs::solve);
    ostream &fout = *pfo;
    ul_t fpos_last = pfi->tellg();
    for (unsigned ci = 0; ci < n_cases; ci++)
    {
        Emacs emacs(*pfi);
        getline(*pfi, ignore);
        if (tellg)
        {
            ul_t fpos = pfi->tellg();
            cerr << "Case (ci+1)="<<(ci+1) << ", tellg=[" <<
                fpos_last << " " << fpos << ") size=" <<
                (fpos - fpos_last) << "\n";
            fpos_last = fpos;
        }

        (emacs.*psolve)();
        fout << "Case #"<< ci+1 << ":";
        emacs.print_solution(fout);
        fout << "\n";
        fout.flush();
    }

    if (pfi != &cin) { delete pfi; }
    if (pfo != &cout) { delete pfo; }
    return 0;
}

typedef vector<bool> vb_t;

static void check_push_segment(vau2_t& segments, u_t b, u_t e)
{
    if (b < e)
    {
        segments.push_back(au2_t{b, e});
    }
}

static void rand_fill(vb_t& right)
{
    const u_t K = right.size();
    vau2_t segments;
    segments.push_back(au2_t{0, K});
    while (!segments.empty())
    {
        const au2_t seg = segments.back();
        const u_t b = seg[0], e = seg[1];
        segments.pop_back();
        const u_t delta = e - b; // != 0
        const u_t half = delta/2;
        const u_t add = 2*(rand() % half);
        if (add == 0)
        {
            right[b] = false;
            right[e - 1] = true;
            check_push_segment(segments, b + 1, e - 1);
        }
        else
        {
            u_t cut = b + add;
            right[b] = false;
            right[cut - 1] = true;
            check_push_segment(segments, b + 1, cut - 1);
            check_push_segment(segments, cut, e);
        }
    }
}

#if 0
static void rand_fill_recursive(vb_t& left, u_t b, u_t e) // (e - b) % 2 = 0
{
    if (b < e)
    {
        u_t delta = (e - b);
        u_t half = delta / 2;
        u_t add = 2*(rand() % half);
        if (add == 0)
        {
            left[b] = true;
            left[e - 1] = false;
            rand_fill(left, b + 1, e - 1);
        }
        else
        {
            u_t cut = b + add;
            left[b] = true;
            left[cut - 1] = false;
            rand_fill(left, b + 1, cut - 1);
            rand_fill(left, cut, e);
        }
    }
}
#endif

static void write_rand_inpt(const char* fn, u_t K, u_t Q)
{
    ofstream f(fn);
    f << "1\n" << K << ' ' << Q << '\n';
    vb_t right(size_t(K), false);
    rand_fill(right);
    for (bool b: right)
    {
        f << "()"[int(b)];
    }
    f << '\n';
    for (u_t lrp = 0; lrp < 3; ++lrp)
    {
        const char* sep = "";
        for (u_t i = 0; i < K; ++i)
        {
            f << sep << (rand() % 1000000) + 1; sep = " ";
        }
        f << '\n';
    }
    for (u_t se = 0; se < 2; ++se)
    {
        const char* sep = "";
        for (u_t i = 0; i < Q; ++i)
        {
            u_t idx = (rand() % K) + 1;
            f << sep << idx; sep = " ";
        }
        f << '\n';
    }
    f.close();
}

static int run_compare(const char* fn, bool compare)
{
    ull_t solution = 0, solution_naive = 0, dum;
    if (compare)
    {
        ifstream fin(fn);
        fin >> dum; // T=1
        Emacs emacs(fin);
        emacs.solve_naive();
        solution_naive = emacs.get_solution();
    }
    ifstream fin(fn);
    fin >> dum; // T=1
    Emacs emacs(fin);
    emacs.solve();
    solution = emacs.get_solution();
    int rc = 0;
    if (compare && (solution != solution_naive))
    {
        cerr << "Failed: solution=" << solution << " != naive=" <<
            solution_naive << '\n';
        rc = 1;
    }
    return rc;
}

static int test(int argc, char ** argv)
{
    int rc = 0;
    int ai = 1; // "test";
    int nt = stoi(argv[++ai]);
    int Kmin = stoi(argv[++ai]);
    int Kmax = stoi(argv[++ai]);
    int Q = stoi(argv[++ai]);
    const char* fn_small = "emacs-auto.in";
    const char* fn_big = "emacs-auto-big.in";
    for (int ti = 0; (ti < nt) && (rc == 0); ++ti)
    {
        cerr << "tested: " << ti << '/' << nt << '\n';
        u_t K = Kmin + rand() % (Kmax + 1 - Kmin);
        const char* fn = (K < 100 ? fn_small : fn_big);
        K = 2*(K / 2);
        write_rand_inpt(fn, K, Q);
        rc = run_compare(fn, K < 100);
    }
    return rc;
}

int main(int argc, char ** argv)
{
    int rc = 0;
    if ((argc > 1) && (string(argv[1]) == string("test")))
    {
        rc = test(argc, argv);
    }
    else
    {
        rc = real_main(argc, argv);
    }
    return rc;
}
