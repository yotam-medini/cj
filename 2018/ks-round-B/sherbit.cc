// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com --

#include <algorithm>
#include <array>
#include <fstream>
#include <iostream>
#include <map>
#include <numeric>
#include <set>
#include <string>
#include <utility>
#include <vector>

#include <cstdlib>

using namespace std;

typedef unsigned u_t;
typedef unsigned long ul_t;
typedef unsigned long long ull_t;
typedef vector<bool> vb_t;
typedef set<u_t> setu_t;
typedef vector<u_t> vu_t;
typedef array<u_t, 2> au2_t;
typedef vector<au2_t> vau2_t;
typedef map<u_t, au2_t> u_to_au2_t;

static unsigned dbg_flags;

string u2str(ull_t x)
{
    string s = "";
    while (x > 0)
    {
        s.push_back("01"[x % 2]);
        x /= 2;
    }
    if (s.empty()) { s = string("0"); }
    string rs(s.rbegin(), s.rend());
    return rs;
}

u_t gcd(u_t m, u_t n)
{
   while (n)
   {
      u_t t = n;
      n = m % n;
      m = t;
   }
   return m;
}

ull_t choose(u_t n, u_t k, ull_t limit)
{
    if (k > n/2)
    {
        k = n - k;
    }
    vu_t high, low;
    high = vu_t(vu_t::size_type(k), 0);
    iota(high.begin(), high.end(), n - k + 1);
    u_t d = 2;
    while (d <= k)
    {
        u_t dd = d;
        for (vu_t::iterator i = high.begin(), e = high.end();
            (dd > 1) && (i != e); ++i)
        {
            u_t g = gcd(dd, *i);
            if (g > 1)
            {
                *i /= g;
                dd /= g;
            }
        }
        ++d;
    }
    // u_t r = accumulate(high.begin(), high.end(), 1, multiplies<u_t>());
    ull_t r = 1;
    for (vu_t::const_iterator i = high.begin(), e = high.end();
        (i != e) && (r < limit); ++i)
    {
        ull_t h = *i;
	r *= h;
    }
    if (r > limit)
    {
        r = limit;
    }
    return r;
}

#if 0
u_t choose_cache(u_t n, u_t k)
{
    typedef pair<u_t, u_t> uu_t;
    typedef map<uu_t, u_t> uu2u_t;
    static uu2u_t nk2c;

    u_t ret = 0;
    uu2u_t::key_type nk(n, k);
    pair<uu2u_t::iterator, uu2u_t::iterator> w = nk2c.equal_range(nk);
    if (w.first == w.second)
    {
        ret = choose(n, k);
        uu2u_t::value_type v(nk, ret);
        nk2c.insert(w.first, v);
    }
    else
    {
        ret = (*w.first).second;
    }
    return ret;
}
#endif

static void minby(u_t& v, const u_t x)
{
    if (v > x)
    {
        v = x;
    }
}

static void maxby(u_t& v, const u_t x)
{
    if (v < x)
    {
        v = x;
    }
}

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
    Constraint(const au2_t& _be={0, 0}, u_t _nb=0) : be(_be), nbits(_nb) {}
    Constraint(const ABC& abc) : be{abc.a, abc.b + 1}, nbits(abc.c) {}
    u_t size() const { return be[1] - be[0]; }
    au2_t be;
    u_t nbits;
    vu_t subs;
};
bool operator<(const Constraint& c0, const Constraint& c1)
{
    bool lt = (c0.be < c1.be);
    return lt;
}
typedef vector<Constraint> vconstraint_t;

class SubConstraint
{
 public:
    SubConstraint(const au2_t& _be={0, 0}, u_t _cmin=0, u_t _cmax=u_t(-1)) :
        be(_be), cmin(_cmin), cmax(_cmax),
	sub_solution(size_t(be[1] - be[0]), false) {}
    au2_t be;
    u_t size() const { return be[1] - be[0]; }
    u_t cmin, cmax;
    vu_t parents;
    vu_t end_parents;
    vb_t sub_solution;
};
bool operator<(const SubConstraint& c0, const SubConstraint& c1)
{
    bool lt = (c0.be < c1.be);
    return lt;
}
typedef vector<SubConstraint> vsubconstraint_t;

#if 0
class SubBits : public au2_t
{
 public:
    SubBits(u_t i=0, u_t nb=0) : au2_t({i, nb}) {}
    u_t subc_idx() const { return (*this)[0]; }
    void subc_idx(u_t i) { (*this)[0] = i; }
    u_t pre_bits() const { return (*this)[1]; }
    void pre_bits(u_t nb) { (*this)[1] = nb; }
};
typedef map<SubBits, ull_t> sb2ull_t;
#endif

class SherBit
{
 public:
    SherBit(istream& fi);
    void solve_naive();
    void solve();
    void print_solution(ostream&) const;
 private:
    bool legal(const ull_t n) const;
    void set_sconstraints();
    ull_t sub_solve(const vu_t& pre_bits);
    ull_t parent_pre_bits(u_t pi, const vu_t& pre_bits) const;
    u_t N;
    u_t K;
    ull_t P;
    vabc_t constraints;
    vconstraint_t sconstraints;
    vsubconstraint_t sub_cstr;
    string solution;
    typedef map<vu_t, ull_t> memo_t;
    memo_t memo;
};

SherBit::SherBit(istream& fi)
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
		 string s = u2str(n);
		 size_t nz = N - s.size();
		 string zs(nz, '0');
		 solution = zs + s;
	     }
	 }
    }
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

void SherBit::solve()
{
    set_sconstraints();
    sub_solve(vu_t());
}

void SherBit::set_sconstraints()
{
    sconstraints = vconstraint_t{constraints.begin(), constraints.end()};
    sort(sconstraints.begin(), sconstraints.end());
    setu_t bes;
    for (const Constraint& c: sconstraints)
    {
        bes.insert(bes.end(), c.be[0]);
        bes.insert(bes.end(), c.be[1]);
    }
    setu_t::const_iterator iter = bes.begin();
    for (u_t b = *iter, e = *(++iter); iter != bes.end(); b = e, e = *(++iter))
    {
        sub_cstr.push_back(SubConstraint(au2_t{b, e}));
    }

    // cmin pass
    for (u_t ci = 0; ci != K; ++ci)
    {
        Constraint& c = sconstraints[ci];
	SubConstraint point(au2_t{c.be[0], c.be[0]});
        u_t scb = upper_bound(sub_cstr.begin(), sub_cstr.end(), point) -
	    sub_cstr.begin();
	u_t sce = scb;
	for (; (sce < sub_cstr.size()) && (sub_cstr[sce].be[1] <= c.be[1]);
	     ++sce)
	{
	    c.subs.push_back(sce);
	    if (sub_cstr[sce].be[1] == c.be[1])
	    {
	        sub_cstr[sce].end_parents.push_back(ci);
	    }
	    else
	    {
                sub_cstr[sce].parents.push_back(ci);
	    }
	}
	for (u_t sci = scb; sci != sce; ++sci)
	{
	    SubConstraint& subc = sub_cstr[sci];
	    u_t other_size = c.size() - subc.size();
	    u_t cmin = c.nbits - min(other_size, c.nbits);
	    maxby(subc.cmin, cmin);
	}
    }
    // cmax pass
    for (u_t ci = 0; ci != K; ++ci)
    {
        const Constraint& c = sconstraints[ci];
	u_t sum_cmin = 0;
	SubConstraint point(au2_t{c.be[0], c.be[0]});
        u_t scb = upper_bound(sub_cstr.begin(), sub_cstr.end(), point) -
	    sub_cstr.begin();
	u_t sce = scb;
	for (; (sce < sub_cstr.size()) && (sub_cstr[sce].be[1] <= c.be[1]);
	     ++sce)
	{
	    const SubConstraint& subc = sub_cstr[sce];
	    sum_cmin += subc.cmin;
	}
	for (u_t sci = scb; sci != sce; ++sci)
	{
	    SubConstraint& subc = sub_cstr[sci];
	    u_t other_cmin = sum_cmin - subc.cmin;
	    u_t cmax = c.nbits - min(other_cmin, c.nbits);
	    minby(subc.cmax, cmax);
	}
    }
}

ull_t SherBit::sub_solve(const vu_t& pre_bits)
{
    // SubBits key{subi, pre_bits};
    u_t subi = pre_bits.size();
    auto er = memo.equal_range(pre_bits);
    memo_t::iterator iter = er.first;
    if (er.first == er.second)
    {
	ull_t nnn = 0;
	SubConstraint& sc = sub_cstr[subi];
	vu_t sub_pre_bits(pre_bits);
	for (u_t my_bits = sc.cmin; (my_bits <= sc.cmax) && solution.empty();
	     ++my_bits)
	{
	    bool can = true;
	    for (u_t ci = 0, ce = sc.parents.size(); can && (ci != ce); ++ci)
	    {
		u_t pi = sc.parents[ci];
		u_t nb = parent_pre_bits(pi, pre_bits) + my_bits;
		can = nb <= sconstraints[pi].nbits;
	    }
	    for (u_t ci = 0, ce = sc.end_parents.size(); can && (ci != ce);
		 ++ci)
	    {
		u_t pi = sc.end_parents[ci];
		u_t nb = parent_pre_bits(pi, pre_bits) + my_bits;
		can = nb == sconstraints[pi].nbits;
	    }
	    if (can)
	    {
		ull_t tail = 1;
		if (subi + 1 < sub_cstr.size())
		{
		    sub_pre_bits.push_back(my_bits);
		    tail = sub_solve(sub_pre_bits);
		    sub_pre_bits.pop_back();
		}
		if (!solution.empty())
		{
		    nnn = P;
		}
		else
		{
		    // chosse ...
		    ull_t choose_my = choose(sc.size(), my_bits, P - nnn);
		    ull_t ct = choose_my * tail;
		    if (nnn + ct >= P)
		    {
			; // found!!
			build_solution(sub_pre_bits, my_bits);
		    }
		    else
		    {
			nnn += ct;
		    }
		}
	    }
	}
	memo_t::value_type v{pre_bits, nnn};
	iter = memo.insert(iter, v);
    }
    ull_t ret = iter->second;
    return ret;
}

ull_t SherBit::parent_pre_bits(u_t pi, const vu_t& pre_bits) const
{
    u_t nb = 0;
    const Constraint& parent = sconstraints[pi];
    for (u_t subi: parent.subs)
    {
        if (subi < pre_bits.size())
	{
	    nb += pre_bits[subi];
	}
    }
    return nb;
}

void SherBit::print_solution(ostream &fo) const
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
