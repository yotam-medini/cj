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
    virtual const class INode* i_node() const { return 0; }
    virtual class INode* i_node() { return 0; }
    virtual const class OpNode* op_node() const { return 0; }
    virtual class OpNode* op_node() { return 0; }
    virtual const class PlusNode* plus_node() const { return 0; }
    virtual class PlusNode* plus_node() { return 0; }
    virtual const class MultNode* mult_node() const { return 0; }
    virtual class MultNode* mult_node() { return 0; }
    virtual const class SharpNode* sharp_node() const { return 0; }
    virtual class SharpNode* sharp_node() { return 0; }
    virtual BaseNode* clone() const = 0;
    virtual BaseNode* reduce() = 0;
    virtual string str() const = 0;
 protected:
    static const bigint32_t big_one;
};
const bigint32_t BaseNode::big_one(1);

typedef vector<BaseNode*> vpnode_t;
typedef vpnode_t::iterator vpnd_iter_t;
typedef vector<const BaseNode*> vcpnode_t;

void vpnode_delete(vpnode_t& vp)
{
    for (const BaseNode* p: vp)
    {
        delete p;
    }
    vp.clear();
}

void vpnode_clone(vpnode_t& r, const vcpnode_t& orig)
{
    r.clear();
    r.reserve(orig.size());
    for (const BaseNode* p: orig)
    {
        r.push_back(p->clone());
    }
}

class INode: public BaseNode
{
 public:
    INode(const bigint32_t _n=bigint32_t(0)) : n(_n) {}
    const INode* i_node() const { return this; }
    INode* i_node() { return this; }
    string str() const { return n.dec(); }
    INode* clone() const { return new INode(n); }
    virtual BaseNode* reduce() { return this; }
    bigint32_t n;
};
typedef vector<INode*> vinode_t;

class OpNode: public BaseNode
{
 public:
    virtual ~OpNode()
    {
        for (BaseNode* p: children)
        {
            delete p;
        }
    }
    const OpNode* op_node() const { return this; }
    OpNode* op_node() { return this; }
    virtual char op() const = 0;
    string str() const;
    void reduce_sort_children()
    {
        reduce_children();
        sort_children();
    }
    void clone_children(OpNode& op) const;
    void reduce_children();
    void sort_children();
    vpnode_t children; // at least 2 for +, always 2 for op=#
};

class PlusNode: public OpNode
{
 public:
    const PlusNode* plus_node() const { return this; }
    PlusNode* plus_node() { return this; }
    char op() const { return '+'; }
    PlusNode* clone() const;
    BaseNode* reduce();
};

class MultNode: public OpNode
{
 public:
    MultNode(bigint32_t* f=nullptr) : factor(f) { }
    ~MultNode() { delete factor; }
    const MultNode* mult_node() const { return this; }
    MultNode* mult_node() { return this; }
    char op() const { return '*'; }
    MultNode* clone() const;
    BaseNode* reduce();
    const bigint32_t& get_factor() const
    {
        return factor ? *factor : big_one;
    }
    void set_factor(const bigint32_t& f);
    static const bigint32_t big_one;
    bigint32_t* factor;
 private:
    bool reduce_trivial(BaseNode*& ret);
};
const bigint32_t MultNode::big_one(1);

class SharpNode: public OpNode
{
 public:
    const SharpNode* sharp_node() const { return this; }
    SharpNode* sharp_node() { return this; }
    char op() const { return '#'; }
    SharpNode* clone() const;
    BaseNode* reduce()
    {
        reduce_children();
        return this;
    }
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
            s.push_back(op());
        }
        s += p->str();
        already_looped = true;
    }
    s.push_back(')');
    return s;
}

void OpNode::reduce_children()
{
    for (size_t i = 0, nc = children.size(); i < nc; ++i)
    {
        BaseNode* reduced = children[i]->reduce();
        if (children[i] != reduced)
        {
            delete children[i];
            children[i] = reduced;
        }
    }
}

void OpNode::clone_children(OpNode& op) const
{
    op.children.reserve(children.size());
    for (BaseNode* c: children)
    {
        op.children.push_back(c->clone());
    }
}

PlusNode* PlusNode::clone() const
{
    PlusNode* p = new PlusNode();
    clone_children(*p);
    return p;
}

MultNode* MultNode::clone() const
{
    MultNode* p = new MultNode();
    p->factor = (factor ? new bigint32_t(*factor) : nullptr);
    clone_children(*p);
    return p;
}

SharpNode* SharpNode::clone() const
{
    SharpNode* p = new SharpNode();
    clone_children(*p);
    return p;
}

bool bn_equal(const BaseNode* p0, const BaseNode* p1); // fwd

bool equal_children(const OpNode* p0, const OpNode* p1)
{
    bool eq = true;
    const size_t sz0 = p0->children.size();
    const size_t sz1 = p1->children.size();
    eq = eq && (sz0 == sz1);
    for (size_t i = 0; eq && (i < sz0); ++i)
    {
        eq = eq && bn_equal(p0->children[i], p1->children[i]);
    }
    return eq;
}

bool bn_equal(const BaseNode* p0, const BaseNode* p1)
{
    const INode* pn0 = p0->i_node();
    const INode* pn1 = p1->i_node();
    const OpNode* pb0 = p0->op_node();
    const OpNode* pb1 = p1->op_node();
    bool eq = ((!!pn0) == (!!pn1)) && ((!!pb0) == (!!pb1));
    if (eq)
    {
        if (pn0) // && pn1
        {
            eq = bigint32_t::eq(pn0->n, pn1->n);
        }
        else // pb0 && pb1
        {
            eq = (pb0->op() == pb1->op());
            eq = eq && equal_children(pb0, pb1);
            if (eq)
            {
                const MultNode* pm0 = pb0->mult_node();
                const MultNode* pm1 = pb1->mult_node();
                if (pm0)
                {
                    const bigint32_t* factor0 = pm0->factor;
                    const bigint32_t* factor1 = pm1->factor;
                    eq = (!factor0 && !factor1) ||
                         (factor0 && factor1 && 
                          bigint32_t::eq(*factor0, *factor1));
                }
            }            
        }
    }
    
    return eq;
}

// Assuming:
// 1. p0, p1 are reduced and children are reduced ans sorted
// 2. By reduction, PlusNode do NOT have PlusNode children

bool bn_lt(const BaseNode* p0, const BaseNode* p1)
{
    const INode* pn0 = p0->i_node();
    const INode* pn1 = p1->i_node();
    const OpNode* pb0 = p0->op_node();
    const OpNode* pb1 = p1->op_node();
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
            const OpNode* pp0 = p0->plus_node();
            // const OpNode* pp1 = p1->plus_node();
            const MultNode* pm0 = p0->mult_node();
            const MultNode* pm1 = p1->mult_node();
            // const OpNode* ps0 = p0->sharp_node();
            const OpNode* ps1 = p1->sharp_node();

            if (pb0->op() != pb1->op())
            {
                lt = (pp0 || (pm0 && ps1));
            }
            else // same op
            {
                const size_t sz0 = pb0->children.size();
                const size_t sz1 = pb1->children.size();
                const size_t sz_min = min(sz0, sz1);
                size_t i = 0;
                for (; (i < sz_min) && 
                    bn_equal(pb0->children[i], pb1->children[i]); ++i)
                {}
                if (i < sz_min)
                {
                    lt = bn_lt(pb0->children[i], pb1->children[i]);
                }
                else
                {
                    if (sz0 < sz1)
                    {
                        lt = true;
                    }
                    else if (pm0)
                    {
                        lt = bigint32_t::lt(
                            pm0->get_factor(), pm1->get_factor());
                    }
                }
            }
        }
    }
    return lt;
}

void OpNode::sort_children()
{
    sort(children.begin(), children.end(),
        [](const BaseNode* p0, const BaseNode* p1) -> bool
        {
            return bn_lt(p0, p1);
        });
}

BaseNode* PlusNode::reduce()
{
    reduce_children();
    for (size_t i = 0, nc = children.size(); i < nc; ++i)
    {
         PlusNode* pn = children[i]->plus_node();
         if (pn)
         {
             children[i] = pn->children[0];
             children.insert(children.end(), pn->children.begin() + 1,
                 pn->children.end());
             pn->children.clear();
             delete pn;
         }
    }
    bigint32_t tsum;
    // no more PlusNode child
    sort_children();
    vpnode_t new_children;
    const size_t nc = children.size();
    size_t i = 0;
    INode* inode0 = children[0]->i_node();
    if (inode0)
    {   INode* inode;
        for (i = 1; (i < nc) && ((inode = children[i]->i_node())); ++i)
        {
            bigint32_t::add(tsum, inode0->n, inode->n);
            bigint32_t::bi_swap(inode0->n, tsum);
            delete inode;
        }
        new_children.push_back(inode0);
    }
    size_t isharp = i;
    size_t isharp_into_mult = i;
    // MultNode-s are multiplications of SharpNode-s
    for (size_t imult = i; imult < isharp;)
    {
        const size_t imult_b = imult;
        MultNode* pmb = children[imult_b]->mult_node();
        bigint32_t factor = pmb->get_factor();
        MultNode* pm;
        for (++imult; (imult < isharp) && 
            ((pm = children[imult_b]->mult_node())) && equal_children(pmb, pm);
            ++imult)
        {
            bigint32_t::add(tsum, factor, pm->get_factor());
            bigint32_t::bi_swap(factor, tsum);
            delete children[imult_b];
            children[imult_b] = 0;
        }
        if (pmb->children.size() == 1) // single SharpNode
        {
            const SharpNode* sharp_term = pmb->children[0]->sharp_node();
            if (!sharp_term) {
                cerr << "BUG: SharpNode expected\n";
                exit(1);
            }
            SharpNode* ps;
            for ( ; (isharp_into_mult < nc) &&
                ((ps = children[isharp_into_mult]->sharp_node())) &&
                    bn_lt(ps, sharp_term);
                 ++isharp_into_mult)
            {}
            for ( ; (isharp_into_mult < nc) &&
                ((ps = children[isharp_into_mult]->sharp_node())) &&
                    bn_equal(ps, sharp_term);
                 ++isharp_into_mult)
            {
                bigint32_t::add(tsum, factor, big_one);
                bigint32_t::bi_swap(factor, tsum);
                delete ps;
                children[isharp_into_mult] = 0;
            }
        }
        if (factor.is_one())
        {
            if (pmb->factor)
            {
                delete pmb->factor;
                pmb->factor = 0;
            }
            if (pmb->children.size() == 1)
            {
                new_children.push_back(pmb->children[0]);
                new_children.clear();
            }
        }
        else
        {
            new_children.push_back(pmb);
        }
    }
    for ( ; isharp < nc; ++isharp) // those who were not joind to MultNode-s.
    {
        if (children[isharp])
        {
            new_children.push_back(children[isharp]);
        }
    }
    swap(children, new_children);
    sort_children();
    BaseNode* ret = this;
    if (children.size() == 1)
    {
        ret = children[0];
        children.clear();
        // caller will delete this
    }
    return ret;
}

PlusNode* mult_pluses(
    bigint32_t& f, 
    vpnode_t& leading, 
    vpnd_iter_t pb, 
    vpnd_iter_t pe)
{
    size_t n_terms = 1;
    for (vpnd_iter_t iter = pb; iter != pe; ++iter)
    {
        size_t n = (*iter)->plus_node()->children.size();
        n_terms += n;
    }
    vcpnode_t cleading(leading.begin(), leading.end());
    vector<vcpnode_t> pre_cloned, pre_cloned_next;
    pre_cloned.reserve(n_terms);
    pre_cloned_next.reserve(n_terms);
    pre_cloned.push_back(cleading);
    for (vpnd_iter_t iter = pb; iter != pe; ++iter)
    {
        const PlusNode* pp = (*iter)->plus_node();
        pre_cloned_next.clear();
        for (const vcpnode_t& curr: pre_cloned)
        {
            for (const BaseNode* ppc: pp->children)
            {
                vcpnode_t curr_pb(curr);
                curr_pb.push_back(ppc);
                pre_cloned_next.push_back(curr_pb);
            }
        }
        swap(pre_cloned, pre_cloned_next);
    }
    PlusNode *pp = new PlusNode;
    pp->children.reserve(n_terms);
    for (const vcpnode_t& pc: pre_cloned)
    {
        MultNode* pm = new MultNode(f.is_one() ? nullptr : new bigint32_t(f));
        vpnode_clone(pm->children, pc);
        BaseNode* pmr = pm->reduce();
        if (pmr != pm)
        {
            delete pm;
        }
        pp->children.push_back(pmr);
    }
    return pp;
}

void MultNode::set_factor(const bigint32_t& f)
{
    if (f.is_one())
    {
        delete factor;
        factor = 0;
    }
    else if (factor)
    {
        *factor = f;
    }
    else
    {
        factor = new bigint32_t(f);
    }
}

BaseNode* MultNode::reduce()
{
    reduce_children();
    sort_children(); // Inode-s, PlusNode-s, MultNode-s, SharpNode-s
    const size_t nc = children.size();
    const size_t b_indes = 0; // , b_pnodes, b_mnodes, b_
    size_t i = 0;
    for ( ; (i < nc) && children[i]->i_node(); ++i) {}
    const size_t b_pnodes = i;
    for ( ; (i < nc) && children[i]->plus_node(); ++i) {}
    const size_t b_mnodes = i;
    for ( ; (i < nc) && children[i]->mult_node(); ++i) {}
    const size_t b_snodes = i;
    const size_t n_inodes = b_pnodes - b_indes;
    const size_t n_pnodes = b_mnodes - b_pnodes;
    // const size_t n_mnodes = b_snodes - b_mnodes;
    // const size_t n_snodes = nc - b_snodes;
    vpnode_t new_children;
    bigint32_t new_factor = get_factor();
    bigint32_t tmp;
    for (i = b_indes; i < b_indes + n_inodes; ++i) // Integers
    {
        bigint32_t::mult(tmp, new_factor, children[i]->i_node()->n);
        bigint32_t::bi_swap(new_factor, tmp);
        delete children[i];
        children[i] = 0;
    }
    for (i = b_mnodes; i < b_snodes; ) // Mults
    {
        MultNode* pmb = children[i]->mult_node();
        if (pmb->factor)
        {
            bigint32_t::mult(tmp, new_factor, *(pmb->factor));
            bigint32_t::bi_swap(new_factor, tmp);
            delete pmb->factor;
            pmb->factor = 0;
        }
        MultNode* pm;
        for (++i; (i < b_snodes) && ((pm = children[i]->mult_node())) &&
            equal_children(pmb, pm); ++i)
        {
            bigint32_t::mult(tmp, new_factor, children[i]->i_node()->n);
            bigint32_t::bi_swap(new_factor, tmp);
            delete pm;
            children[i] = 0;
        }
        new_children.push_back(pmb);
    }
    set_factor(new_factor);
    for (i = b_snodes; i < nc; ) // Sharps
    {
        const size_t ib = i;
        SharpNode* psb = children[i]->sharp_node();
        SharpNode* ps = children[i]->sharp_node();
        for (++i; (i < nc) && ((ps = children[i]->sharp_node())) &&
            bn_equal(psb, ps); ++i)
        {}
        size_t n_equal = i - ib;
        if (n_equal == 1)
        {
            new_children.push_back(psb);
            children[ib] = 0;
        }
        else
        {
            MultNode* pm = new MultNode;
            pm->factor = new bigint32_t(n_equal);
            pm->children.push_back(psb);
            for (size_t idel = ib + 1; idel < i; ++idel)
            {
                delete children[idel];
                children[idel] = nullptr;
            }
            new_children.push_back(pm);
        }
    }
    BaseNode* ret = this;
    if (new_factor.is_zero())
    {
        ret = new INode; // 0
        vpnode_delete(new_children);
        children.clear();
    }
    else
    {
        if (n_pnodes == 0)
        {
            swap(children, new_children);
            sort_children();
            if (new_factor.is_one() && (children.size() == 1))
            {
                ret = children[0];
                children.clear();
            }
            else if (children.empty())
            {
                ret = new INode(new_factor);
            }
        }
        else
        {
            vpnd_iter_t plus_b = children.begin() + b_pnodes;
            vpnd_iter_t plus_e = plus_b + n_pnodes;
            ret = mult_pluses(new_factor, new_children, plus_b, plus_e);
            vpnode_delete(new_children);
            children.clear();
        }
    }
    
    return ret;
}

#if 0
BaseNode* MultNode::reduce_trivial()
{
    BaseNode* ret = this;
    const bigint32_t& factor = get_factor();
    if (factor.is_zero())
    {
        ret =  new INode; // 0
        vpnode_delete(new_children);
        children.clear();
    }
    else if (factor().is_one()) && mchildren.size() == 1)
    {
        ret = children[0];
        children.clear();
    }
    return ret;
}
#endif

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
    BaseNode* drop_invariant_number(OpNode* p);
    BaseNode* reduce_naive(BaseNode* p);
    BaseNode* sum_terms(OpNode* p);
    void add_n(bigint32_t& r, const vinode_t& a, size_t n_numbers) const;
    void mult_n(bigint32_t& r, const vinode_t& a, size_t n_numbers) const;
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
        // exp_nodes[i] = reduce_naive(exp_nodes[i]);
        BaseNode* reduced = exp_nodes[i]->reduce();
        if (exp_nodes[i] != reduced)
        {
            delete exp_nodes[i];
            exp_nodes[i] = reduced;
        }
        if (dbg_flags & 0x1) { cerr << "i="<<i << ", after reduce: " <<
             exp_nodes[i]->str() << '\n'; }
    }
    classify();
}

void BinOp::add_n(bigint32_t& r, const vinode_t& a, size_t n_numbers) const
{
    bigint32_t tmp;
    r = a[0]->n;
    for (size_t i = 1; i < n_numbers; ++i)
    {
        bigint32_t::add(tmp, r, a[i]->n);
        bigint32_t::bi_swap(tmp, r);
    }
}

void BinOp::mult_n(bigint32_t& r, const vinode_t& a, size_t n_numbers) const
{
    bigint32_t tmp;
    r = a[0]->n;
    for (size_t i = 1; i < n_numbers; ++i)
    {
        bigint32_t::mult(tmp, r, a[i]->n);
        bigint32_t::bi_swap(tmp, r);
    }
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
        const char opc = e[binop_position];
        OpNode* pp = 0;
        switch (opc)
        {
         case '+':
            pp = new PlusNode;
            break;
         case '*':
            pp = new MultNode;
            break;
         case '#':
            pp = new SharpNode;
            break;
        }
        pp->children.push_back(parse(e, sb + 1, binop_position));
        pp->children.push_back(parse(e, binop_position + 1, se - 1));
        p = pp;
    }
    return p;
}

BaseNode* BinOp::reduce_naive(BaseNode* p)
{
    BaseNode* ret = p;
    OpNode* pp = p->op_node();
    if (pp)
    {
        vpnode_t& children = pp->children;
        const size_t nc = children.size();
        for (size_t i = 0; i < nc; ++i)
        {
            children[i] = reduce_naive(children[i]);
        }
        sort(children.begin(), children.end(),
            [](const BaseNode* p0, const BaseNode* p1) -> bool
            {
                return bn_lt(p0, p1);
            });
        if (pp->op() != '#')
        {
            vinode_t pn(nc, 0);
            size_t n_numbers = 0;
            for (size_t i = 0; i < nc; ++i)
            {
                pn[i] = children[i]->i_node();
                n_numbers += (pn[i] ? 1 : 0);
            }
            if (n_numbers >= 2)
            {
                INode* iret = new INode;
                ret = iret;
                if (pp->op() == '+')
                {
                    add_n(iret->n, pn, n_numbers);
                }
                else // '*''
                {
                    mult_n(iret->n, pn, n_numbers);
                }
                if (n_numbers == nc) // all chidren are numbers
                {   
                    ret = iret;
                    delete pp;
                }
                else
                {
                    for (size_t i = 0; i < n_numbers; ++i)
                    {
                        delete children[i];
                    }
                    children[0] = iret;
                    children.erase(children.begin() + 1, 
                                   children.begin() + n_numbers);
                }
            }
            else if (pn[0]) // Just one number
            {
                if (pn[0]->n.is_zero())
                {
                    if (pp->op() == '+')
                    {
                        ret = drop_invariant_number(pp);
                    }
                    else // op == '*'
                    {
                        ret = children[0]; // zero
                        children[0] = 0; // do not delete
                        delete pp;
                    }
                }
                else if ((pp->op() == '*') && pn[0]->n.is_one())
                {
                    ret = drop_invariant_number(pp);
                }
            }
        }
    }
    return ret;
}

BaseNode* BinOp::drop_invariant_number(OpNode* p)
{
    BaseNode* ret = p;
    if (p->children.size() == 2)
    {
        ret = p->children[1];
        p->children[1] = 0;
        delete p;
    }
    else
    {
        delete p->children[0];
        p->children.erase(p->children.begin());
    }
    return ret;
}

BaseNode* BinOp::sum_terms(OpNode* p)
{
    BaseNode* ret = p;
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
