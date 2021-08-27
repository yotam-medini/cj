// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com --

#include <algorithm>
#include <fstream>
#include <iostream>
#include <iterator>
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

// #include "bigint.h"
///////////////////////////////// {
#include <vector>
#include <algorithm>
#include <string>

using namespace std;

typedef signed long long ll_t;
typedef vector<ull_t> vull_t;

int most_significant_bit(ull_t x)
{
    int msb = -1;
    if (x)
    {
        msb = 0;
        u_t mask_bits = 32;
        ull_t mask = (ull_t(1) << mask_bits) - 1; // 32 bits on
        while (mask)
        {
            if (x & (mask << (msb + mask_bits)))
            {
                msb += mask_bits;
            }
            mask_bits /= 2;
            mask >>= (mask_bits ? : 1);
        }
    }
    return msb;
}

class BigIntBase
{
 public:
    BigIntBase(ull_t _base, const vull_t &_digits=vull_t(), 
        bool _negative=false) :
        base(_base), digits(_digits), negative(_negative)
        {}
    ll_t get_llt() const;
    void set(ull_t v);
    void set_vneg(ull_t v, bool _negative=false)
    {
        negative = _negative;
        set(v);
    }
    void sset(const string& s, u_t dbase);
    bool is_zero() const { return digits.empty(); }
    bool is_one() const { return (digits.size() == 1) && (digits[0] == 1); }
    bool is_negative() const { return negative; }
    string strbase(u_t dbase) const;
    static void add(BigIntBase& r, 
                    const BigIntBase& big0, const BigIntBase& big1);
    static void sub(BigIntBase& r,
                    const BigIntBase& big0, const BigIntBase& big1);
    static void mult(BigIntBase& r, 
                     const BigIntBase& big0, const BigIntBase& big1);
    static void divmod(BigIntBase& q, BigIntBase& r,
                       const BigIntBase& u, const BigIntBase& v);
    static bool eq(const BigIntBase& big0, const BigIntBase& big1)
    {
        return (big0.negative == big1.negative) &&
            (big0.digits == big1.digits);
    }
    static void bib_swap(BigIntBase& big0, BigIntBase& big1)
    {
        swap(big0.digits, big1.digits);
        swap(big0.negative, big1.negative);
    }
 private:
    static void add_abs(BigIntBase& r, 
        const BigIntBase& big0, const BigIntBase& big1);
    static void sub_abs(BigIntBase& r,
        const BigIntBase& big0, const BigIntBase& big1);
    static void divmod_digit(BigIntBase& q, BigIntBase& r, 
                             const BigIntBase& u, ull_t v);
    static void knuth_divmod(BigIntBase& q, BigIntBase& r, 
                             const BigIntBase& u, const BigIntBase& v);
    static bool lt_abs(const BigIntBase& big0, const BigIntBase& big1);
    void pop0()
    {
        if (digits.back() == 0)
        {
            digits.pop_back();
        }
    }
    void pop0s()
    {
        while ((!digits.empty()) && (digits.back() == 0))
        {
            digits.pop_back();
        }
    }
    ull_t base;
    vull_t digits;
    bool negative;
};

template<int base_bits>
class BigInt
{
 public:
    static ull_t base() 
    {
        ull_t ret = ull_t(1) << base_bits; 
        return ret;
    }
    BigInt(const vull_t &_digits, bool _negative) :
        bib(base(), _digits, _negative)
        {}
    BigInt(ll_t v=0) : bib(base(), vull_t(), (v < 0))
    {
        bib.set(v >= 0 ? v : -v);
    }
    BigInt(const string& s, u_t dbase) : bib(base())
    {
        sset(s, dbase);
    }
    void set(ull_t v)
    {
        bib.set_vneg(v);
    }
    void sset(const string& s, u_t dbase)
    {
        bib.sset(s, dbase);
    }
    ll_t get_llt() const { return bib.get_llt(); }
    bool is_zero() const { return bib.is_zero(); }
    bool is_one() const { return bib.is_one(); }
    string strbase(u_t dbase) const { return bib.strbase(dbase); }
    string dec() const { return strbase(10); }
    string hex() const { return strbase(0x10); }

    static void add(BigInt& r, const BigInt& big0, const BigInt& big1)
    {
        BigIntBase::add(r.bib, big0.bib, big1.bib);
    }
    static void sub(BigInt& r, const BigInt& big0, const BigInt& big1)
    {
        BigIntBase::sub(r.bib, big0.bib, big1.bib);
    }
    static void mult(BigInt& r, const BigInt& big0, const BigInt& big1)
    {
        BigIntBase::mult(r.bib, big0.bib, big1.bib);
    }
    static void divmod(BigInt& q, BigInt& r,
                       const BigInt& big0, const BigInt& big1)
    {
        BigIntBase::divmod(q.bib, r.bib, big0.bib, big1.bib);
    }
    static bool eq(const BigInt& big0, const BigInt& big1)
    {
        return BigIntBase::eq(big0.bib, big1.bib);
    }
    static bool lt(const BigInt& big0, const BigInt& big1)
    {
        BigInt delta;
        sub(delta, big1, big0);
        bool is_lt = !(delta.bib.is_negative() || delta.is_zero());
        return is_lt;
    }
    static void bi_swap(BigInt& big0, BigInt& big1)
    {
        BigIntBase::bib_swap(big0.bib, big1.bib);
    }
 private:
    BigIntBase bib;
};

template <int base_bits>
bool operator==(const BigInt<base_bits>& bi0, const BigInt<base_bits>& bi1)
{
    bool eq = BigInt<base_bits>::eq(bi0, bi1);
    return eq;
}

template <int base_bits>
bool operator!=(const BigInt<base_bits>& bi0, const BigInt<base_bits>& bi1)
{
    return !(bi0 == bi1);
}

template <int base_bits>
bool operator<(const BigInt<base_bits>& bi0, const BigInt<base_bits>& bi1)
{
    bool lt = BigInt<base_bits>::lt(bi0, bi1);
    return lt;
}

typedef BigInt<1> bigint1_t;
typedef BigInt<16> bigint16_t;
typedef BigInt<32> bigint32_t;

string BigIntBase::strbase(u_t dbase) const
{
    string s;
    if (is_zero())
    {
        s = "0";
    }
    else
    {
        if (negative)
        {
            s = string("-");
        }
        BigIntBase n(*this); n.negative = false;
        BigIntBase d(base); d.set(base); // const like
        BigIntBase q(base), rdigit(base);;
        while (!n.is_zero())
        {
            divmod_digit(q, rdigit, n, dbase);
            ull_t idigit = rdigit.get_llt();
            char digit = "0123456789abcdef"[idigit];
            s.push_back(digit);
            swap(n, q);
        }
        // mirror
        for (size_t i = 0, j = s.size() - 1; i < j; ++i, --j)
        {
            swap(s[i], s[j]);
        }
    }
    return s;
}

void BigIntBase::set(ull_t v)
{
    digits.clear();
    while (v > 0)
    {
        digits.push_back(v % base);
        v /= base;
    }
}

void BigIntBase::sset(const string& s, u_t dbase)
{
    BigIntBase bib_db(base); bib_db.set(dbase);
    BigIntBase total(base), tmp(base);
    const bool neg = (s[0] == '-');
    for (u_t i = neg ? 1 : 0; i < s.size(); ++i)
    {
        static const char* hexdigits = "0123456789abcdef";
        mult(tmp, total, bib_db);
        const char cdigit = s[i];
        u_t digit = 0;
        while ((digit < dbase) && (hexdigits[digit] != cdigit))
        {
            ++digit;
        }
        BigIntBase bib_digit(base); bib_digit.set(digit);
        add(total, tmp, bib_digit);
    }
    bib_swap(*this, total);
    negative = neg;
}

ll_t BigIntBase::get_llt() const
{
    ull_t ret = 0;
    ull_t bp = 1;
    for (ull_t d: digits)
    {
        ull_t add = d * bp;
        bp *= base;
        ret += add;
    }
    if (negative)
    {
        ret = -ret;
    }
    return ret;
}

void BigIntBase::add(
    BigIntBase& r, const BigIntBase& big0, const BigIntBase& big1)
{
    if (big0.negative == big1.negative)
    {
        r.negative = big0.negative;
        add_abs(r, big0, big1);
    }
    else
    {
        if (lt_abs(big0, big1))
        {
            sub_abs(r, big1, big0);
            r.negative = big1.negative;
        }
        else
        {
            sub_abs(r, big0, big1);
            r.negative = big0.negative;
        }
        
    }
}

void BigIntBase::sub(
    BigIntBase& r, const BigIntBase& big0, const BigIntBase& big1)
{
    if (big0.negative == big1.negative)
    {
        r.digits.clear();
        if (lt_abs(big0, big1))
        {
            sub_abs(r, big1, big0);
            r.negative = !big1.negative;
        }
        else
        {
            sub_abs(r, big0, big1);
            r.negative = big0.negative;
        }
    }
    else
    {
        add_abs(r, big0, big1);
        r.negative = big0.negative;
    }
}

void BigIntBase::mult(
    BigIntBase& r, const BigIntBase& big0, const BigIntBase& big1)
{
    r.set(0);
    r.negative = (big0.negative != big1.negative);
    BigIntBase rold(r);
    for (size_t id1 = 0; id1 < big1.digits.size(); ++id1)
    {
        const ull_t digit1 = big1.digits[id1];
        // BigIntBase& a = add[ia];
        // BigIntBase p(big0.__base);
        // p.digits.reserve(big0.digits.size() + id1 + 1);
        BigIntBase a(big0.base);
        a.digits.reserve(big0.digits.size() + id1 + 1);
        a.digits.insert(a.digits.begin(), size_t(id1), ull_t(0));
        ull_t carry = 0;
        for (size_t id0 = 0; id0 < big0.digits.size(); ++id0)
        {
            ull_t p = digit1 * big0.digits[id0] + carry;
            a.digits.push_back(p % a.base);
            carry = p / a.base;
        }
        if (carry)
        {
            a.digits.push_back(carry);
        }
        a.pop0s();
        bib_swap(r, rold);
        add_abs(r, rold, a);        
    }
}

void BigIntBase::divmod(BigIntBase& q, BigIntBase& r,
                        const BigIntBase& u, const BigIntBase& v)
{
    r = u;
    q.set(0);
    q.negative = (u.negative != v.negative);
    if (!lt_abs(u, v))
    {
        if (v.digits.size() == 1)
        {
            divmod_digit(q, r, u, v.digits[0]);
        }
        else
        {
            knuth_divmod(q, r, u, v);
        }
    }
}

void BigIntBase::add_abs(
    BigIntBase& r, const BigIntBase& big0, const BigIntBase& big1)
{
    r.digits.clear();
    const u_t sz0 = big0.digits.size();
    const u_t sz1 = big1.digits.size();
    const u_t min_sz = min(sz0, sz1);
    const u_t max_sz = max(sz0, sz1);
    r.digits.reserve(max_sz + 1);
    u_t i, carry = 0;
    for (i = 0; i < min_sz; ++i)
    {
        ull_t a = big0.digits[i] + big1.digits[i] + carry;
        if ((carry = (a >= big0.base)))
        {
            a -= big0.base;                
        }
        r.digits.push_back(a);
    }
    const vull_t& digits = (sz0 > sz1 ? big0.digits : big1.digits);
    for ( ; i < max_sz; ++i)
    {
        ull_t a = digits[i] + carry;
        if ((carry = (a >= big0.base)))
        {
            a -= big0.base;                
        }
        r.digits.push_back(a);
    }
    if (carry)
    {
        r.digits.push_back(1);
    }
}

// Assuming |big0| >= |big1|
void BigIntBase::sub_abs(
    BigIntBase& r, const BigIntBase& big0, const BigIntBase& big1)
{
    r.digits.clear();
    r.digits.reserve(big1.digits.size());
    const u_t sz0 = big0.digits.size();
    const u_t sz1 = big1.digits.size();
    u_t i, borrow = 0;
    for (i = 0; i < sz1; ++i)
    {
        ull_t a = 0;
        if (big0.digits[i] >= big1.digits[i] + borrow)
        {
            a = big0.digits[i] - (big1.digits[i] + borrow);
            borrow = 0;
        }
        else
        {
            a = (r.base + big0.digits[i]) - (big1.digits[i] + borrow);
            borrow = 1;
        }
        r.digits.push_back(a);
    }
    for (; i < sz0; ++i)
    {
        ull_t a = 0;
        if (big0.digits[i] >= borrow)
        {
            a = big0.digits[i] - borrow;
            borrow = 0;
        }
        else
        {
            a = (r.base + big0.digits[i]) - borrow;
            borrow = 1;
        }
        r.digits.push_back(a);
    }
    r.pop0s();
}

bool BigIntBase::lt_abs(const BigIntBase& big0, const BigIntBase& big1)
{
    const u_t sz0 = big0.digits.size();
    const u_t sz1 = big1.digits.size();
    bool lt = (sz0 < sz1);
    if ((!lt) && (sz0 == sz1))
    {
        int i = sz0 - 1;
        for ( ; (i >= 0) && (big0.digits[i] == big1.digits[i]); --i)
        {
        }
        lt = (i >= 0) && (big0.digits[i] < big1.digits[i]);
    }
    return lt;
}

void BigIntBase:: divmod_digit(BigIntBase& q, BigIntBase& r, 
                               const BigIntBase& u, ull_t v)
{
    // ull_t rdigit = u.digits.back();
    ull_t rdigit = 0;;
    size_t n_digits = u.digits.size();
    if (u.digits.back() < v)
    {
        --n_digits;
        rdigit = u.digits.back();
    }
    q.digits.clear();
    q.digits.insert(q.digits.end(), n_digits, 0);
    for (size_t i = n_digits; i-- > 0; )
    {
        rdigit = u.base * rdigit + u.digits[i];
        q.digits[i] = rdigit / v;
        rdigit = rdigit % v;
    }
    q.pop0s();
    r.set(rdigit);
}

void BigIntBase::knuth_divmod(BigIntBase& q, BigIntBase& r, 
                              const BigIntBase& u, const BigIntBase& v)
{
    // D1. Normalize
    const size_t n = v.digits.size();
    const size_t m = u.digits.size() - n; // m + n == u.digits.size()
    q.digits.clear();
    q.digits.insert(q.digits.end(), size_t(m + 1), 0);
    ull_t dd = (u.base - 1) / v.digits.back();
    {
        ull_t dd_alt = ull_t(1) << most_significant_bit(dd);
        if (dd_alt * v.digits.back() >= u.base / 2)
        {
            dd = dd_alt;
        }
    }
    BigIntBase d(u.base, vull_t(size_t(1), dd));
    BigIntBase u_norm(u.base), v_norm(u.base);
    mult(u_norm, u, d);
    mult(v_norm, v, d);
    if (u_norm.digits.size() == m + n)
    {
        u_norm.digits.push_back(0);
    }

    const ull_t v_nm1 = v_norm.digits[n - 1];
    const ull_t v_nm2 = v_norm.digits[n - 2];
    // D2. Inititialize j --> 7. Loop on J.
    for (int j = m; j >= 0; --j)
    {
        // D3. Calculate q.
        ull_t num = u_norm.digits[j + n] * u.base + u_norm.digits[j + n - 1];
        ull_t qt = num / v_nm1;
        ull_t rt = num % v_nm1;
        if ((qt == u.base) || 
            (qt * v_nm2 > u.base * rt + u_norm.digits[j + n - 2]))
        {
            --qt;
            rt += v_nm1;
            if ((rt < u.base) && 
                (qt * v_nm2 > u.base * rt + u_norm.digits[j + n - 2]))
            {
                --qt;
                rt += v_nm1;
            }
        }
        // D4. Multiply and subtract.
        vull_t::const_iterator udv = u_norm.digits.begin();
        const vull_t top_digits(udv + j, udv + j + n + 1);
        BigIntBase utop(u.base, top_digits);
        utop.pop0();
        BigIntBase qt_bib(u.base, vull_t(size_t(1), qt));
        BigIntBase u_sub(u.base), qv(u.base);
        mult(qv, v_norm, qt_bib);
        // D5. set reminder.
        if (lt_abs(utop, qv))
        {
            // D6. Add back.
            --qt;
            qt_bib.set(qt);
            mult(qv, v_norm, qt_bib);
        }
        // D4. continuation...
        sub_abs(u_sub, utop, qv);
        copy(u_sub.digits.begin(), u_sub.digits.begin() + n + 1,
             u_norm.digits.begin() + j);
        q.digits[j] = qt;
    }
    // D8. Unnormalize.
    u_norm.digits.erase(u_norm.digits.begin() + n, u_norm.digits.end());
    BigIntBase r_ignore(u.base);
    divmod_digit(r, r_ignore, u_norm, dd);
    q.pop0();
}

///////////////////////////////// }

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
    virtual void seed_eval(bigint32_t& r, ull_t seed) const = 0;
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
    void seed_eval(bigint32_t& r, ull_t seed) const
    { r = n; }
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
 private:
    void flatten_pluses();
    void add_mults(
        vpnode_t& new_children, 
        size_t& isharp_into_mult,
        size_t imult, 
        size_t isharp);
    void add_sharps(vpnode_t& new_children, size_t isharp, size_t ncs);
    bigint32_t n;
    void seed_eval(bigint32_t& r, ull_t seed) const;
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
    void seed_eval(bigint32_t& r, ull_t seed) const;
    static const bigint32_t big_one;
    bigint32_t* factor;
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
    void seed_eval(bigint32_t& r, ull_t seed) const;
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
        else
        {
            const MultNode* pm = mult_node();
            if (pm && pm->factor)
            {
                s += pm->factor->dec() + string("*");
            }
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

size_t filter_out_nulls(vpnode_t& a, size_t i0)
{
    const size_t sz = a.size();
    for (size_t i1 = i0; i1 < sz; ++i1)
    {
        if (a[i1])
        {
            a[i0++] = a[i1];
        }
    }
    a.erase(a.begin() + i0, a.end());
    return i0;
}

void PlusNode::flatten_pluses()
{
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
}

void PlusNode::add_mults(
    vpnode_t& new_children, 
    size_t& isharp_into_mult,
    size_t imult, 
    size_t isharp)
{
    const size_t nc = children.size();
    bigint32_t tsum;
    for ( ; imult < isharp;)
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
}

void PlusNode::add_sharps(vpnode_t& new_children, size_t isharp, size_t ncs)
{
    for ( ; isharp < ncs; ) // those who were not joind to MultNode-s.
    {
        const size_t isharp_b = isharp;
        SharpNode* psb = children[isharp_b]->sharp_node();
        for (++isharp; (isharp < ncs) && 
            bn_equal(psb, children[isharp]->sharp_node()); ++isharp) {}
        u_t n_eq = isharp - isharp_b;
        if (n_eq > 1)
        {
            MultNode* pm = new MultNode(new bigint32_t(n_eq));
            pm->children.push_back(psb);
            new_children.push_back(pm);
            children[isharp_b] = nullptr;
            for (size_t idel = isharp_b + 1; idel < ncs; ++idel)
            {
                delete children[idel];
                children[idel] = nullptr;
            }
        }
        else
        {
            new_children.push_back(psb);
        }
    }
}

BaseNode* PlusNode::reduce()
{
    reduce_children();
    flatten_pluses(); // after taht, no more pluses
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
    size_t imult = i, isharp = i;
    for ( ; (isharp < nc) && !children[isharp]->sharp_node(); ++isharp) {}
    size_t isharp_into_mult = isharp;
    // MultNode-s are multiplications of SharpNode-s
    add_mults(new_children, isharp_into_mult, imult, isharp);
    const size_t ncs = filter_out_nulls(children, isharp);
    add_sharps(new_children, isharp, ncs);
    swap(children, new_children);
    sort_children();
    BaseNode* ret = this;
    if (children.size() > 1)
    {
        INode* pn = children[0]->i_node();
        if (pn && pn->n.is_zero())
        {
            delete pn;
            for (size_t ci = 0, ci1 = 1; ci1 < children.size(); ci = ci1++)
            {
                 children[ci] = children[ci1];
            }
            children.pop_back();
        }
    }
    if (children.size() == 1)
    {
        ret = children[0];
        children.clear();
    }
    return ret;
}

void PlusNode::seed_eval(bigint32_t& r, ull_t seed) const
{
    bigint32_t cr, t;
    r.set(0);
    for (const BaseNode* c: children)
    {
        c->seed_eval(cr, seed);
        bigint32_t::add(t, r, cr);
        bigint32_t::bi_swap(r, t);
    }
}

BaseNode* mult_pluses(
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
    BaseNode* ppr = pp->reduce();
    if (ppr != pp)
    {
        delete pp;
    }
    return ppr;
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

void MultNode::seed_eval(bigint32_t& r, ull_t seed) const
{
    bigint32_t cr, t;
    r = get_factor();
    for (const BaseNode* c: children)
    {
        c->seed_eval(cr, seed);
        bigint32_t::mult(t, r, cr);
        bigint32_t::bi_swap(r, t);
    }
}

void SharpNode::seed_eval(bigint32_t& r, ull_t seed) const
{
    bigint32_t x, y;
    children[0]->seed_eval(x, seed);
    children[1]->seed_eval(y, seed);
    bigint32_t xx, yy, xy;
    bigint32_t::mult(xx, x, x);
    bigint32_t::mult(yy, y, y);
    bigint32_t::mult(xy, x, y);

    bigint32_t a[6];
    for (u_t i = 0; i < 6; ++i)
    {
        a[i].set(seed % 4);
        seed /= 4;
    }

    bigint32_t rold, t;
    bigint32_t::mult(r, a[0], x);
    bigint32_t::mult(t, a[1], y);
    rold = r;
    bigint32_t::add(r, rold, t);

    bigint32_t::mult(t, a[2], xx);
    rold = r;
    bigint32_t::add(r, rold, t);

    bigint32_t::mult(t, a[3], yy);
    rold = r;
    bigint32_t::add(r, rold, t);

    bigint32_t::mult(t, a[4], xy);
    rold = r;
    bigint32_t::add(r, rold, t);

    rold = r;
    bigint32_t::add(r, rold, a[5]);
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
    typedef vector<bigint32_t> vbigint32_t;
    typedef vector<vbigint32_t> vvbigint32_t;
    void parse_expressions();
    BaseNode* parse(const string& e, size_t sb, size_t se) const;
    void naive_classify();
    void classify();
    static const bigint32_t big_one;
    u_t N;
    vs_t expressions;
    vu_t solution;
    vpnode_t exp_nodes;
    vvbigint32_t exp_lin_evals;
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
    exp_lin_evals = vvbigint32_t(size_t(N), vbigint32_t());
    static const u_t Nvals = 0x1000;
    for (u_t i = 0; i < N; ++i)
    {
        exp_lin_evals[i].reserve(Nvals);
    }
    for (u_t seed = 0; seed < Nvals; ++seed)
    {
        for (u_t i = 0; i < N; ++i)
        {
            bigint32_t r;
            exp_nodes[i]->seed_eval(r, seed);
            exp_lin_evals[i].push_back(r);
        }
    }
    if (dbg_flags & 0x2) {
        for (u_t i = 0; i < N; ++i) {
            cerr << "["<<i<<"] " << expressions[i] << " :=>";
            for (const bigint32_t& v: exp_lin_evals[i]) {
                cerr << ' ' << v.dec(); }
            cerr << '\n';
        }
    }
    naive_classify();
}

void BinOp::solve()
{
    parse_expressions();
    for (size_t i = 0; i < N; ++i)
    {
        if (dbg_flags & 0x1) { cerr << "i="<<i << ", before reduce: " <<
            exp_nodes[i]->str() << '\n'; }
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

void BinOp::naive_classify()
{
    vu_t class2idx;

    solution.push_back(0);
    class2idx.push_back(0);
    
    for (size_t ei = 1; ei < N; ++ei)
    {
        const vbigint32_t& e = exp_lin_evals[ei];
        const size_t nc = class2idx.size();
        size_t ci_match = nc; // undefined
        for (size_t ci = 0; (ci < nc) && (ci_match == nc); ++ci)
        {
            size_t ei_first = class2idx[ci];
            if (e == exp_lin_evals[ei_first])
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
