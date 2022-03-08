// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com --

#include <algorithm>
#include <fstream>
#include <iostream>
#include <string>
#include <queue>
#include <unordered_set>
#include <utility>
#include <tuple>
#include <vector>

#include <cstdlib>

using namespace std;

typedef unsigned u_t;
typedef unsigned long ul_t;
typedef unsigned long long ull_t;
typedef vector<u_t> vu_t;
typedef vector<ull_t> vull_t;
typedef pair<ull_t, vu_t> ull_vu_t;
// typedef priority_queue<ull_vu_t, vector<ull_vu_t>, greater<ull_vu_t>> pq_t;

static unsigned dbg_flags;

class Node
{
 public:
    typedef tuple<ull_t, vu_t, ull_t> t3_t;
    Node(ull_t _prod=1, const vu_t& _powers=vu_t(), ull_t _left=0) :
        t3{_prod, _powers, _left}
    {}
    ull_t& prod()   { return get<0>(t3); }
    vu_t&  powers() { return get<1>(t3); }
    ull_t& left()   { return get<2>(t3); }

    ull_t       prod()   const { return get<0>(t3); }
    const vu_t& powers() const { return get<1>(t3); }
    ull_t       left()   const { return get<2>(t3); }

    static bool lt(const Node& node0, const Node& node1)
    {
        bool ret = node0.t3 < node1.t3;
        return ret;
    }
 private:
    t3_t t3;
};

bool operator<(const Node& node0, const Node& node1)
{
    return Node::lt(node0, node1);
}
        
bool operator>(const Node& node0, const Node& node1)
{
    return Node::lt(node1, node0);
}
        
class Prime
{
 public:
    Prime(istream& fi);
    void solve_naive();
    void solve_pq();
    void solve();
    void print_solution(ostream&) const;
 private:
    void backtrace(ull_t prod, ull_t pi_max, vu_t& powers, ull_t left);
    u_t M;
    vu_t P, N;
    ul_t solution;
};

Prime::Prime(istream& fi) : solution(0)
{
    fi >> M;
    P.reserve(M);
    N.reserve(M);
    for (u_t i = 0; i < M; ++i)
    {
        u_t p;
        ull_t n;
        fi >> p >> n;
        P.push_back(p);
        N.push_back(n);
    }
}

void Prime::solve_naive()
{
    vu_t nums;
    for (u_t i = 0; i < M; ++i)
    {
        for (u_t k = 0; k < N[i]; ++k)
        {
            nums.push_back(P[i]);
        }
    }
    const u_t nn = nums.size();
    const u_t mask_max = 1u << nn;
    for (u_t mask = 0; mask < mask_max; ++mask)
    {
        ull_t sum = 0, prod = 1;
        for (u_t i = 0; i < nn; ++i)
        {
            if (((1u << i) & mask) == 0)
            {
                sum += nums[i];
            }
            else
            {
                prod *= nums[i];
            }
        }
        if ((sum == prod) && (solution < sum))
        {
            solution = sum;
        }
    }
}

void Prime::solve()
{
    ull_t total = 0;
    for (size_t i = 0; i < M; ++i)
    {
        total += P[i]*N[i];
    }
    vu_t powers(M, 0);
    backtrace(1, M - 1, powers, total);
}

void Prime::backtrace(ull_t prod, ull_t pi_max, vu_t& powers, ull_t left)
{
    if (prod <= left)
    {
        if (dbg_flags & 0x1) {
            cerr<<"pt_max="<<pi_max<<", prod="<<prod << ", left="<<left << '\n';}
        if ((prod == left) && (solution < prod))
        {
            solution = prod;
        }
        else
        {
            const ull_t curr_prime = P[pi_max];
            if ((powers[pi_max] < N[pi_max]) && (curr_prime < left))
            {
                ++powers[pi_max];
                backtrace(prod * curr_prime, pi_max, powers, left - curr_prime);
                --powers[pi_max];
            }
            if (pi_max > 0)
            {
                backtrace(prod, pi_max - 1, powers, left);
            }
        }
    }
}

void Prime::solve_pq()
{
    typedef priority_queue<Node, vector<Node>, greater<Node>> pq_t;
    typedef unordered_set<ull_t> set_ull_t;
    pq_t pq;
    set_ull_t qset;
    ull_t total = 0;
    for (size_t i = 0; i < M; ++i)
    {
        total += P[i]*N[i];
    }
    {
        vu_t powers_01(M, 0);
        for (size_t i = 0; i < M; ++i)
        {
            vu_t powers(M, 0);
            ull_t left = total - P[i];
            if (P[i] <= left)
            {
                powers_01[i] = 1;
                pq.push(Node(P[i], powers_01, left));
                qset.insert(qset.end(), P[i]);
                powers_01[i] = 0;
            }
        }
    }
    while (!pq.empty())
    {
        const Node& node = pq.top();
        ull_t prod = node.prod();
        const vu_t powers = node.powers(); // copy
        ull_t left = node.left();
        if (dbg_flags & 0x1) {cerr << "prod="<<prod << ", left="<<left << '\n';}
        pq.pop();
        qset.erase(prod);
        if ((prod == left) && (solution < prod))
        {
            solution = prod;
        }
        for (size_t i = 0; i < M; ++i)
        {
            if (powers[i] < N[i])
            {
                const ull_t p_prod = prod*P[i];
                const ull_t p_left = left - P[i];
                if ((p_prod <= p_left) && (qset.find(p_prod) == qset.end()))
                {
                    vu_t p_powers(powers);
                    p_powers[i] += 1;
                    pq.push(Node(p_prod, p_powers, p_left));
                    qset.insert(qset.end(), p_prod);
                }
            }
        }
    }
}

#if 0
void Prime::solve()
{
    pq_t pq;
    ull_t sum_all = 0;
    for (u_t i = 0; i < M; ++i)
    {
        sum_all += ull_t(N[i]) * ull_t(P[i]);
    }
    ull_t prod = P[0];
    ull_t subsum = sum_all - P[0];
    if (prod == subsum)
    {
        solution = prod;
    }
    else if (prod < subsum)
    {
        vu_t powers(size_t(M), 0);
        for (u_t i = 0; (i < M) && (solution == 0); ++i)
        {
            if (P[i] == sum_all - P[i])
            {
                solution = P[i];
            }
            else if (P[i] <= sum_all - P[i])
            {
                powers[i] = 1;
                // (ull_vu_t   {P[i], powers});
                pq.push(ull_vu_t{P[i], powers});
                powers[i] = 0;
            }
        }
        while ((!pq.empty()) && (solution == 0))
        {
            const ull_vu_t& pp = pq.top();
            const vu_t& pwrs = pp.second;
            u_t i = 0;
            for (i = 0; (i < M) && (pwrs[i] == N[i]); ++i) {}
            if (i < M)
            {
                ull_t next = pp.first * P[i];
                ull_t subsum_next = sum_all;
                for (u_t j = 0; j < M; ++j)
                {
                    subsum_next -= pwrs[i] * P[i];
                }
                if (next == subsum_next)
                {
                    solution = next;
                }
                else if (next <= subsum)
                {
                    vu_t pnext(pwrs);
                    ++pnext[i];
                    pq.push(ull_vu_t{next, pnext});
                }
            }
            pq.pop();
        }
    }
}
#endif

void Prime::print_solution(ostream &fo) const
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

    void (Prime::*psolve)() =
        (naive ? &Prime::solve_naive : &Prime::solve);
    ostream &fout = *pfo;
    ul_t fpos_last = pfi->tellg();
    for (unsigned ci = 0; ci < n_cases; ci++)
    {
        Prime prime(*pfi);
        getline(*pfi, ignore);
        if (tellg)
        {
            ul_t fpos = pfi->tellg();
            cerr << "Case (ci+1)="<<(ci+1) << ", tellg=[" <<
                fpos_last << " " << fpos << ") size=" <<
                (fpos - fpos_last) << "\n";
            fpos_last = fpos;
        }

        (prime.*psolve)();
        fout << "Case #"<< ci+1 << ":";
        prime.print_solution(fout);
        fout << "\n";
        fout.flush();
    }

    if (pfi != &cin) { delete pfi; }
    if (pfo != &cout) { delete pfo; }
    return 0;
}
