// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com --

#include <fstream>
#include <iostream>
#include <string>
#include <utility>
#include <set>
#include <vector>

#include <cstdlib>

using namespace std;

typedef unsigned u_t;
typedef unsigned long ul_t;
typedef unsigned long long ull_t;
typedef vector<u_t> vu_t;
typedef vector<vu_t> vvu_t;

static unsigned dbg_flags;

class PaliFree
{
 public:
    PaliFree(istream& fi);
    void solve_naive();
    void solve();
    void print_solution(ostream&) const;
 private:
    bool is_palifree(const string& candid) const;
    void possible_low6(vu_t& vbits);
    void possible_12(u_t b, const vu_t& low_bits, vu_t& high_bits);
    u_t N;
    string S;
    bool possible;
};

PaliFree::PaliFree(istream& fi) : possible(false)
{
    fi >> N >> S;
}

void PaliFree::solve_naive()
{
    vu_t qidx;
    for (u_t i = 0; i < N; ++i)
    {
        if (S[i] == '?')
        {
            qidx.push_back(i);
        }
    }
    const u_t nq = qidx.size();
    bool any_free = false;
    for (u_t bits = 0; (bits < (1u << nq)) && !any_free; ++bits)
    {
        string candid(S);
        for (u_t bi = 0; bi < nq; ++bi)
        {
            const u_t si = qidx[bi];
            char zo = ((bits & (1u << bi)) != 0 ? '1' : '0');
            candid[si] = zo;
        }
        any_free = is_palifree(candid);
    }
    possible = any_free;
}

bool PaliFree::is_palifree(const string& candid) const
{
    bool palifree = true;
    const u_t sz = candid.size();
    for (u_t b = 0; palifree && (b + (5 - 1) < sz); ++b)
    {
        bool pali = 
            (candid[b + 0] == candid[b + 4]) &&
            (candid[b + 1] == candid[b + 2]);
        palifree = !pali;
    }
    for (u_t b = 0; palifree && (b + (6 - 1) < sz); ++b)
    {
        bool pali = 
            (candid[b + 0] == candid[b + 5]) &&
            (candid[b + 1] == candid[b + 4]);
            (candid[b + 2] == candid[b + 3]);
        palifree = !pali;
    }
    return palifree;
}

void PaliFree::solve()
{
    if (N <= 15)
    {
        solve_naive();
    }
    else
    {
        vu_t low_bits, high_bits;
        possible_low6(low_bits);
        for (u_t b = 6; possible && (b + 6 < N); b += 6)
        {
            possible_12(b, low_bits, high_bits);
            swap(low_bits, high_bits);
        }
    }
}

void PaliFree::possible_low6(vu_t& vbits)
{
    vbits.clear();
    vu_t qidx;
    for (u_t i = 0; i < 6; ++i)
    {
        if (S[i] == '?')
        {
            qidx.push_back(i);
        }
    }
    const u_t nq = qidx.size();
    if ((nq == 0) && is_palifree(S.substr(0, 6)))
    {
        vbits.push_back(0); // something
    }
    else
    {
        for (u_t bits = 0; bits < (1u << nq); ++bits)
        {
            string candid = S.substr(0, 6);
            for (u_t bi = 0; bi < nq; ++bi)
            {
                const u_t si = qidx[bi];
                char zo = ((bits & (1u << bi)) != 0 ? '1' : '0');
                candid[si] = zo;
            }
            if (is_palifree(candid))
            {
                vbits.push_back(bits);
            }
        }
    }
    possible = !vbits.empty();
}

void PaliFree::possible_12(u_t b, const vu_t& low_bits, vu_t& high_bits)
{
    set<u_t> set_high_bits;
    vu_t qidx_low, qidx_high;
    for (u_t i = b; i < b + 6; ++i)
    {
        if (S[i] == '?')
        {
            qidx_low.push_back(i);
        }
    }
    const u_t e = min<u_t>(b + 12, N);
    for (u_t i = b + 6; i < e; ++i)
    {
        if (S[i] == '?')
        {
            qidx_high.push_back(i);
        }
    }
    for (u_t bits: low_bits)
    {
        string candid = S.substr(b, e - b);
        for (u_t bi = 0; bi < qidx_low.size(); ++bi)
        {
            const u_t si = qidx_low[bi];
            char zo = ((bits & (1u << bi)) != 0 ? '1' : '0');
            candid[qidx_low[si]] = zo;
        }
        if ((qidx_high.size() == 0) && (is_palifree(S.substr(b, e - b))))
        {
            set_high_bits.insert(set_high_bits.end(), 0); // something
        }
        else
        {
            for (u_t hbits = 0; hbits < (1u << qidx_high.size()); ++hbits)
            {
                for (u_t bi = 0; bi < qidx_high.size(); ++bi)
                {
                    const u_t si = qidx_low[bi];
                    char zo = ((hbits & (1u << bi)) != 0 ? '1' : '0');
                    candid[qidx_high[si]] = zo;
                    if (is_palifree(candid))
                    {
                        set_high_bits.insert(set_high_bits.end(), hbits);
                    }
                }
            }
        }
    }
    high_bits.clear();
    high_bits.insert(high_bits.end(), 
        set_high_bits.begin(), set_high_bits.end());
}

void PaliFree::print_solution(ostream &fo) const
{
    fo << ' ' << (possible ? "POSSIBLE" : "IMPOSSIBLE");
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

    void (PaliFree::*psolve)() =
        (naive ? &PaliFree::solve_naive : &PaliFree::solve);
    if (solve_ver == 1) { psolve = &PaliFree::solve; } // solve1
    ostream &fout = *pfo;
    ul_t fpos_last = pfi->tellg();
    for (unsigned ci = 0; ci < n_cases; ci++)
    {
        PaliFree palifree(*pfi);
        getline(*pfi, ignore);
        if (tellg)
        {
            ul_t fpos = pfi->tellg();
            cerr << "Case (ci+1)="<<(ci+1) << ", tellg=[" <<
                fpos_last << " " << fpos << ") size=" <<
                (fpos - fpos_last) << "\n";
            fpos_last = fpos;
        }

        (palifree.*psolve)();
        fout << "Case #"<< ci+1 << ":";
        palifree.print_solution(fout);
        fout << "\n";
        fout.flush();
    }

    if (pfi != &cin) { delete pfi; }
    if (pfo != &cout) { delete pfo; }
    return 0;
}

static int test_specific(int argc, char ** argv)
{
    int rc = 0;
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
    }
    return rc;
}

static int test(int argc, char ** argv)
{
    int rc = ((argc > 1) && (string(argv[1]) == string("specific"))
        ? test_specific(argc - 1, argv + 1)
        : test_random(argc, argv));
    return rc;
}

int main(int argc, char **argv)
{
    int rc = ((argc > 1) && (string(argv[1]) == string("test"))
        ? test(argc - 2, argv + 2)
        : real_main(argc, argv));
    return rc;
}
