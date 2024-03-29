#include <vector>
#include <algorithm>
#include <string>

using namespace std;

typedef unsigned int u_t;
typedef signed long long ll_t;
typedef unsigned long ul_t;
typedef unsigned long long ull_t;
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

#include <iostream>

template <int base_bits>
bool bb_test_specific(ll_t x, ll_t y)
{
    bool ok = true;
    const BigInt<base_bits> bx(x);
    if (bx.is_zero() != (x == 0)) { cerr << "is_zero failed, "; ok = false; }
    if (bx.is_one() != (x == 1)) { cerr << "is_one failed, "; ok = false; }
    if (x > 0)
    {
        const BigInt<base_bits> nbx(-x);
        if (nbx == bx) { cerr << "Negation failed, ", ok = false; }
        BigInt<base_bits> pbx(nbx);
        const ull_t ux = x;
        pbx.set(ux);
        if (pbx != bx) { cerr << "set(ull_t) failed, ", ok = false; }
        
    }
    const BigInt<base_bits> by(y);
    if ((bx == by) != (x == y))
    {
        cerr << "equal? failed, ";
        ok = false;
    }
    if ((bx < by) != (x < y))
    {
        cerr << "LessThan? failed, ";
        ok = false;
    }
    BigInt<base_bits> q, r;
    BigInt<base_bits>::add(r, bx, by);
    if (r.get_llt() != x + y) { cerr << "Add failed, "; ok = false; }
    BigInt<base_bits>::sub(r, bx, by);
    if (r.get_llt() != x - y) { cerr << "Sub failed, "; ok = false; }
    BigInt<base_bits>::mult(r, bx, by);
    if (r.get_llt() != x * y) { cerr << "mult failed, "; ok = false; }
    if (y != 0)
    {
        BigInt<base_bits>::divmod(q, r, bx, by);
        if (q.get_llt() != x / y) { cerr << "div failed, "; ok = false; }
        if (r.get_llt() != x % y) { cerr << "mod failed, "; ok = false; }
    }
    if (BigInt<base_bits>::lt(x, y) != (x < y))
    {
        cerr << "less-than? failed, ";
        ok = false;
    }
    if (!ok)
    {
        cerr << "base_bits="<<base_bits << ", x=" << x << ", y=" << y << '\n';
    }
    return ok;
}

bool test_specific(ll_t x, ll_t y)
{
    bool ok = true;
    ok = ok && bb_test_specific<1>(x, y);
    ok = ok && bb_test_specific<16>(x, y);
    ok = ok && bb_test_specific<32>(x, y);
    return ok;
}

bool test_some_pairs()
{
    const static ll_t hexall = 0xffff;
    static const ll_t some[] = {
        0, 1, 2, 3, 4, 5, 15, 16, 17, hexall - 1, hexall, hexall + 1};
    static u_t n_some = sizeof(some)/sizeof(some[0]);
    bool ok = true;
    for (u_t i = 0; ok && (i < n_some); ++i)
    {
        const ll_t x = some[i];
        for (u_t j = 0; ok && (j < n_some); ++j)
        {
             const ll_t y = some[j];
             ok = ok && test_specific(x, y);
             ok = ok && test_specific(-x, y);
             ok = ok && test_specific(x, -y);
             ok = ok && test_specific(-x, -y);
        }
    }
    return ok;
}

template <int base_bits>
void bb_test_two_powers()
{
    cout << "2-powers of <base_bits=" << base_bits << ">:\n";
    BigInt<base_bits> two(2);
    BigInt<base_bits> g(1), gnext;
    for (u_t p = 0; p <= 70; ++p)
    {
        cout << "2 ^ " << p << " = ";
        const string s = g.dec();
        cout << s << '\n';
        BigInt<base_bits>::mult(gnext, g, two);
        BigInt<base_bits>::bi_swap(gnext, g);
    }
}

void test_two_powers()
{
    bb_test_two_powers<1>();
    bb_test_two_powers<16>();
    bb_test_two_powers<32>();
}

template <int base_bits>
bool bb_test_sset(const string& s, u_t dbase)
{
    BigInt<base_bits> bi(s, dbase);
    const string xs = bi.strbase(dbase);
    bool ok = (xs == s);
    return ok;
}

bool test_sset(const string& s, u_t dbase)
{
    bool ok = true;    
    ok = ok && bb_test_sset<1>(s, dbase);
    ok = ok && bb_test_sset<16>(s, dbase);
    ok = ok && bb_test_sset<32>(s, dbase);
    return ok;
}

int main(int argc, char **argv)
{
    bool ok = true;
    if (argc > 2)
    {
        if (string(argv[1]) == string("s"))
        {
            ok = test_sset(string(argv[2]), stod(argv[3]));
        }
        else
        {
            const ll_t x = stol(argv[1]);
            const ll_t y = stol(argv[2]);
            ok = test_specific(x, y);
        }
    }
    else
    {
        ok = test_some_pairs();
        if (ok)
        {
             test_two_powers();
        }
    }
    return (ok ? 0 : 1);
}
