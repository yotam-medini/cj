// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com --

#include <fstream>
#include <iostream>
#include <string>
#include <utility>
#include <set>
#include <vector>
#include <deque>

#include <cstdlib>

using namespace std;

typedef unsigned u_t;
typedef unsigned long ul_t;
typedef unsigned long long ull_t;
typedef vector<u_t> vu_t;
typedef deque<u_t> dqu_t;
typedef vector<vu_t> vvu_t;

static unsigned dbg_flags;

class IndexComb
{
 public:
    dqu_t qidx; // size <= 6
    vu_t  combs;
    void drop_index(u_t idx0)
    {
        if ((!qidx.empty()) && (qidx.front() == idx0))
        {
            qidx.pop_front();
            vu_t new_combs;
            if (!qidx.empty())
            {
                ull_t set_bit_bits = 0;
                for (u_t bits: combs)
                {
                    bits >>= 1;
                    const ull_t bit = 1ull << bits;
                    if ((set_bit_bits & bit) != 0)
                    {
                        set_bit_bits |= bit;
                        new_combs.push_back(bits);
                    }
                }
            }
            swap(new_combs, combs);
        }
    }
}; 

class PaliFree
{
 public:
    PaliFree(istream& fi);
    PaliFree(u_t _N, const string& _S) : N(_N), S(_S), possible(false) {}
    void solve_naive();
    void solve();
    void print_solution(ostream&) const;
    bool get_solution() const { return possible; }
 private:
    bool is_palifree(const string& candid) const;
    void possible_low6();
    void possible_6(string& candid, u_t offset);
    bool is_palifree_sub6(const string& candid, u_t offset) const;
    u_t N;
    string S;
    bool possible;
    IndexComb index_comb;
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
            (candid[b + 1] == candid[b + 3]);
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
    string scandid(S);
    possible_low6();
    for (u_t b = 1; possible && (b + 6 < N); b += 1)
    {
        index_comb.drop_index(b);
        possible_6(scandid, b);
    }
}

void PaliFree::possible_low6()
{
    const u_t m6 = min<u_t>(N, 6);
    for (u_t i = 0; i < m6; ++i)
    {
        if (S[i] == '?')
        {
            index_comb.qidx.push_back(i);
        }
    }
    const u_t nq = index_comb.qidx.size();
    if ((nq == 0) && is_palifree(S.substr(0, m6)))
    {
        index_comb.combs.push_back(0); // something
    }
    else
    {
        for (u_t bits = 0; bits < (1u << nq); ++bits)
        {
            string candid = S.substr(0, m6);
            for (u_t bi = 0; bi < nq; ++bi)
            {
                const u_t si = index_comb.qidx[bi];
                char zo = ((bits & (1u << bi)) != 0 ? '1' : '0');
                candid[si] = zo;
            }
            if (is_palifree(candid))
            {
                index_comb.combs.push_back(bits);
            }
        }
    }
    possible = !index_comb.combs.empty();
}

void PaliFree::possible_6(string& candid, u_t offset)
{
    if (S[offset + 5] == '?')
    {
        index_comb.qidx.push_back(offset + 5);
        vu_t& combs = index_comb.combs;
        const u_t n_combs = combs.size();
        for (u_t i = 0; i < n_combs; ++i)
        {
            combs.push_back(combs[i] | (1u << 5));
        }
    }
    vu_t new_combs;
    for (u_t comb: index_comb.combs)
    {
        for (u_t bi = 0; bi < index_comb.qidx.size(); ++bi)
        {
            const u_t si = index_comb.qidx[bi];
            char zo = ((comb & (1u << bi)) != 0 ? '1' : '0');
            candid[si] = zo;
        }
        if (is_palifree_sub6(candid, offset))
        {
            new_combs.push_back(comb);
        }
    }
    possible = !(new_combs.empty());
    swap(index_comb.combs, new_combs);
}

bool PaliFree::is_palifree_sub6(const string& candid, u_t offset) const
{
    bool palifree = true;
    for (u_t b = offset; palifree && (b < offset + 2); ++b)
    {
        bool pali = 
            (candid[b + 0] == candid[b + 4]) &&
            (candid[b + 1] == candid[b + 3]);
        palifree = !pali;
    }
    if (palifree)
    {
        const u_t b = offset;
        bool pali = 
            (candid[b + 0] == candid[b + 5]) &&
            (candid[b + 1] == candid[b + 4]);
            (candid[b + 2] == candid[b + 3]);
        palifree = !pali;
    }
    return palifree;
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

static int test_specific(const string& s)
{
    int rc = 0;
    bool sol_naive = false, sol_prod = false;
    {
        PaliFree p_naive(s.size(), s);
        p_naive.solve_naive();
        sol_naive = p_naive.get_solution();
    }
    {
        PaliFree p_prod(s.size(), s);
        p_prod.solve();
        sol_prod = p_prod.get_solution();
    }
    if (sol_naive != sol_prod)
    {
        rc = 1;
        cerr << "Inconsistent: naive=" << sol_naive << " != " <<
            "prod=" << sol_prod << "\n";
        ofstream f("palifree-fail.in");
        f << "1\n" << s.size() << '\n' << s << '\n';
        f.close();
    }
    return rc;
}

static int test(int argc, char ** argv)
{
    int rc = 0;
    ull_t _3pN = 1;
    for (u_t N = 1; (rc == 0) && (N <= 16); ++N)
    {
        cerr << __func__ << ", N=" << N << " / 16\n";
        _3pN *= 3;
        string s;
        for (ull_t comb = 0; (rc == 0) && (comb < _3pN); ++comb)
        {
            ull_t vcomb = comb;
            while (s.size() < N)
            {
                s.push_back("01?"[vcomb % 3]);
                vcomb /= 3;
            }
            cerr << "N=" << N << ", " << comb << " / " << _3pN << '\n';
            rc = test_specific(s);
        }
    }
    return rc;
}

int main(int argc, char **argv)
{
    int rc = ((argc > 1) && (string(argv[1]) == string("test"))
        ? test(argc - 2, argv + 2)
        : real_main(argc, argv));
    return rc;
}
