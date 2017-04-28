// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com -- Created: 2013/April/20

#include <iostream>
#include <fstream>
#include <string>
#include <set>
#include <queue>
#include <vector>
#include <utility>

#include <cstdlib>
#include <gmpxx.h>
#include <boost/numeric/ublas/matrix.hpp>

using namespace std;

// typedef mpz_class mpcz_t;
typedef mpq_class mpcq_t;
typedef unsigned long ul_t;
// typedef unsigned long long ull_t;
// typedef vector<ul_t> vul_t;
typedef boost::numeric::ublas::matrix<long> mtx_l_t;

static unsigned dbg_flags;

class Horse
{
 public:
    Horse(long ve=0, ul_t vs=0) : e(ve), s(vs) {}
    long e;
    ul_t s;
};
typedef vector<Horse> vh_t;

bool better(const Horse& h0, const Horse& h1)
{
    bool ret = (h0.e >= h1.e) && (h0.s >= h1.s);
    ret = ret && ((h0.e > h1.e) || (h0.s > h1.s));
    return ret;
}

class Delivery
{
 public:
    Delivery(unsigned vs=0, unsigned vd=0): start(vs), dest(vd) {}
    unsigned start;
    unsigned dest;
};
typedef vector<Delivery> deliveries_t;

class Option
{
 public:
    Option(mpcq_t vt=0, long ve=0, ul_t vs=0) : t(vt), e(ve), s(vs) 
    {
        td = t.get_d();
    }
    static bool better(const Option &o0, const Option &o1);
    string str() const;
    mpcq_t t;
    double td;
    long e;
    ul_t s;
};

string Option::str() const
{
    ostringstream oss;
    oss << "{t="<<t<<"="<<t.get_d() << ", e="<<e << ", s="<<s << "}";
    return oss.str();
}

bool operator<(const Option &o0, const Option &o1)
{
    bool lt = (o0.t < o1.t);
    if ((!lt) && (o0.t == o1.t))
    {
        lt = (o0.e < o1.e);
        if ((!lt) && (o0.e == o1.e))
        {
            lt = (o0.s < o1.s);
        }
    }
    return lt;
}

bool Option::better(const Option &o0, const Option &o1)
{
    bool ret = (o0.t <= o1.t) && (o0.e >= o1.e) && (o0.s >= o1.s);
    ret = ret && ((o0.t < o1.t) || (o0.e > o1.e) || (o0.s > o1.s));
    return ret;
}


typedef set<Option> setopt_t;

class City
{
 public:
    City() : earliest(-1) {}
    void add_option(const Option &o);
    bool add_vs(const Option &o, setopt_t &opts);
    mpcq_t earliest;
    setopt_t options;
    setopt_t new_options;
};

void City::add_option(const Option& o)
{
    if (dbg_flags & 0x1) { cerr << "add_option("<<o.str()<< "\n"; }
    if (earliest < 0)
    {
        earliest = o.t;
        new_options.insert(o);
    }
    else
    {
        if (earliest > o.t)
        {
            earliest = o.t;
        }
        bool add = add_vs(o, options);
        add = add_vs(o, new_options) && add;
        if (add)
        {
            new_options.insert(o);
        }
    }
}

bool City::add_vs(const Option& o, setopt_t &opts)
{
    bool ret = true;
    for (auto i = opts.begin(), i1 = i, e = opts.end(); i != e; i = i1)
    {
        ++i1;
        const Option &oo = *i;
        if (Option::better(o, oo))
        {
            opts.erase(i);
        }
        else if (Option::better(oo, o))
        {
            ret = false;
        }
    }
    return ret;
}

ostream &operator<<(ostream &os, const City &c)
{
    os << "earliest="<<c.earliest<<"="<<c.earliest.get_d() << "\n";
    os << "options["<<c.options.size()<<"]:\n";
    for (auto i = c.options.begin(), e = c.options.end(); i != e; ++i)
    {
        os << "  " << (*i).str() << "\n";
    }
    os << "new_options["<<c.new_options.size()<<"]:\n";
    for (auto i = c.new_options.begin(), e = c.new_options.end(); i != e; ++i)
    {
        os << "  " << (*i).str() << "\n";
    }
    return os;
}

class Pony
{
 public:
    Pony(istream& fi);
    void solve_naive();
    void solve();
    void print_solution(ostream&) const;
 private:
    typedef vector<City> vcity_t;
    void bfs();
    unsigned n, q;
    mtx_l_t dist;
    vh_t horses;
    deliveries_t deliveries;
    vcity_t cities;
    vector<mpcq_t> solution;
    unsigned di;
};

Pony::Pony(istream& fi)
{
    fi >> n >> q;
    horses.reserve(n);
    for (unsigned i = 0; i < n; ++i)
    {
        ul_t e, s;
        fi >> e >> s;
        horses.push_back(Horse(e, s));
    }
    dist = mtx_l_t(n, n);
    for (unsigned s = 0; s < n; ++s)
    {
        for (unsigned d = 0; d < n; ++d)
        {
            // fi >> dist(s, d);
            long x;
            fi >> x;
            dist(s, d) = x;
        }
    }
    deliveries.reserve(q);
    for (unsigned i = 0; i < q; ++i)
    {
        unsigned s, d;
        fi >> s >> d;
        deliveries.push_back(Delivery(s - 1, d - 1));
    }
}

void Pony::solve_naive()
{
}

void Pony::bfs()
{
    const Delivery &delivery = deliveries[di];
    cities = vcity_t(vcity_t::size_type(n), City());
    queue<unsigned> qcities;
    set<unsigned> qcities_set;
    unsigned cs = delivery.start;
    qcities.push(cs);
    qcities_set.insert(cs);
    cities[cs].add_option(Option(0, horses[cs].e, horses[cs].s));
    while (!qcities.empty())
    {
         unsigned ci = qcities.front();
         City &icity = cities[ci];
         if (dbg_flags & 0x2) { cerr << "ci="<<ci << ": " << icity << "\n"; }
         auto b_opt = icity.new_options.begin();
         auto e_opt = icity.new_options.end();
         qcities.pop();
         qcities_set.erase(ci);
         for (unsigned cj = 0; cj < n; ++cj)
         {
             long d = dist(ci, cj);
             if (d > 0)
             {
                 City &jcity = cities[cj];
                 mpcq_t earliest_old = jcity.earliest;
                 for (auto oi = b_opt; oi != e_opt; ++oi)
                 {
                     const Option &iopt = *oi;
                     if (iopt.e >= d)
                     {
                         mpcq_t t(d, iopt.s);
                         Option jopt(iopt.t + t, iopt.e - d, iopt.s);
                         jcity.add_option(jopt);
                     }
                 }
                 if ((jcity.earliest >= 0) &&
                    (((earliest_old < 0) || (earliest_old > jcity.earliest))))
                 {
                     Option jopt(jcity.earliest, horses[cj].e, horses[cj].s);
                     jcity.add_option(jopt);
                 }
                 if (!jcity.new_options.empty() && 
                     qcities_set.find(cj) == qcities_set.end())
                 {
                     qcities.push(cj);
                     qcities_set.insert(cj);
                 }
             }
         }
         icity.options.insert(b_opt, e_opt);
         icity.new_options.clear();
    }
    solution.push_back(cities[delivery.dest].earliest);
}

void Pony::solve()
{
    solution.reserve(q);
    for (di = 0; di < q; ++di)
    {
        bfs();
    }
}

void Pony::print_solution(ostream &fo) const
{
    for (unsigned i = 0; i < q; ++i)
    {
         char buf[0x20], *pbuf = &buf[0];
         double x = solution[i].get_d();
         sprintf(pbuf, " %.9f", x);
         if (strchr(pbuf, '.'))
         {
             unsigned nlen = strlen(pbuf);
             while (pbuf[--nlen] == '0')
             {
                 pbuf[nlen] = '\0';
             }
         }
         fo << pbuf;
    }
}

int main(int argc, char ** argv)
{
    const string dash("-");

    unsigned n_opts = 0;
    bool naive = false;

    if ((argc > 1) && (string(argv[1]) == "-naive"))
    {
        naive = true;
        n_opts = 1;
    }
    int ai_in = n_opts + 1;
    int ai_out = ai_in + 1;
    int ai_dbg = ai_out + 1;
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

    if (ai_dbg < argc) { dbg_flags = strtoul(argv[ai_dbg], 0, 0); }

    unsigned n_cases;
    *pfi >> n_cases;

    void (Pony::*psolve)() =
        (naive ? &Pony::solve_naive : &Pony::solve);

    ostream &fout = *pfo;
    bool tellg = getenv("TELLG");
    ul_t fpos_last = pfi->tellg();
    for (unsigned ci = 0; ci < n_cases; ci++)
    {
        Pony problem(*pfi);
        if (tellg)
        {
            ul_t fpos = pfi->tellg();
            cerr << "Case (ci+1)="<<(ci+1) << ", tellg=[" <<
                fpos_last << ", " << fpos << ")\n";
            fpos_last = fpos;
        }

        (problem.*psolve)();
        fout << "Case #"<< ci+1 << ":";
        problem.print_solution(fout);
        fout << "\n";
        fout.flush();

    }

    if (pfi != &cin) { delete pfi; }
    if (pfo != &cout) { delete pfo; }
    return 0;
}
