// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com --

#include <algorithm>
#include <fstream>
#include <iostream>
#include <iterator>
// #include <map>
// #include <set>
#include <string>
#include <utility>
#include <vector>

#include <cstdlib>

using namespace std;

typedef unsigned u_t;
typedef unsigned long ul_t;
typedef unsigned long long ull_t;
typedef vector<string> vs_t;
typedef vector<u_t> vu_t;;

#include "bigint.h"

static unsigned dbg_flags;

class INode; class OpNode; // forward

class BaseNode
{
 public:
    virtual ~BaseNode() {}
    virtual const class INode* inode() const { return 0; }
    virtual class INode* inode() { return 0; }
    virtual const class OpNode* opnode() const { return 0; }
    virtual class OpNode* opnode() { return 0; }
    virtual string str() const = 0;
};
typedef vector<BaseNode*> vpnode_t;

class INode: public BaseNode
{
 public:
    const INode* inode() const { return this; }
    INode* inode() { return this; }
    string str() const { return n.dec(); }
    bigint32_t n;
};

class OpNode: public BaseNode
{
 public:
    OpNode(char _op='+') : op(_op) {}
    virtual ~OpNode()
    {
        for (BaseNode* p: children)
        {
            delete p;
        }
    }
    const OpNode* opnode() const { return this; }
    OpNode* opnode() { return this; }
    string str() const;
    char op;
    vpnode_t children; // at least 2, always 2 for op=#
};

string OpNode::str() const
{
    string s;
    s.push_back('(');
    bool already_looped = false;
    for (const BaseNode* p: children)
    {
        if (already_looped)
        {
            s.push_back(op);
        }
        s += children[0]->str();
        already_looped = true;
    }
    s.push_back(')');
    return s;
}

bool bn_equal(const BaseNode* p0, const BaseNode* p1)
{
    const INode* pn0 = p0->inode();
    const INode* pn1 = p1->inode();
    const OpNode* pb0 = p0->opnode();
    const OpNode* pb1 = p1->opnode();
    bool eq = ((!!pn0) == (!!pn1)) && ((!!pb0) == (!!pb1));
    if (eq)
    {
        if (pn0) // && pn1
        {
            eq = bigint32_t::eq(pn0->n, pn1->n);
        }
        else // pb0 && pb1
        {
            eq = (pb0->op == pb1->op);
            for (u_t i: {0, 1})
            {
                eq = eq && bn_equal(pb0->children[i], pb1->children[i]);
            }
        }
    }
    
    return eq;
}

bool bn_lt(const BaseNode* p0, const BaseNode* p1)
{
    const INode* pn0 = p0->inode();
    const INode* pn1 = p1->inode();
    const OpNode* pb0 = p0->opnode();
    const OpNode* pb1 = p1->opnode();
    bool lt = pn0 && pb1;
    bool teq = ((!!pn0) == (!!pn1)) && ((!!pb0) == (!!pb1));
    if ((!lt) && teq)
    {
        if (pn0) // && pn1
        {
            lt = bigint32_t::lt(pn0->n, pn1->n);
        }
        else // pb0 && pb1
        {
            const size_t sz0 = pb0->children.size();
            const size_t sz1 = pb1->children.size();
            const size_t sz_min = min(sz0, sz1);
            size_t i = 0;
            for (; (i < sz_min) && 
                bn_equal(pb0->children[i], pb1->children[i]); ++i)
            {}
            lt = (i < sz_min 
                ? bn_lt(pb0->children[i], pb1->children[i])
                : (sz0 < sz1));
        }
    }
    return lt;
}

class BinOp
{
 public:
    BinOp(istream& fi);
    ~BinOp();
    void solve_naive();
    void solve();
    void print_solution(ostream&) const;
 private:
    void parse_expressions();
    BaseNode* parse(const string& e, size_t sb, size_t se) const;
    BaseNode* reduce_naive(BaseNode* p);
    void classify();
    static const bigint32_t big_one;
    u_t N;
    vs_t expressions;
    vu_t solution;
    vpnode_t exp_nodes;
};
const bigint32_t BinOp::big_one(1);

BinOp::BinOp(istream& fi)
{
    fi >> N;
    expressions.reserve(N);
    copy_n(istream_iterator<string>(fi), N, back_inserter(expressions));
}

BinOp::~BinOp()
{
    for (BaseNode* p: exp_nodes)
    {
        delete p;
    }
}

void BinOp::solve_naive()
{
    parse_expressions();
    for (size_t i = 0; i < N; ++i)
    {
        if (dbg_flags & 0x1) { cerr << "i="<<i << ", before reduce: " <<
            exp_nodes[i]->str() << '\n'; }
        exp_nodes[i] = reduce_naive(exp_nodes[i]);
        if (dbg_flags & 0x1) { cerr << "i="<<i << ", after reduce: " <<
             exp_nodes[i]->str() << '\n'; }
    }
    classify();
}

void BinOp::classify()
{
    vu_t class2idx;

    solution.push_back(0);
    class2idx.push_back(0);
    
    for (size_t ei = 1; ei < N; ++ei)
    {
        const BaseNode* e = exp_nodes[ei];
        const size_t nc = class2idx.size();
        size_t ci_match = nc; // undefined
        for (size_t ci = 0; (ci < nc) && (ci_match == nc); ++ci)
        {
            size_t ei_first = class2idx[ci];
            if (bn_equal(e, exp_nodes[ei_first]))
            {
                ci_match = ci;
            }
        }
        solution.push_back(ci_match);
        if (ci_match == nc)
        {
            class2idx.push_back(ei);
        }
    }
}

void BinOp::solve()
{
     solve_naive();
}

void BinOp::parse_expressions()
{
    exp_nodes.reserve(N);
    for (const string e: expressions)
    {
        exp_nodes.push_back(parse(e, 0, e.size()));
    }
}

BaseNode* BinOp::parse(const string& e, size_t sb, size_t se) const
{
    BaseNode* p = 0;
    if (e[sb] != '(')
    {
        INode* pp = new INode;
        const string sub(e.begin() + sb, e.begin() + se);
        pp->n.sset(sub, 10);
        p = pp;
    }
    else
    {
        // get main binary operator position
        size_t binop_position = 0;
        u_t nest = 0;
        for (size_t si = sb; binop_position == 0; ++si)
        {
            const char c = e[si];
            switch (c)
            {
             case '(':
                ++nest;
                break;
             case ')':
                --nest;
                break;
             case '+':
             case '*':
             case '#':
                if (nest == 1)
                {
                    binop_position = si;
                }
            }
        }
        OpNode* pp = new OpNode(e[binop_position]);
        pp->children.push_back(parse(e, sb + 1, binop_position));
        pp->children.push_back(parse(e, binop_position + 1, se - 1));
        p = pp;
    }
    return p;
}

BaseNode* BinOp::reduce_naive(BaseNode* p)
{
    BaseNode* ret = p;
    OpNode* pp = p->opnode();
    if (pp)
    {
        INode* pn[2];   
        for (u_t i: {0, 1})
        {
            pp->children[i] = reduce_naive(pp->children[i]);
            pn[i] = pp->children[i]->inode();
        }
        if (pp->op != '#')
        {
            if (pn[0] && pn[1])
            {
                INode* iret = new INode;
                ret = iret;
                if (pp->op == '+')
                {
                    bigint32_t::add(iret->n, pn[0]->n, pn[1]->n);
                }
                else // '*''
                {
                    bigint32_t::mult(iret->n, pn[0]->n, pn[1]->n);
                }
                delete pp;
            }
            else if (pn[0] || pn[1])
            {
                int i_replace = -1;
                for (u_t i = 0; (i < 2) && (i_replace == -1); ++i)
                {
                    INode* pni = pn[i];
                    if (pni)
                    {
                        if (pni->n.is_zero())
                        {
                            i_replace = (pp->op == '*') ? i : 1 - i;
                        }
                        else if ((pp->op == '*') &&
                            bigint32_t::eq(pn[i]->n, big_one))
                        {
                            i_replace = 1 - i;
                        }
                    }
                }
                if (i_replace != -1)
                {
                    ret = pp->children[i_replace];
                    pp->children[i_replace] = 0;
                    delete pp;
                }
            }
        }
    }
    return ret;
}

void BinOp::print_solution(ostream &fo) const
{
    for (u_t i: solution)
    {
        fo << ' ' << (i + 1);
    }
}

int main(int argc, char ** argv)
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

    void (BinOp::*psolve)() =
        (naive ? &BinOp::solve_naive : &BinOp::solve);
    if (solve_ver == 1) { psolve = &BinOp::solve; } // solve1
    ostream &fout = *pfo;
    ul_t fpos_last = pfi->tellg();
    for (unsigned ci = 0; ci < n_cases; ci++)
    {
        BinOp binop(*pfi);
        getline(*pfi, ignore);
        if (tellg)
        {
            ul_t fpos = pfi->tellg();
            cerr << "Case (ci+1)="<<(ci+1) << ", tellg=[" <<
                fpos_last << " " << fpos << ") size=" <<
                (fpos - fpos_last) << "\n";
            fpos_last = fpos;
        }

        (binop.*psolve)();
        fout << "Case #"<< ci+1 << ":";
        binop.print_solution(fout);
        fout << "\n";
        fout.flush();
    }

    if (pfi != &cin) { delete pfi; }
    if (pfo != &cout) { delete pfo; }
    return 0;
}
