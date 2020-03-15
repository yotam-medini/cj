// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com --

#include <algorithm>
#include <iostream>
#include <fstream>
#include <string>
#include <array>
#include <set>
#include <map>
#include <vector>
#include <utility>

#include <cstdlib>

using namespace std;

typedef unsigned u_t;
typedef unsigned long ul_t;
typedef unsigned long long ull_t;
typedef set<u_t> setu_t;
typedef vector<u_t> vu_t;
typedef array<u_t, 2> au2_t;
typedef vector<au2_t> vau2_t;
typedef map<u_t, au2_t> u_to_au2_t;

static unsigned dbg_flags;

class ABC
{
 public:
    ABC(u_t _a=0, u_t _b=0, u_t _c=0) : a(_a), b(_b), c(_c) {}
    bool legal(const ull_t n) const
    {
        u_t n1 = 0;
	for (u_t bi = a - 1; bi <= b - 1; ++bi)
	{
	    if ((n & (ull_t(1) << bi)) != 0)
	    {
	        ++n1;
	    }
	}
	bool ret = (n1 == c);
	return ret;
    }
    u_t a, b, c;
};
bool operator<(const ABC& v0, const ABC& v1)
{
    bool lt = (v0.a < v1.a) || ((v0.a == v1.a) && (v0.b < v1.b));
    return lt;
}
typedef vector<ABC> vabc_t;

class Constraint
{
 public:
   Constraint(const au2_t& _be={0, 0}, u_t _c=0) : be(_be), c(_c) {}
   Constraint(const ABC& abc) : be{abc.a, abc.b + 1}, c(abc.c) {}
   au2_t be;
   u_t c;
};
bool operator<(const Constraint& c0, const Constraint& c1)
{
    bool lt = (c0.be < c1.be);
    return lt;
}
typedef vector<Constraint> vconstraint_t;

class Segment
{
 public:
    Segment(const au2_t& _be=au2_t{0, 0}, const au2_t& pcr=au2_t{0, 0}) :
        be(_be), pre_count_range(pcr) {}
    au2_t be; // [begin, end)
    vu_t econst_idx; // ending constraints
    au2_t pre_count_range;
    u_to_au2_t pre_to_range;
};
typedef vector<Segment> vseg_t;

class SherBit
{
 public:
    SherBit(istream& fi);
    void solve_naive();
    void solve();
    void print_solution(ostream&) const;
 private:
    bool legal(const ull_t n) const;
    void set_disj_segs();
    void set_sconstraints();
    void bdys_add_constraint(setu_t& bdys, const Constraint& constraint) const
    {
	bdys.insert(bdys.end(), constraint.be[0]);
	bdys.insert(bdys.end(), constraint.be[1]);
    }
    void bdys_to_disj_segs(const setu_t& bdys);
    void set_econst_idx();
    ull_t tail_comb(u_t si, u_t pre_count);
    // ull_t search(seg_bits);
    u_t N;
    u_t K;
    ull_t P;
    vabc_t constraints;
    vconstraint_t sconstraints;
    vseg_t disj_segs;
    ull_t solution;
};

SherBit::SherBit(istream& fi) : solution(0)
{
    fi >> N >> K >> P;
    constraints.reserve(K);
    for (u_t ri = 0; ri != K; ++ri)
    {
        u_t a, b, c;
	fi >> a >> b >> c;
        constraints.push_back(ABC{a, b, c});
    }
}

void SherBit::solve_naive()
{
    for (ull_t n = 0, n_legal = 0; n_legal < P; ++n)
    {
         if (legal(n))
	 {
	     ++n_legal;
	     if (n_legal == P)
	     {
	         solution = n;
	     }
	 }
    }
}

static void maxby(u_t& r, u_t x)
{
    if (r < x)
    {
        r = x;
    }
}

void SherBit::solve()
{
    set_sconstraints();
//    set_disj_segs();
#if 0
    for (Segment& seg: disj_segs)
    {
        for (u_t si = 0, sn = sconstraints.size(); si != sn; ++si)
	{
	    if (seg.be[1] == sconstraints[si].b + 1)
	    {
	        seg.econst_idx.push_back(si);
	    }
	}
    }
#endif
}

void SherBit::set_sconstraints()
{
    vconstraint_t sc{constraints.begin(), constraints.end()};
    if (sc.size() > 0)
    {
        sort(sc.begin(), sc.end(), 
            [](const Constraint& c0, const Constraint& c1) -> bool
            {
                bool lt = (c0.be[1] < c1.be[1]) ||
                    ((c0.be[1] == c1.be[1]) && (c0.be[0] < c1.be[0]));
                return lt;
            });
        vconstraint_t split;
	for (u_t i = 0, i1 = 1, ns = sc.size(); i < ns; i = i1 )
	{
	    const u_t e = sc[i].be[1];
	    for ( ; (i1 < ns) && (sc[i1].be[1] == e); ++i1)
	    {
	        ;
	    }
	    if ((i1 != i + 1) || !split.empty())
	    {
                --i1;
                for (u_t si = i; si != i1; ++si)
                {
                    const Constraint& curr = sc[si];
                    const Constraint& next = sc[si + 1];
                    split.push_back(Constraint(au2_t{curr.be[0], next.be[0]},
                        next.c - curr.c));
                }
                split.push_back(sc[i1]);
                ++i1;
            }
	}
        if (!split.empty())
        {
            swap(sc, split);
            split.clear();
        }

        sort(sc.begin(), sc.end());
	for (u_t i = 0, i1 = 1, ns = sc.size(); i < ns; i = i1)
	{
	    const u_t b = sc[i].be[0];
	    for ( ; (i1 < ns) && (sc[i1].be[0] == b); ++i1)
	    {
	        ;
	    }
	    if ((i1 != i + 1) || !split.empty())
	    {
                split.push_back(sc[i]);
                for (u_t si = i + 1; si != i1; ++si)
                {
                    const Constraint& pre = split.back();
                    const Constraint& curr = sc[si];
                    split.push_back(Constraint(au2_t{pre.be[1], curr.be[1]},
                        curr.c - pre.c));
                }
            }
	}
        if (!split.empty())
        {
            swap(sc, split);
        }
    }
    swap(sconstraints, sc);
}

void SherBit::set_disj_segs()
{
    sconstraints.insert(sconstraints.end(), 
        constraints.begin(), constraints.end());
    sort(sconstraints.begin(), sconstraints.end());
    disj_segs.reserve(N);
    u_t rmax = (sconstraints.empty() ? 0 : sconstraints[0].be[1]);
    setu_t bdys;
    for (const Constraint& constraint: sconstraints)
    {
        if (constraint.be[0] < rmax)
	{
            maxby(rmax, constraint.be[1]);
	}
	else
	{
	    bdys_to_disj_segs(bdys);
	    bdys.clear();
	    rmax = 0;
	}
        bdys_add_constraint(bdys, constraint);
    }
    bdys_to_disj_segs(bdys);
    set_econst_idx();
}

void SherBit::bdys_to_disj_segs(const setu_t& bdys)
{
    u_t b = 0;
    for (u_t e: bdys)
    {
        if (b != 0)
	{
	    disj_segs.push_back(au2_t{b, e});
	}
	b = e;
    }
}

void SherBit::set_econst_idx()
{
}

bool SherBit::legal(const ull_t n) const
{
    bool ret = true;
    for (vabc_t::const_iterator i = constraints.begin(), e = constraints.end();
        ret && (i != e); ++i)
    {
        const ABC& constraint = *i;
	ret = constraint.legal(n);
    }
    return ret;
}

void SherBit::print_solution(ostream &fo) const
{
    vector<char> vc;
    ull_t n = solution;
    while (n > 0)
    {
        vc.push_back("01"[n % 2]);
	n /= 2;
    }
    vc.insert(vc.end(), N - vc.size(), '0');
    fo << ' ';
    for (vector<char>::const_reverse_iterator i = vc.rbegin(), e = vc.rend();
         i != e; ++i)
    {
        fo << *i;
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

    void (SherBit::*psolve)() =
        (naive ? &SherBit::solve_naive : &SherBit::solve);
    ostream &fout = *pfo;
    ul_t fpos_last = pfi->tellg();
    for (unsigned ci = 0; ci < n_cases; ci++)
    {
        SherBit sherbit(*pfi);
        getline(*pfi, ignore);
        if (tellg)
        {
            ul_t fpos = pfi->tellg();
            cerr << "Case (ci+1)="<<(ci+1) << ", tellg=[" <<
                fpos_last << " " << fpos << ") size=" <<
                (fpos - fpos_last) << "\n";
            fpos_last = fpos;
        }

        (sherbit.*psolve)();
        fout << "Case #"<< ci+1 << ":";
        sherbit.print_solution(fout);
        fout << "\n";
        fout.flush();
    }

    if (pfi != &cin) { delete pfi; }
    if (pfo != &cout) { delete pfo; }
    return 0;
}
