// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com --

// #include <algorithm>
#include <fstream>
#include <iostream>
#include <sstream>
#include <array>
#include <string>
#include <utility>
#include <vector>

#include <cstdlib>

// #include "bigfrac.cc"
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

#if defined(TEST_BIG_FRAC)

#include <iostream>
#include <cmath>

static ull_t choose(u_t n, u_t k)
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
    ull_t r =
        accumulate(high.begin(), high.end(), 1ull, multiplies<ull_t>());
    return r;
}

static int test_choose_small()
{
    int rc = 0;
    for (u_t n = 1; (rc == 0) && (n <= 0x10); ++n)
    {
        for (u_t k = 0; (rc == 0) && (k <= n); ++k)
        {
             ull_t c_native;
             bigint32_t c_bi;
             c_native = choose(n, k);
             choose(c_bi, n, k);
             ull_t c_bi_ll = c_bi.get_llt();
             if (c_bi_ll != c_native)
             {
                 rc = 1;
                 cerr << "Inconsistent choose(" << n << ", " << k << ")\n"
                     "   native="<<c_native << ", BI="<<c_bi_ll << '\n';
             }
        }
    }
    return rc;
}

static int test_choose(int argc, char **argv)
{
    int rc = test_choose_small();
    if (rc == 0)
    {
        int ai = 0;
        const u_t n = stoi(argv[ai++]);
        const u_t k = stoi(argv[ai++]);
        bigint32_t r;
        choose(r, n, k);
        cout << "choose("<<n << ", " << k << ") = " << r.dec() << '\n';
    }
    return rc;
}

static bool almost_equal(double x0, double x1)
{
    double error = abs(x0 - x1) / (abs(x0) + abs(x1) + 1.);
    return error < 1./double(0x100);
}

static int test_double(int argc, char **argv)
{
    int rc = 0;
    int ai = 0;
    const ull_t b = stoi(argv[ai++]);
    const u_t p = stoi(argv[ai++]);
    double db = b;
    double math_pow = pow(b, p);
    cout << "db="<<db << ", p="<<p << ", math_pow="<<math_pow << '\n';

    bigint32_t b32 = b, bp = 1;
    for (u_t i = 0; i < p; ++i)
    {
        bigint32_t m;
        bigint32_t::mult(m, bp, b32);
        bp = m;
    }
    double bpd = double(bp);
    cout << "bigint32_t: bp = " << bp.dec() << ", bpd = " << bpd << '\n';

    double error = abs(bpd - math_pow) / (bpd + math_pow + 1.);
    cout << "error = " << error << '\n';
    if (error > 1./double(0x100))
    {
         rc = 1;
    }
    return rc;
}

static int test_frac(int argc, char **argv)
{
    int rc = 0;
    int ai = 0;
    const ull_t n1 = stoi(argv[ai++]);
    const ull_t d1 = stoi(argv[ai++]);
    const ull_t n2 = stoi(argv[ai++]);
    const ull_t d2 = stoi(argv[ai++]);
    double f1 = (double(n1) / double(d1));
    double f2 = (double(n2) / double(d2));
    Frac bf1(n1, d1);
    Frac bf2(n2, d2);
    if (!almost_equal(double(bf1), f1)) { rc = 1; }
    if (!almost_equal(double(bf2), f2)) { rc = 1; }
    Frac r;
    if (!almost_equal(double(Frac::add(r, bf1, bf2)), f1 + f2)) { rc = 1; }
    if (!almost_equal(double(Frac::sub(r, bf1, bf2)), f1 - f2)) { rc = 1; }
    if (!almost_equal(double(Frac::mult(r, bf1, bf2)), f1 * f2)) { rc = 1; }
    if (!almost_equal(double(Frac::div(r, bf1, bf2)), f1 / f2)) { rc = 1; }
    
    return rc;
}

int main(int argc, char **argv)
{
    int rc = 0;
    const string test(argc > 1 ? argv[1] : "");
    if (test == string("test_choose"))
    {
         rc = test_choose(argc - 2, argv + 2);
    }
    else if (test == string("test_double"))
    {
         rc = test_double(argc - 2, argv + 2);
    }
    else if (test == string("test_frac"))
    {
         rc = test_frac(argc - 2, argv + 2);
    }
    else
    {
        cerr << "Unsupported test: " << test << '\n';
        rc = 1;
    }
    return rc;
}

#endif // TEST_BIG_FRAC

using namespace std;

typedef unsigned u_t;
typedef unsigned long ul_t;
typedef unsigned long long ull_t;
typedef vector<u_t> vu_t;
typedef vector<vu_t> vvu_t;
typedef vector<string> vs_t;

static unsigned dbg_flags;

u_t gcd(u_t m, u_t n)
{
    while (n)
    {
        u_t r = m % n;
        m = n;
        n = r;
    }
    return m;
}

class Exam
{
 public:
    Exam(istream& fi);
    Exam(const vs_t& _A, const vu_t& _S) :
        N(_A.size()), Q(_A[0].size()), A(_A), S(_S)
        {}
    void solve_naive();
    void solve();
    void print_solution(ostream&) const;
    void get_solution(string& s, string& e) const 
    {
        s = solution; e = expectation;
    }
 private:
    typedef array<int, 3> ai3_t;
    typedef array<u_t, 3> au3_t;
    typedef vector<au3_t> vau3_t;
    static char ft_other(char c) { return c == 'F' ? 'T' : 'F'; }
    void solve_N1();
    void solve_N2();
    void solve_N3();
    void count_eq_neqs();
    void try_dist(u_t bits);
    void set_solution();
    string bits2str(u_t bits) const;
    u_t eq_gpos_count_choice(u_t pi, u_t bits) const;
    u_t neq_gpos3_count_choice(u_t neqi, u_t pi, u_t bits) const;
    u_t N, Q;
    vs_t A;
    vu_t S;
    string solution;
    string expectation;
    vu_t eq_idx, neq_idx[3];
    vu_t gs_possible;
    vau3_t g_dists_possible;

    Frac best_expectation;
    u_t best_dist_bits;
    // u_t g_best;
    // au3_t best_dist;
};

Exam::Exam(istream& fi)
{
    fi >> N >> Q;
    for (u_t i = 0; i < N; ++i)
    {
        string a;
        u_t score;
        fi >> a >> score;
        A.push_back(a);
        S.push_back(score);
    }
}

void Exam::solve_naive()
{
    vu_t possibles;
    for (u_t bits = 0; bits < (1u << Q); ++bits)
    {
        bool possible = true;
        for (u_t i = 0; possible && (i < N); ++i)
        {
            u_t n_same = 0;
            for (u_t j = 0; j < Q; ++j)
            {
                if (((bits & (1u << j)) != 0) == (A[i][j] == 'T'))
                {
                    ++n_same;
                }
            }
            possible = (n_same == S[i]);
        }
        if (possible)
        {
            possibles.push_back(bits);
            if (dbg_flags & 0x1) { cerr << bits2str(bits) << " possible\n"; }
        }
    }
    u_t best_bits;
    u_t best_mscore = 0;
    for (u_t bits = 0; bits < (1u << Q); ++bits)
    {
        u_t mscore = 0;
        for (u_t i = 0; i < Q; ++i)
        {
            u_t pbit = bits & (1u << i);
            for (const u_t ap: possibles)
            {
                if (pbit == (ap & (1u << i)))
                {
                    ++mscore;
                }
            }
        }
        if (dbg_flags & 0x2) { 
cerr << bits2str(bits) << ' ' << mscore << '\n'; }
        if (best_mscore < mscore)
        {
            best_mscore = mscore;
            best_bits = bits;
        }
    }
    for (u_t i = 0; i < Q; ++i)
    {
        solution.push_back(best_bits & (1u << 1) ? 'T' : 'F');
    }
    const u_t g = gcd(best_mscore, possibles.size());
    {
        ostringstream oss;
        oss << (best_mscore / g) << '/' << (possibles.size() / g);
        expectation = oss.str();
    }
}

void Exam::solve()
{
    switch (N)
    {
     case 1:
        solve_N1();
        break;
     case 2:
        solve_N2();
        break;
     case 3:
        solve_N3();
        break;
    }
}

void Exam::solve_N1()
{
    u_t e = S[0];
    if (2*S[0] >= Q)
    {
        solution = A[0];
        ostringstream oss;
    }
    else
    {
        for (char c: A[0])
        {
            solution.push_back(ft_other(c));
        }
        e = Q - S[0];
    }
    ostringstream oss;
    oss << e << "/1";
    expectation = oss.str();
}

void Exam::solve_N2()
{
    u_t n_equql = 0;
    for (u_t i = 0; i < Q; ++i)
    {
        n_equql += (A[0][i] == A[1][i] ? 1 : 0);
    }
    const u_t n_not_equal = Q - n_equql;
    
    u_t score_in_equal = (S[0] + S[1] - n_not_equal)/2;
    u_t score_in_not_equal[2];
    for (u_t i: {0, 1})
    {
        score_in_not_equal[i] = S[i] - score_in_equal;
    }
    const u_t i_better = (S[0] < S[1] ? 1 : 0);
    const u_t score_not_equal_better = score_in_not_equal[i_better];
    for (u_t i = 0; i < Q; ++i)
    {
        const char c = A[i_better][i];
        char csol = '?';
        if (A[0][i] == A[1][i])
        {
            csol = (2*score_in_equal >= n_equql) ? c : ft_other(c);
        }
        else
        {
            csol = (2*score_not_equal_better <= n_not_equal) ? ft_other(c) : c;
        }
        solution.push_back(csol);
    }
    u_t n_expect = max(score_in_equal, n_equql - score_in_equal) +
        score_not_equal_better;
    ostringstream oss;
    oss << n_expect << "/1";
    expectation = oss.str();
}

void Exam::solve_N3()
{
    count_eq_neqs();
    //  Eq + Neq0 + Neq1 + Neq2 = Q
    //  G - good guesses in Eq
    //  g[i] - good guess of in Neqi (where i is mibority)
    //  0 <= G <= Eq
    //  0 <= g[i] <= Neq[i]
    //  G + g[i] + (Neq[i+1] - g[i+1]) + (Neq[i+2] - g[i+2]) = S[i] ##  +=mod3

    // Given G
    //  [  1 -1 -1      g0     S0 - (G + N1+ N2)
    //    -1  1 -1   *  g2  =  S1 - (G + N0+ N2)
    //    -1 -1  1 ]    g2     S2 - (G + N0+ N1)
    // Inverse = [0 h h][h 0 h][h h 0]  Where  h = -1/2

    u_t n_neq = 0;
    for (u_t i = 0; i < 3; ++i) { n_neq += neq_idx[i].size(); }

    for (u_t g = 0; g <= eq_idx.size(); ++g)
    {
        bool good = true;
        // solve
        ai3_t rhs, gcand{0, 0, 0};
        for (u_t i = 0; i < 3; ++i)
        {
            rhs[i] = int(S[i]) - int(g + n_neq - neq_idx[i].size());
        }
        for (u_t i = 0; good && (i < 3); ++i)
        {
            for (u_t j = 0; j < 3; ++j)
            {
                gcand[i] += (i == j ? 0 : -rhs[j]); // *hm where h= -1/2
            }
            if (gcand[i] % 2 != 0)
            {
                cerr << "gcand["<<i<<"]="<<gcand[i] << " != 0mod2\n";
                exit(1);
            }
            gcand[i] /= 2;
            good = (0 <= gcand[i]) && (gcand[i] <= int(neq_idx[i].size()));
        }
        if (good)
        {
            au3_t ugcand{u_t(gcand[0]), u_t(gcand[1]), u_t(gcand[2])};
            if (dbg_flags & 0x1) { 
                cerr << "Possible["<< gs_possible.size() << "]=" << g <<", gi="; 
                for (u_t x: ugcand) { cerr << ' ' << x; } cerr << '\n';
            }
            gs_possible.push_back(g);
            g_dists_possible.push_back(ugcand);
        }
    }
    if (dbg_flags & 0x1) { cerr << "#(Possible)="<<gs_possible.size() << '\n'; }
    for (u_t bits = 0; bits < (1u << (3 + 1)) ; ++bits)
    {
        try_dist(bits);
    }
    set_solution();
}

void Exam::count_eq_neqs()
{
   for (size_t i = 0; i < Q; ++i)
   {
       if (A[0][i] == A[1][i])
       {
           if (A[0][i] == A[2][i])
           {
               eq_idx.push_back(i);
           }
           else 
           {
               neq_idx[2].push_back(i);
           }
       }
       else if (A[0][i] == A[2][i])
       {
            neq_idx[1].push_back(i);
       }
       else
       {
            neq_idx[0].push_back(i);
       }
   }
}

void Exam::try_dist(u_t bits)
{
    bigint32_t count, total, tmp;
    
    for (u_t pi = 0, pe = gs_possible.size(); pi < pe; ++pi)
    {
        const u_t g_possible = gs_possible[pi];
        const au3_t& g3_possible = g_dists_possible[pi];
        u_t sum_comb = eq_gpos_count_choice(pi, bits);
        bigint32_t eq_choose, neq_choose3[3];
        choose(eq_choose, eq_idx.size(), g_possible);
        bigint32_t m(eq_choose);
        for (u_t i: {0, 1, 2})
        {
            sum_comb += neq_gpos3_count_choice(i, pi, bits);
            choose(neq_choose3[i], neq_idx[i].size(), g3_possible[i]);
            bigint32_t::mult(tmp, m, neq_choose3[i]);
            swap(m, tmp);
        }
        bigint32_t possible_exp;
        bigint32_t::mult(possible_exp, m, bigint32_t(sum_comb));
        bigint32_t::add(tmp, total, possible_exp);
        swap(total, tmp);
        
        bigint32_t::add(tmp, count, m);
        swap(count, tmp);
    }
    Frac dist_exp(total, count);
    if (best_expectation < dist_exp)
    {
         best_expectation = dist_exp;
         best_dist_bits = bits;
    }
}

u_t Exam::eq_gpos_count_choice(u_t pi, u_t bits) const
{
    const u_t g_possible = gs_possible[pi];
    const u_t n = ((bits & (1u << 3)) != 0)
        ? g_possible : eq_idx.size() - g_possible;
    return n;
}

u_t Exam::neq_gpos3_count_choice(u_t neqi, u_t pi, u_t bits) const
{
    const u_t g3p = g_dists_possible[pi][neqi];
    const u_t neqsz = neq_idx[neqi].size();
    const u_t n = ((bits & (1u << neqi)) != 0) ? g3p : neqsz - g3p;
    return n;
}

void Exam::set_solution()
{
    vector<bool>  vb(Q, false);
    solution = string(Q, '?');
    for (u_t i: eq_idx)
    {
        const char ft = A[0][i];
        solution[i] = ((best_dist_bits & (1u << 3)) != 0) ? ft : ft_other(ft);
    }
    for (u_t i: {0, 1, 2})
    {
        const bool asis = ((best_dist_bits & (1u << i)) != 0);
        for (u_t j: neq_idx[i])
        {
            const char ft = A[i][j];
            solution[j] = asis ? ft : ft_other(ft);
        }
    }
    expectation = best_expectation.qstr();
}

string Exam::bits2str(u_t bits) const
{
    string ret;
    for (u_t j = 0; j < Q; ++j)
    {
        char ft = "FT"[((bits & (1u<<j)) != 0) ? 1 : 0];
        ret.push_back(ft);
    }
    return ret;
}

void Exam::print_solution(ostream &fo) const
{
    fo << ' ' << solution << ' ' << expectation;
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

    void (Exam::*psolve)() =
        (naive ? &Exam::solve_naive : &Exam::solve);
    if (solve_ver == 1) { psolve = &Exam::solve; } // solve1
    ostream &fout = *pfo;
    ul_t fpos_last = pfi->tellg();
    for (unsigned ci = 0; ci < n_cases; ci++)
    {
        Exam exam(*pfi);
        getline(*pfi, ignore);
        if (tellg)
        {
            ul_t fpos = pfi->tellg();
            cerr << "Case (ci+1)="<<(ci+1) << ", tellg=[" <<
                fpos_last << " " << fpos << ") size=" <<
                (fpos - fpos_last) << "\n";
            fpos_last = fpos;
        }

        (exam.*psolve)();
        fout << "Case #"<< ci+1 << ":";
        exam.print_solution(fout);
        fout << "\n";
        fout.flush();
    }

    if (pfi != &cin) { delete pfi; }
    if (pfo != &cout) { delete pfo; }
    return 0;
}

static int test_case(const vs_t& A, const vu_t& S, const string& Atrue)
{
    int rc = 0;
    Exam exam_naive(A, S);
    Exam exam(A, S);
    exam_naive.solve_naive();
    exam.solve();
    string solution_naive, expectation_naive;
    string solution, expectation;
    exam_naive.get_solution(solution_naive, expectation_naive);
    exam.get_solution(solution, expectation);
    if (dbg_flags & 0x4)
    { 
        for (u_t i = 0; i < A.size(); ++i)
        {
            cerr << " [" << i << "];  " << A[i] << ' ' << S[i] << '\n';
        }
        cerr << "expectation = " << expectation << '\n' << 
            "Atrue: " << Atrue << '\n' <<
            "naive: " << solution_naive << '\n' <<
            "real:  " << solution << '\n';
    }
    if (expectation != expectation_naive)
    {
        rc = 1;
        cerr << "Inconsistent expectation: naive=" << expectation_naive <<
            ", real=" << expectation << '\n';
        ofstream f("exam-fail.in");
        f << "1\n" << A.size() << ' ' << A[0].size() << '\n';
        for (size_t i = 0; i < A.size(); ++i)
        {
            f << A[i] << ' ' << S[i] << '\n';
        }
        f.close();
    }
    return rc;
}

static u_t rand_range(u_t low, u_t high)
{
    u_t ret = low + rand() % (high + 1 - low);
    return ret;
}

string rand_ft(u_t sz)
{
    string ft;
    while (ft.size() < sz)
    {
        ft.push_back("FT"[rand() % 2]);
    }
    return ft;
}

static int test(int argc, char ** argv)
{
    int rc = 0;
    int ai = 0;
    u_t nt = stod(argv[ai++]);
    u_t Nmin = stod(argv[ai++]); // 1,2,3
    u_t Nmax = stod(argv[ai++]); // 1,2,3
    u_t Qmin = stod(argv[ai++]);
    u_t Qmax = stod(argv[ai++]);
    dbg_flags = (ai < argc ? strtoul(argv[ai++], 0, 0) : 0);
    for (u_t ti = 0; (rc == 0) && (ti < nt); ++ti)
    {
        cerr << "tested << " << ti << '/' << nt << '\n';
        u_t N = rand_range(Nmin, Nmax);
        u_t Q = rand_range(Qmin, Qmax);
        vs_t A;
        vu_t S;
        const string Atrue = rand_ft(Q);
        while (A.size() < N)
        {
            const string a = rand_ft(Q);
            u_t score = 0;
            for (u_t i = 0; i < Q; ++i)
            {
                score += (a[i] == Atrue[i] ? 1 : 0);
            }
            A.push_back(a);
            S.push_back(score);
        }
        rc = test_case(A, S, Atrue);
        if (rc != 0) { cerr << "Atrue = " << Atrue << '\n'; }
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
