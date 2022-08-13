// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com --

#include <algorithm>
#include <array>
#include <fstream>
#include <iostream>
#include <sstream>
#include <numeric>
#include <set>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

#include <cstdlib>

using namespace std;

typedef unsigned u_t;
typedef unsigned long ul_t;
typedef long long ll_t;
typedef unsigned long long ull_t;
typedef vector<u_t> vu_t;
typedef vector<vu_t> vvu_t;
typedef vector<ull_t> vull_t;
typedef array<u_t, 2> au2_t;
typedef vector<au2_t> vau2_t;
typedef set<u_t> setu_t;
typedef vector<setu_t> vsetu_t;


// #include "bigfrac.cc" // TEMPORARY !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
#if defined(DEBUG_WITH_FRAC)
/// { bigfrac
#include <numeric>
#include <vector>
#include <algorithm>
#include <string>
#include <sstream>

using namespace std;

typedef unsigned int u_t;
typedef signed long long ll_t;
typedef unsigned long ul_t;
typedef unsigned long long ull_t;
typedef vector<u_t> vu_t;
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
    bool is_positive() const { return !(is_zero() || is_negative()); }
    bool is_negative() const { return negative; }
    void negate() { negative = !negative; }
    explicit operator double() const;
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
    bool is_positive() const { return bib.is_positive(); }
    bool is_negative() const { return bib.is_negative(); }
    bool is_one() const { return bib.is_one(); }
    void negate() { bib.negate(); }
    string strbase(u_t dbase) const { return bib.strbase(dbase); }
    string dec() const { return strbase(10); }
    string hex() const { return strbase(0x10); }
    explicit operator double() const { return double(bib); }

    static BigInt& add(BigInt& r, const BigInt& big0, const BigInt& big1)
    {
        BigIntBase::add(r.bib, big0.bib, big1.bib);
        return r;
    }
    static BigInt& sub(BigInt& r, const BigInt& big0, const BigInt& big1)
    {
        BigIntBase::sub(r.bib, big0.bib, big1.bib);
        return r;
    }
    static BigInt& mult(BigInt& r, const BigInt& big0, const BigInt& big1)
    {
        BigIntBase::mult(r.bib, big0.bib, big1.bib);
        return r;
    }
    static BigInt& divmod(BigInt& q, BigInt& r,
                          const BigInt& big0, const BigInt& big1)
    {
        BigIntBase::divmod(q.bib, r.bib, big0.bib, big1.bib);
        return q;
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

template <int base_bits>
void swap(BigInt<base_bits>& bi0, BigInt<base_bits>& bi1)
{
    BigInt<base_bits>::bi_swap(bi0, bi1);
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

BigIntBase::operator double() const
{
    double ret = 0.;
    double bp = 1.;
    for (u_t digit: digits)
    {
        double add = double(digit) * bp;
        ret += add;
        bp *= base;
    }
    return ret;
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

static u_t _gcd(u_t m, u_t n)
{
   while (n)
   {
      u_t t = n;
      n = m % n;
      m = t;
   }
   return m;
}

void choose(bigint32_t& r, u_t n, u_t k)
{
    vu_t high(vu_t::size_type(k), 0);
    iota(high.begin(), high.end(), n - k + 1);
    u_t d = 2;
    while (d <= k)
    {
        u_t dd = d;
        for (vu_t::iterator i = high.begin(), e = high.end();
            (dd > 1) && (i != e); ++i)
        {
            u_t g = _gcd(dd, *i);
            if (g > 1)
            {
                *i /= g;
                dd /= g;
            }
        }
        ++d;
    }
    r.set(1);
    for (u_t h: high)
    {
        bigint32_t m;
        bigint32_t::mult(m, r, bigint32_t(h));
        swap(r, m);
    }
}

class Frac
{
 public:
    Frac(ull_t _n=0, ull_t _d=1, int _sign=1): n(_n), d(_d),
       sign(_sign == 1 ? 1 : -1) 
    {
        reduce();
    }
    Frac(const bigint32_t &_n, const bigint32_t _d=1, int _sign=1): n(_n), d(_d),
       sign(_sign == 1 ? 1 : -1) 
    {
        reduce();
    }
    bool is_zero() const { return n.is_zero(); }
    explicit operator double() const 
    { 
        double ret = double(n) / double(d);
        if (sign != 1) { ret = -ret; }
        return ret;
    }
    static Frac& add(Frac& res, const Frac& q0, const Frac& q1)
    {
        bigint32_t qoq1d, q1q0d;
        bigint32_t::mult(qoq1d, q0.n, q1.d);
        bigint32_t::mult(q1q0d, q1.n, q0.d);
        bigint32_t::add(res.n, qoq1d, q1q0d);
        bigint32_t::mult(res.d, q0.d, q1.d);
        res.reduce();
        return res;
    };
    static Frac& sub(Frac& res, const Frac& q0, const Frac& q1)
    {
        bigint32_t qoq1d, q1q0d;
        bigint32_t::mult(qoq1d, q0.n, q1.d);
        bigint32_t::mult(q1q0d, q1.n, q0.d);
        bigint32_t::sub(res.n, qoq1d, q1q0d);
        bigint32_t::mult(res.d, q0.d, q1.d);
        res.sign = 1;
        if (res.n.is_negative())
        {
            res.n.negate();
            res.sign = -1;
        }
        res.reduce();
        return res;
    };
    static Frac& mult(Frac& res, const Frac& q0, const Frac& q1)
    {
        bigint32_t::mult(res.n, q0.n, q1.n);
        bigint32_t::mult(res.d, q0.d, q1.d);
        res.sign = (q0.sign == q1.sign ? 1 : -1);
        res.reduce();
        return res;
    };
    static Frac& div(Frac& res, const Frac& q0, const Frac& q1)
    {
        bigint32_t::mult(res.n, q0.n, q1.d);
        bigint32_t::mult(res.d, q0.d, q1.n);
        res.sign = (q0.sign == q1.sign ? 1 : -1);
        return res;
    };
    static bool lt(const Frac& x0, const Frac& x1);
    bool strset(const string& s);
    string str() const;
    string qstr() const;
    static const Frac zero, one, minus_one;
 private:
    static void gcd(bigint32_t& g, const bigint32_t& x0, const bigint32_t& x1);
    void reduce()
    {
        bigint32_t g, dummy;
        gcd(g, n, d);
        { //  n / g
            bigint32_t t(n);
            bigint32_t::divmod(n, dummy, t, g);
        }
        { //  d / g
            bigint32_t t(d);
            bigint32_t::divmod(d, dummy, t, g);
        }
    }
    bigint32_t n, d;
    int sign;
};

bool operator<(const Frac& x0, const Frac& x1)
{
    return Frac::lt(x0, x1);
}

bool Frac::lt(const Frac& x0, const Frac& x1)
{
    bool ret = false;
    if (x0.sign != x1.sign)
    {
         ret = (x1.sign > 0);
    }
    else 
    {
         bigint32_t x0nx1d, x1nx0d;
         bigint32_t::mult(x0nx1d, x0.n, x1.d);
         bigint32_t::mult(x1nx0d, x1.n, x0.d);
         if (x0nx1d != x1nx0d)
         {
             ret = (x0.sign == 1) == (x0nx1d < x1nx0d);
         }
    }
    return ret;
}

const Frac Frac::zero(0);
const Frac Frac::one(1);
const Frac Frac::minus_one(1, 1, -1);

void Frac::gcd(bigint32_t& g, const bigint32_t& _x0, const bigint32_t& _x1)
{
    bigint32_t x0(_x0);
    bigint32_t x1(_x1);
    while (!x1.is_zero())
    {
        bigint32_t q, r;
        bigint32_t::divmod(q, r, x0, x1);
        x0 = x1;
        x1 = r;
    }
    g = x0;
}

bool Frac::strset(const string& s)
{
    bool ok = true;
    string sub(s);
    sign = 1;
    if (sub[0] == '-')
    {
        sign = -1;
        sub = s.substr(1);
    }
    size_t slash = sub.find('/');
    if (slash == string::npos)
    {
        n.sset(sub, 10);
        d = 1;
    }
    else
    {
        n.sset(sub.substr(0, slash), 10);
        d.sset(sub.substr(slash + 1), 10);
    }
    ok = (d.is_positive());
    if (ok) { reduce(); }
    return ok;
}

string Frac::str() const
{
    ostringstream os;
    os << '(' << (sign == 1 ? "" : "-") << n.dec();
    if (!d.is_one())
    {
         os << '/' << d.dec();
    }
    os << ')';
    string ret = os.str();
    return ret;
}

string Frac::qstr() const
{
    ostringstream os;
    os << (sign == 1 ? "" : "-") << n.dec() << '/' << d.dec();
    string ret = os.str();
    return ret;
}
/// } bigfrac
#else // DEBUG_WITH_FRAC

// Trivial Frac
class Frac
{
 public:
    Frac(ull_t _n=0, ull_t _d=1, int _sign=1) {}
    string str() const { return "?"; }
    static Frac& add(Frac& res, const Frac& q0, const Frac& q1)
    { return res; }
    static Frac& sub(Frac& res, const Frac& q0, const Frac& q1)
    { return res; }
    static Frac& mult(Frac& res, const Frac& q0, const Frac& q1)
    { return res; }
    static Frac& div(Frac& res, const Frac& q0, const Frac& q1)
    { return res; }
    static const Frac zero, one, minus_one;
};

const Frac Frac::zero(0);
const Frac Frac::one(1);
const Frac Frac::minus_one(1, 1, -1);

#endif // ! DEBUG_WITH_FRAC

static unsigned dbg_flags;

static const ull_t MOD_BIG = 1000000000 + 7; // 1000000007

// See: https://en.wikipedia.org/wiki/Extended_Euclidean_algorithm
ll_t extended_gcd(ll_t& bezout_x, ll_t& bezout_y, const ll_t a, const ll_t b)
{
    
    ll_t s = 0, old_s = 1;
    ll_t r = b, old_r = a;
         
    while (r != 0)
    {
        lldiv_t div_result = lldiv(ll_t(old_r), ll_t(r));
        ll_t quotient = div_result.quot;
        ll_t smod = old_s - quotient * s;
        old_r = r;  r = div_result.rem;
        old_s = s;  s = smod;
    }

    bezout_x = old_s;
    bezout_y = (b ? (old_r - old_s * a) / b : 0);
    return ((a == 0) && (b == 0) ? 0 : old_r);
}

ull_t invmod(ull_t a, ull_t m)
{
    ll_t x, y;
    extended_gcd(x, y, a, m);
    ull_t inv = (x + m) % m;
    return inv;
}

ull_t n_choose_2(ull_t n)
{
    int c = 0;
    if (n >= 2)
    {
        c = (n*(n - 1))/2;
    }
    return c;
}
    
class KCount
{
 public:
    KCount(u_t _M): M(_M) { run(); }
    u_t M;
    vull_t counters;
 private:
    void run();
    ull_t components_count() const;
    vvu_t edge_priorities;
    vvu_t active;
};

void KCount::run()
{
    counters = vull_t(M/2 + 1, 0);
    u_t E = (M*(M - 1))/2;
    vau2_t ijs; ijs.reserve(E);
    for (u_t i = 0; i < M; ++i)
    {
        for (u_t j = i + 1; j < M; ++j)
        {
            ijs.push_back(au2_t{i, j});
        }
    }
    edge_priorities = vvu_t(M, vu_t(M, 0));
    active = vvu_t(M, vu_t());
    vu_t perm(E, 0);
    iota(perm.begin(), perm.end(), 0);
    ull_t Efact = 1;
    for (ull_t n = 1; n <= E; ++n) { Efact *= n; }
    if (dbg_flags & 0x1) { cerr << "M="<<M<<", E="<<E<<", Efact="<<Efact<<'\n'; }
    ull_t n = 0;
    for (bool more = true; more; 
        more = next_permutation(perm.begin(), perm.end()), ++n)
    {
        if ((dbg_flags & 0x1) && ((n & (n - 1)) == 0)) {
             cerr << "Ran " << n << " / " << Efact << " permutations\n"; }
        for (u_t p = 0; p < E; ++p) // both perm index & priority
        {
            const au2_t& ij = ijs[perm[p]];
            u_t i = ij[0], j = ij[1];
            edge_priorities[i][j] = p;
            edge_priorities[j][i] = p;
        }
        vu_t vset_adjs = vu_t(M, 0);
        for (u_t i = 0; i < M; ++i)
        {
            u_t jlink = i; // undef
            u_t pbest = 0;
            for (u_t j = 0; j < M; ++j)
            {
                if (i != j)
                {
                    if (pbest <= edge_priorities[i][j])
                    {
                         pbest = edge_priorities[i][j];
                         jlink = j;
                    }
                }
            }
            vset_adjs[i] |= (1u << jlink);
            vset_adjs[jlink] |= (1u << i);
        }
        for (u_t i = 0; i < M; ++i)
        {
            vu_t a;
            for (u_t j = 0; j < M; ++j)
            {
                if ((vset_adjs[i] & (1u << j)) != 0)
                {
                    a.push_back(j);
                }
            }
            swap(active[i], a);
        }
        ull_t cc = components_count();
        ++counters[cc];
    }
}

ull_t KCount::components_count() const
{
    ull_t n = 0;
    vector<bool> visited(M, false);
    vu_t node2c(M, 0);
    for (u_t v = 0; v < M; ++v)
    {
        if (!visited[v])
        {
            node2c[v] = n;
            setu_t q;
            q.insert(v);
            while (!q.empty())
            {
                u_t node = *q.begin();
                q.erase(q.begin());
                for (u_t a: active[node])
                {
                    if (!visited[a])
                    {
                        visited[a] = true;
                        node2c[a] = n;
                        q.insert(a);
                    }
                }
            }
            if (!active[v].empty())
            {
                ++n;
            }
        }
    }
    return n;
}

u_t sqroot(u_t n)
{
    u_t r = 0;
    if (n > 0)
    {
        r = 1;
        for (bool iter = true; iter; )
        {
            u_t d = n / r;
            u_t rmin = min(r, d);
            u_t rmax = max(r, d);
            if (rmax - rmin > 1)
            {
                u_t rnext = (r + d)/2;
                r = rnext;
            }
            else
            {
                iter = false;
                r =  (rmax * rmax <= n) ? rmax : rmin;
            }
        }
    }
    return r;
}

u_t ceil_sqroot(u_t n)
{
    u_t r = sqroot(n);
    if (r*r < n)
    {
        ++r;
    }
    return r;
}

u_t edges_get_min_vertices(u_t e)
{
    // V >= max({v: v(v-1)/2 < e}) + 1
    // solving   v^2 -v -2e < 0
    u_t x = (1 + sqroot(1 + 8*e))/2;
    if (x*(x - 1) < 2*e)
    {
        ++x;
    }
    return x;
}

class UBF
{
 public:
    UBF(ull_t _u=0, const Frac& _f=Frac::zero) : u(_u), f(_f) {}
    void nd_set(ull_t n, ull_t d)
    {
        u = (n * invmod(d, MOD_BIG)) % MOD_BIG;
        if (dbg_flags) { f = Frac(n, d); }
    }
    string str() const
    {
        ostringstream oss;
        if (dbg_flags)
        {
            oss << "{u=" << u << ", f=" << f.str() << "}";
        }
        else
        {
            oss << "?";
        }
        return oss.str();
    }
    ull_t u;
    Frac f; // for debug
};

UBF operator+(const UBF& uf0, const UBF& uf1)
{
    UBF ret{(uf0.u + uf1.u) % MOD_BIG};
    if (dbg_flags)
    {
        Frac::add(ret.f, uf0.f, uf1.f);
    }
    return ret;
}

UBF operator-(const UBF& uf0, const UBF& uf1)
{
    UBF ret{(uf0.u + (MOD_BIG - uf1.u)) % MOD_BIG};
    if (dbg_flags)
    {
        Frac::sub(ret.f, uf0.f, uf1.f);
    }
    return ret;
}

UBF operator*(const UBF& uf0, const UBF& uf1)
{
    UBF ret{(uf0.u * uf1.u) % MOD_BIG}; 
    if (dbg_flags)
    {
        Frac::mult(ret.f, uf0.f, uf1.f);
    }
    return ret;
}

UBF operator/(const UBF& uf0, const UBF& uf1)
{
    UBF ret{(uf0.u * invmod(uf1.u, MOD_BIG)) % MOD_BIG};
    if (dbg_flags)
    {
        Frac::div(ret.f, uf0.f, uf1.f);
    }
    return ret;
}

void swap(UBF& uf0, UBF& uf1)
{
    swap(uf0.u, uf1.u);
    swap(uf0.f, uf1.f);
}

typedef unordered_map<ul_t, UBF> um_u2ubf_t;

UBF ubf_factorial(ull_t n)
{
    static vector<UBF> fcache;
    if (fcache.size() <= n)
    {
        fcache.reserve(n + 1);
        if (fcache.empty())
        {
            fcache.push_back(UBF(1, 1));
        }
        for (ull_t j = fcache.size(); j <= n; ++j)
        {
            fcache.push_back(fcache.back() * UBF(j, j));
        }
    }
    return fcache[n];
}

UBF ubf_choose(ull_t n, ull_t k)
{
    UBF c(0, 0);
    if (n >= k)
    {
        c = ubf_factorial(n) / (ubf_factorial(k) * ubf_factorial(n - k));
    }
    return c;
}

class CompCountSolver
{
 public:
    CompCountSolver(u_t m, u_t k) :
        M(m), K(k), E((M*(M - 1))/2), invE(invmod(E, MOD_BIG))
    {}
    ull_t result();
    const u_t M, K, E, invE;
 private:
    void get_probability(UBF& prob, u_t i, u_t j, u_t k);
    void compute_probability(UBF& prob, u_t i, u_t j, u_t k);
    um_u2ubf_t ijk_memo;
};

ull_t CompCountSolver::result()
{
   UBF r;
   get_probability(r, E, M, K);
   if (dbg_flags & 0x1) { cerr << __func__ << ", M="<<M << ", K="<<K <<
       " P=" << r.f.str() << '\n'; }
   return r.u;
}

void CompCountSolver::get_probability(UBF& prob, u_t i, u_t j, u_t k)
{
    const ul_t key = (i << 16) | (j << 8) | k;
    um_u2ubf_t::iterator iter = ijk_memo.find(key);
    if (iter == ijk_memo.end())
    {
        prob.u = 0;
        prob.f = Frac::zero;
        if (!((i <= (j*(j - 1))/2) && (j <= 2*i)))
        {
            ; // impossible
        }
        else if (k == 0)
        {
            ; // zero
        }
        else if (i <= 1)
        {
            prob.u = ((j == 2*i) && (k == i) ? 1 : 0);
            prob.f = Frac(prob.u, 1);
        }
        else if (i > 1)
        {
            compute_probability(prob, i, j, k);
        }
        if (dbg_flags & 0x2) { cerr << __func__ <<
            "(i="<<i << ", j="<<j << ", k="<<k << ") = "<<prob.f.str() << '\n'; }
        iter = ijk_memo.insert(iter, um_u2ubf_t::value_type(key, prob));
    }
    prob = iter->second;
}


void CompCountSolver::compute_probability(UBF& prob, u_t i, u_t j, u_t k)
{
    // u_t n_edges = 0;
    UBF total; // zero initialized
    const u_t isub = i - 1;
    // const u_t jmin = (2 + ceil_sqroot(1 + 8*isub))/2;
    // const u_t jmax = min(M, 2*isub);
    // for (u_t jsub = jmin; jsub <= jmax; ++jsub)
    {
        typedef array<u_t, 4> au4_t;
        u_t ne1 = j > 2 ? n_choose_2(M - (j - 2)) : 0;
        u_t ne2 = j > 1 ? (j - 1)*(M - (j - 1)) : 0;
        u_t ne3 = n_choose_2(j) - isub;
        if (dbg_flags & 0x2) { cerr << "ijk=("<<i<<", "<<j<<", "<<k<<")" 
            ", ne1="<<ne1<<", ne2="<<ne2<<", ne3="<<ne3<<'\n'; }
        // n_edges += ne1 + ne2 + ne3;
        u_t ichoice = 0;
        for (const au4_t& nijk: {
            au4_t{ne1, isub, j - 2, k - 1},
            au4_t{ne2, isub, j - 1, k},
            au4_t{ne3, isub, j, k}})
        {
            ++ichoice;
            const u_t nsub = nijk[0], jsub = nijk[2], ksub = nijk[3];
            if (nsub > 0)
            {
                UBF prob_sub;
                get_probability(prob_sub, isub, jsub, ksub);
                if (dbg_flags & 0x4) { cerr << 
                    "ijk=("<<i<<", "<<j<<", "<<k<<") " << "ichoice="<<ichoice <<
                    ", nsub="<<nsub << ", prob_sub="<< prob_sub.f.str() <<'\n'; }
                total = total + UBF(nsub, Frac(nsub)) * prob_sub;
            }
            
        }
    }
    {
        UBF div_n_edges;
        div_n_edges.nd_set(ull_t(1), ull_t(E - isub));
        prob = total * div_n_edges;
    }
    if (dbg_flags & 0x2) { cerr << "ijk=("<<i<<", "<<j<<", "<<k<<")"
        " prob=" << prob.f.str() << '\n'; }
}

class Intranets
{
 public:
    Intranets(istream& fi);
    Intranets(u_t _M, u_t _K) : M(_M), K(_K) {}
    void solve_naive();
    void solve();
    void print_solution(ostream&) const;
    ull_t get_solution() const { return solution; }
 private:
    ull_t choose2(ull_t n) const { return (n*(n - 1)) / ull_t(2); }
    ull_t safe_choose2(ull_t n) const { return n >= 2 ? choose2(n) : 0; }
    void compute_nm_g();
    u_t M, K;
    ull_t solution;
    vector<UBF> nm_g; // [i]: number_matching * g(X), where |X|=i
};

Intranets::Intranets(istream& fi) : solution(0)
{
    fi >> M >> K;
}

void Intranets::solve_naive()
{
    UBF u_solution;
    CompCountSolver ccs(M, K);
    solution = ccs.result();
}

void Intranets::solve()
{
    compute_nm_g();
    const ull_t Mhalf = M / 2;
    UBF ubf_solution; // zero
    UBF i_choose_k(1, 1); 
    // choose(K + d), K) = (k + d) choose(K + d - 1, K)/d  where d>0
    // choose(i, K) = i*(choose(i - 1, K)/(i - K)  where i > K
    bool negate = false; 
    for (ull_t i = K, d = 0; i <= Mhalf; negate = !negate)
    {
        if (dbg_flags & 0x1) { cerr << "i=" << i << ", i_choose_k=" <<
            i_choose_k.str() << ", nm_g[i]="<<nm_g[i].str() << '\n'; }
        if (negate)
        {
            ubf_solution = ubf_solution - i_choose_k * nm_g[i];
        }
        else 
        {
            ubf_solution = ubf_solution + i_choose_k * nm_g[i];
        }
        ++i; ++d;
        i_choose_k = (UBF(i, i) * i_choose_k) / UBF(d, d);
    }
    if (dbg_flags & 0x1) { cerr << "result=" << ubf_solution.str() << '\n'; }
    solution = ubf_solution.u;
}

void Intranets::compute_nm_g()
{
    const ull_t Kmax = M/2;
    nm_g.reserve(Kmax + 1);
    nm_g.push_back(UBF(1, 1));
    // Let C2(x) = choose(x,2) if x>=2, and 0 otherwise.
    // Given 1 <= i <= M/2  and 
    // number of matchings (|X}=i):
    // N(i) = (1/(i!2^i) (M!/(M-2i))! 
    //      = (1/i!) \prod_{j=0}^{i-1} C2(M-2j)
    // g(i) = g(X) = i! \prod_{j=1}^i 1/(C(M) - C(M-2j))
    //    with considering (choose(0,2) == 0)
    // Now: NG(i) = N(i)g(X) = 
    //   (\prod_{j=0}^{i-1} C2(M-2j) (\prod_{j=1}^i 1/(C2(M) - C2(M-2j)))
    // Setting N(0)=g(0)=1, we have:
    // N(i)g(i) = 
    // = C2(M-2(i-1))*N(i - 1) * (1/(C2(M) - C2(M-2i))*g(i - 1)
    // That is:
    // NG(i) = (C2(M-2(i-1)) / C2(M) - C2(M-2i))) NG(i - 1)
    const ull_t _M = M;
    const ull_t _m_choose_2 = choose2(_M) % MOD_BIG;
    const UBF m_choose_2(_m_choose_2, Frac(_m_choose_2));
    for (ull_t i = 1; i <= Kmax; ++i)
    {
         const ull_t c2_mm2im1 = safe_choose2(_M - 2*(i - 1)) % MOD_BIG;
         const ull_t c2_mm2i = safe_choose2(_M - 2*i) % MOD_BIG;
         const ull_t denom = (_m_choose_2 + (MOD_BIG - c2_mm2i)) % MOD_BIG;
         UBF a; a.nd_set(c2_mm2im1, denom);
         nm_g.push_back(a * nm_g.back());
         if (dbg_flags & 0x1) { cerr << "nm_g["<<nm_g.size()-1 << "] = " <<
             nm_g.back().str() << '\n'; }
    }
}

void Intranets::print_solution(ostream &fo) const
{
    fo << ' ' << solution;
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

    void (Intranets::*psolve)() =
        (naive ? &Intranets::solve_naive : &Intranets::solve);
    if (solve_ver == 1) { psolve = &Intranets::solve; } // solve1
    ostream &fout = *pfo;
    ul_t fpos_last = pfi->tellg();
    for (unsigned ci = 0; ci < n_cases; ci++)
    {
        Intranets intranets(*pfi);
        getline(*pfi, ignore);
        if (tellg)
        {
            ul_t fpos = pfi->tellg();
            cerr << "Case (ci+1)="<<(ci+1) << ", tellg=[" <<
                fpos_last << " " << fpos << ") size=" <<
                (fpos - fpos_last) << "\n";
            fpos_last = fpos;
        }

        (intranets.*psolve)();
        fout << "Case #"<< ci+1 << ":";
        intranets.print_solution(fout);
        fout << "\n";
        fout.flush();
    }

    if (pfi != &cin) { delete pfi; }
    if (pfo != &cout) { delete pfo; }
    return 0;
}

static int test_count(int argc, char ** argv)
{
    int rc = 0;
    // dbg_flags |= 0x1;
    for (u_t M : {2, 3, 4, 5})
    {
        KCount kc(M);
        cout << "M = " << M << '\n';
        for (u_t i = 0; i < kc.counters.size(); ++i)
        {
            cout << "  #(" << i << " components) = " << kc.counters[i] << '\n';
        }
    }
    return rc;
}

static int test_case(u_t M, u_t K)
{
    int rc = 0;

    Intranets intranets_naive(M, K);
    intranets_naive.solve();
    ull_t solution_naive = intranets_naive.get_solution();

    Intranets intranets(M, K);
    intranets.solve();
    ull_t solution = intranets.get_solution();

    if (solution != solution_naive)
    {
        cerr << "Failed case: M=" << M << ", K=" << K << '\n';
        rc = 1;
    }
    else
    {
        cerr << "test(M="<<M << ", K="<<K << ") = " << solution << '\n'; 
    }
    return rc;
}

static int test_small(int argc, char **argv)
{
    int rc = 0;
    int ai = -1;
    int Mmin = strtoul(argv[++ai], 0, 0);
    int Mmax = strtoul(argv[++ai], 0, 0);
    int Kmin = strtoul(argv[++ai], 0, 0);
    int Kmax = strtoul(argv[++ai], 0, 0);
    cerr << "Mmin="<<Mmin << ", Mmax="<<Mmax <<
          ", Kmin="<<Kmin << ", Kmax="<<Kmax << '\n';
    for (int M = Mmin; M <= Mmax; ++M)
    {
        const int k_min = min(Kmin, M/2);
        const int k_max = min(Kmax, M/2);
        for (int K = k_min; (rc == 0) && (K <= k_max); ++K)
        {
            rc = test_case(M, K);
        }
    }
    return rc;
}

int main(int argc, char **argv)
{
    int rc = 0;
    const string test_cmd(argc > 1 ? argv[1] : "");
    if (test_cmd.size() >= 4 && test_cmd.substr(0, 4) == string("test"))
    {
        if (test_cmd == string("test_count"))
        {
            rc = test_count(argc - 2, argv + 2);
        }
        else if (test_cmd == string("test_small"))
        {
            rc = test_small(argc - 2, argv + 2);
        } 
        else 
        {
            cerr << "Bad test command: " << test_cmd << '\n';
            rc = 1;
        }
    }
    else
    {
        rc = real_main(argc, argv);
    }
    return rc;
}
