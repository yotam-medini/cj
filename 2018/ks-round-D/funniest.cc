// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com --

#include <algorithm>
#include <array>
#include <fstream>
#include <iostream>
#include <iterator>
#include <numeric>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

#include <cstdlib>

using namespace std;

typedef unsigned u_t;
typedef unsigned long ul_t;
typedef unsigned long long ull_t;
typedef vector<u_t> vu_t;
typedef vector<string> vs_t;
typedef vector<vs_t> vvs_t;
typedef vector<ull_t> vull_t;
typedef vector<vull_t> vvull_t;
typedef vector<vvull_t> vvvull_t;

static unsigned dbg_flags;

// class Trie;
// typedef array<Trie*, 26> atriep_t;
class Trie
{
 public:
    Trie() : terminal(false), children{nullptr} {}
    ~Trie()
    {
        for (Trie *p: children) { delete p; }
    }
    void add(const string& s, size_t offset=0)
    {
        if (offset == s.size())
        {
            terminal = true;
        }
        else
        {
            size_t ci = s[offset] - 'A';
            if (!children[ci])
            {
                children[ci] = new Trie;
            }
            children[ci]->add(s, offset + 1);
        }
    }
    bool search(const string& s, size_t offset=0) const
    {
        bool found = false;
        if (offset == s.size())
        {
            found = terminal;
        }
        else
        {
            size_t ci = s[offset] - 'A';
            found = children[ci] && children[ci]->search(s, offset + 1);
        }
        return found;
    }
    bool terminal;
    array<Trie*, 26> children;
};

class Funniest
{
 public:
    Funniest(istream& fi);
    Funniest(const vu_t&) {}; // TBD for test_case
    void solve_naive();
    void solve();
    void print_solution(ostream&) const;
    ull_t get_solution() const { return 0; }
 private:
    void init_sz_words();
    void naive_subgrid(u_t rb, u_t re, u_t cb, u_t ce);
    void set_rgrid();
    void set_cells();
    void fill_cells();
    void find_words_at(u_t r, u_t c, const Trie& t);
    void sum_scores();
    void subgrid(u_t rb, u_t re, u_t cb, u_t ce);
    bool candidate(ull_t sg_tl, ull_t sg_half_peri);
    void print_rc_scores() const;
    u_t R, C, W;
    vs_t grid;
    vs_t words;
    // total length, half perimeter, # of achieving subgrids
    ull_t solution_tl, solution_half_peri, solution_n;

    u_t sz_wmax;
    vvs_t sz_words, sz_rwords; // sorted, and sorted reversed words
    vs_t rgrid; // rotated grid by columns;

    Trie dtrie, rtrie; // direct, reverse
    // vvull_t hsub, hadd, vsub, vadd;
    vvvull_t r_cols_be_score, c_rows_be_score;
    vvvull_t r_sum_cols_be_score, c_sum_rows_be_score;
};

Funniest::Funniest(istream& fi) : 
    solution_tl(0), solution_half_peri(1), solution_n(0)
{
    fi >> R >> C >> W;
    grid.reserve(R);
    copy_n(istream_iterator<string>(fi), R, back_inserter(grid));
    words.reserve(W);
    copy_n(istream_iterator<string>(fi), W, back_inserter(words));
}

void Funniest::solve_naive()
{
    init_sz_words();

    for (u_t rb = 0; rb < R; ++rb)
    {
        for (u_t re = rb + 1; re <= R; ++re)
        {
            for (u_t cb = 0; cb < C; ++cb)
            {
                for (u_t ce = cb + 1; ce <= C; ++ce)
                {
                    naive_subgrid(rb, re, cb, ce);
                }
            }
        }
    }
}

void Funniest::naive_subgrid(u_t rb, u_t re, u_t cb, u_t ce)
{
    ull_t sg_tl = 0, sg_half_peri = (re - rb) + (ce - cb);
    for (u_t sz = 1; sz <= sz_wmax; ++sz)
    {
        const vs_t& awords = sz_words[sz];
        const vs_t& rwords = sz_rwords[sz];
        // LtoR + RtoL
        for (u_t r = rb; r < re; ++r)
        {
            for (u_t b = cb; b + sz <= ce; ++b)
            {
                const string s = grid[r].substr(b, sz);
                if (binary_search(awords.begin(), awords.end(), s))
                {
                    sg_tl += sz;
                }
                if (binary_search(rwords.begin(), rwords.end(), s))
                {
                    sg_tl += sz;
                }
            }
        }
 
        // TtoB + BtoT
        for (u_t c = cb; c < ce; ++c)
        {
            for (u_t b = rb; b + sz <= re; ++b)
            {
                string s;
                for (u_t si = 0; si < sz; ++si)
                {
                    s.push_back(grid[b + si][c]);
                }
                if (binary_search(awords.begin(), awords.end(), s))
                {
                    sg_tl += sz;
                }
                if (binary_search(rwords.begin(), rwords.end(), s))
                {
                    sg_tl += sz;
                }
            }
        }
    }
    bool better = candidate(sg_tl, sg_half_peri);
    if (better & (dbg_flags & 1)) {
        cerr << "rb="<<rb << ", re="<<re << ", cb="<<cb << ", ce="<<ce <<
            ", sg_tl="<<sg_tl << ", sg_half_peri="<<sg_half_peri << '\n'; }
}

void Funniest::solve()
{
    init_sz_words();
    set_rgrid();
    for (const string& w: words)
    {
        dtrie.add(w);
        const string revw(w.rbegin(), w.rend());
        rtrie.add(revw);
    }
    // hsub = hadd = vsub = vadd = vvull_t(R + 1, vull_t(C + 1, 0));
    r_cols_be_score = vvvull_t(R, vvull_t(C, vull_t(C + 1, 0)));
    c_rows_be_score = vvvull_t(C, vvull_t(R, vull_t(R + 1, 0)));
    for (u_t r = 0; r < R; ++r)
    {
        for (u_t c = 0; c < C; ++c)
        {
            find_words_at(r, c, dtrie);
            find_words_at(r, c, rtrie);
        }
    }
    if (dbg_flags & 0x1) { print_rc_scores(); }
    sum_scores();
    for (u_t rb = 0; rb < R; ++rb)
    {
        for (u_t re = rb + 1; re <= R; ++re)
        {
            for (u_t cb = 0; cb < C; ++cb)
            {
                for (u_t ce = cb + 1; ce <= C; ++ce)
                {
                    subgrid(rb, re, cb, ce);
                }
            }
        }
    }
}

void Funniest::init_sz_words()
{
    sz_wmax = 0;
    for (const string& w: words)
    {
        sz_wmax = max<u_t>(sz_wmax, w.size());
    }
    sz_words = vvs_t(sz_wmax + 1, vs_t());
    sz_rwords = vvs_t(sz_wmax + 1, vs_t());
    for (const string& w: words)
    {
        const u_t sz = w.size();
        sz_words[sz].push_back(w);
        const string rw(w.rbegin(), w.rend());
        sz_rwords[sz].push_back(rw);
    }
    for (u_t sz = 1; sz <= sz_wmax; ++sz)
    {
        sort(sz_words[sz].begin(), sz_words[sz].end());
        sort(sz_rwords[sz].begin(), sz_rwords[sz].end());
    }
}

void Funniest::set_rgrid()
{
    rgrid.reserve(C);
    for (u_t c = 0; c < C; ++c)
    {
        string s; s.reserve(R);
        for (u_t r = 0; r < R; ++r)
        {
            s.push_back(grid[r][c]);
        }
        rgrid.push_back(s);
    }
}

void Funniest::find_words_at(u_t r, u_t c, const Trie& t)
{
    const Trie* trie;
    u_t sz;
    // horizontal
    for (sz = 1, trie = &t; trie && (c + sz <= C); ++sz)
    {
        u_t ci = grid[r][c + sz - 1] - 'A';
        trie = trie->children[ci];
        const bool found = trie && trie->terminal;
        if (found)
        {
            vvull_t& cols_be_score = r_cols_be_score[r];
            for (u_t b = 0; b <= c; ++b)
            {
                for (u_t e = c + sz; e <= C; ++e)
                {
                    cols_be_score[b][e] += sz;
                }
            }
        }
    }
    // vertical
    for (sz = 1, trie = &t; trie && (r + sz <= R); ++sz)
    {
        u_t ci = grid[r + sz - 1][c] - 'A';
        trie = trie->children[ci]; 

        const bool found = trie && trie->terminal;
        if (found)
        {
            vvull_t& rows_be_score = c_rows_be_score[c];
            for (u_t b = 0; b <= r; ++b)
            {
                for (u_t e = r + sz; e <= R; ++e)
                {
                    rows_be_score[b][e] += sz;
                }
            }
        }
    }
}

void Funniest::sum_scores()
{
    r_sum_cols_be_score = vvvull_t(R + 1, vvull_t(C, vull_t(C + 1, 0)));
    c_sum_rows_be_score = vvvull_t(C + 1, vvull_t(R, vull_t(R + 1, 0)));
    for (u_t r = 0; r < R; ++r)
    {
         for (u_t b = 0; b < C; ++b)
         {
              for (u_t e = b + 1; e <= C; ++e)
              {
                   r_sum_cols_be_score[r + 1][b][e] = 
                       r_sum_cols_be_score[r][b][e] + r_cols_be_score[r][b][e];
              }
         }
    }
    for (u_t c = 0; c < C; ++c)
    {
         for (u_t b = 0; b < R; ++b)
         {
              for (u_t e = b + 1; e <= R; ++e)
              {
                   c_sum_rows_be_score[c + 1][b][e] = 
                       c_sum_rows_be_score[c][b][e] + c_rows_be_score[c][b][e];
                   
              }
         }
    }
}

void Funniest::subgrid(u_t rb, u_t re, u_t cb, u_t ce)
{
    const ull_t sg_half_peri = (re - rb) + (ce - cb);
    ull_t sg_tl = 0;
#if 1
    ull_t add;
    add = r_sum_cols_be_score[re][cb][ce] - r_sum_cols_be_score[rb][cb][ce];
    sg_tl += add;
    add = c_sum_rows_be_score[ce][rb][re] - c_sum_rows_be_score[cb][rb][re];
    sg_tl += add;
#else
    ull_t sg_tl_old = sg_tl;
    sg_tl = 0;
    for (u_t r = rb; r < re; ++r)
    {
        ull_t tadd = r_cols_be_score[r][cb][ce];
        sg_tl += tadd;
    }
    for (u_t c = cb; c < ce; ++c)
    {
        ull_t tadd = c_rows_be_score[c][rb][re];
        sg_tl += tadd;
    }
    if (sg_tl != sg_tl_old) {
        cerr << "ERROR\n";
        exit(1);
    }
#endif
    bool better = candidate(sg_tl, sg_half_peri);
    if (better & (dbg_flags & 1)) {
        cerr << "rb="<<rb << ", re="<<re << ", cb="<<cb << ", ce="<<ce <<
            ", sg_tl="<<sg_tl << ", sg_half_peri="<<sg_half_peri << '\n'; }
}

bool Funniest::candidate(ull_t sg_tl, ull_t sg_half_peri)
{
    bool better = false;
    ull_t fun_new = sg_tl * solution_half_peri;
    ull_t fun_old = solution_tl * sg_half_peri;
    if (fun_new > fun_old)
    {
        better = true;
        const ull_t g = gcd(sg_tl, sg_half_peri);
        solution_tl = sg_tl / g;
        solution_half_peri = sg_half_peri / g;
        solution_n = 1;
    }
    else if (fun_new == fun_old)
    {
        ++solution_n;
    }
    return better;
}

void Funniest::print_rc_scores() const
{
    size_t smax;
    cerr << "Horizontal:\n";
    smax = 0;
    for (u_t wet: {false, true}) {
        for (u_t r = 0; r < R; ++r) { 
            const vvull_t& cols_be_score = r_cols_be_score[r];
            for (u_t b = 0; b < C; ++b) {
                for (u_t e = b + 1; e <= C; ++e) {
                    ostringstream oss;
                    oss << '['<<b << ','<<e << ")=" << cols_be_score[b][e];
                    const string s = oss.str();
                    smax = max(smax, s.size());
                    if (wet) {
                        cerr << "  " << string(smax - s.size(), ' ') << s;
                    }
                }
            }
            if (wet) { cerr << '\n'; }
        }
    }
    cerr << "Vertical:\n";
    smax = 0;
    for (u_t wet: {false, true}) {
        for (u_t c = 0; c < C; ++c) { 
            const vvull_t& rows_be_score = c_rows_be_score[c];
            for (u_t b = 0; b < R; ++b) {
                for (u_t e = b + 1; e <= R; ++e) {
                    ostringstream oss;
                    oss << '['<<b << ','<<e << ")=" << rows_be_score[b][e];
                    const string s = oss.str();
                    smax = max(smax, s.size());
                    if (wet) {
                        cerr << "  " << string(smax - s.size(), ' ') << s;
                    }
                }
            }
            if (wet) { cerr << '\n'; }
        }
    }
}

void Funniest::print_solution(ostream &fo) const
{
    fo << ' ' << solution_tl << '/' << solution_half_peri << ' ' <<
        solution_n;
}

static int real_main(int argc, char ** argv)
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

    void (Funniest::*psolve)() =
        (naive ? &Funniest::solve_naive : &Funniest::solve);
    if (solve_ver == 1) { psolve = &Funniest::solve; } // solve1
    ostream &fout = *pfo;
    ul_t fpos_last = pfi->tellg();
    for (unsigned ci = 0; ci < n_cases; ci++)
    {
        Funniest funniest(*pfi);
        getline(*pfi, ignore);
        if (tellg)
        {
            ul_t fpos = pfi->tellg();
            cerr << "Case (ci+1)="<<(ci+1) << ", tellg=[" <<
                fpos_last << " " << fpos << ") size=" <<
                (fpos - fpos_last) << "\n";
            fpos_last = fpos;
        }

        (funniest.*psolve)();
        fout << "Case #"<< ci+1 << ":";
        funniest.print_solution(fout);
        fout << "\n";
        fout.flush();
    }

    if (pfi != &cin) { delete pfi; }
    if (pfo != &cout) { delete pfo; }
    return 0;
}

static u_t rand_range(u_t nmin, u_t nmax)
{
    u_t r = nmin + rand() % (nmax + 1 - nmin);
    return r;
}

static int test_case(int argc, char ** argv)
{
    int rc = rand_range(0, 1);
    ull_t solution(-1), solution_naive(-1);
    bool small = rc == 0;
    if (small)
    {
        Funniest p{vu_t()};
        p.solve_naive();
        solution_naive = p.get_solution();
    }
    {
        Funniest p{vu_t()};
        p.solve();
        solution = p.get_solution();
    }
    if (small && (solution != solution_naive))
    {
        rc = 1;
        cerr << "Failed: solution="<<solution << " != " <<
            solution_naive << " = solution_naive\n";
        ofstream f("funniest-fail.in");
        f << "1\n";
        f.close();
    }
    return rc;
}

static int test_random(int argc, char ** argv)
{
    int rc = 0;
    int ai = 0;
    u_t n_tests = strtoul(argv[ai++], 0, 0);
    for (u_t ti = 0; (rc == 0) && (ti < n_tests); ++ti)
    {
        cerr << "Tested: " << ti << '/' << n_tests << '\n';
        rc = test_case(argc, argv);
    }
    return rc;
}

int main(int argc, char **argv)
{
    int rc = ((argc > 1) && (string(argv[1]) == string("test"))
        ? test_random(argc - 2, argv + 2)
        : real_main(argc, argv));
    return rc;
}
