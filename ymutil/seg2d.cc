#include <algorithm>
#include <vector>
using namespace std;
typedef unsigned u_t;
typedef vector<u_t> vu_t;
typedef vector<vu_t> vvu_t;

class CBT
{
 public:
    CBT(size_t _leaves) :
        leaves(_leaves), // leaves > 0
        size(1),
        height(0)
    {
        while ((1u << height) < leaves)
        {
            ++height;
        }
        size_t width = 1u << height;
        if (width == leaves)
        {
            low_size = leaves;
            size = (1u << (height + 1)) - 1;
            low_offset = size - low_size;
            high_offset = 0;
        }
        else
        {
            size_t high_size = width - leaves;
            low_size = leaves - high_size;
            low_offset = (1u << height) - 1;
            size = low_offset + low_size;
            high_offset = low_offset - high_size;
        }
    }
    size_t leaves;
    size_t size;
    size_t height;
    size_t low_size;
    size_t low_offset;
    size_t high_offset;
};

class Seg2D
{
 public:
    Seg2D(const vu_t& _xs=vu_t(), const vu_t& _ys=vu_t());
    void add(u_t x, u_t y, u_t v);
    u_t subsum(size_t xib, size_t xie, size_t yib, size_t yie) const;
    size_t xidx(u_t x) const { return idx(xs, x); }
    size_t yidx(u_t y) const { return idx(ys, y); }
    u_t sum_below(u_t x, u_t y) const;
 private:
    size_t idx(const vu_t& a, u_t v) const;
    size_t idx_below(const vu_t& a, u_t v) const;
    u_t subrow(size_t x, size_t yib, size_t yie) const;
    vu_t xs, ys;
    CBT xcbt, ycbt;
    vvu_t trees;
};

Seg2D::Seg2D(const vu_t& _xs, const vu_t& _ys) : 
    xs(_xs), ys(_ys),
    xcbt(_xs.size()), ycbt(_ys.size()),
    trees(xcbt.size, vu_t(ycbt.size, 0))
{
}

void Seg2D::add(u_t x, u_t y, u_t v)
{
    size_t xi = xidx(x);
    size_t yi = yidx(y);
    xi = (xi < xcbt.low_size) ? xi + xcbt.low_offset
        : (xi - xcbt.low_offset) + xcbt.high_offset;
    yi = (yi < ycbt.low_size) ? yi + ycbt.low_offset
        : (yi - ycbt.low_offset) + ycbt.high_offset;
    const size_t yi0 = yi;
    for (bool xdown = true; xdown; xdown = xi > 0, xi /= 2)
    {
        yi = yi0;
        for (bool ydown = true; ydown; ydown = yi > 0, yi /= 2)
        {
            trees[xi][yi] += v;
        }
    }
}

// assuming xib < xie,  yib < yie
u_t Seg2D::subsum(size_t xib, size_t xie, size_t yib, size_t yie) const
{
    u_t ret = 0;
    if (xib + 1 == xie)
    {
        ret = subrow(xib, yib, yie);
    }
    else
    {
        const size_t mid = (xib + xie)/2;
        const u_t l = subsum(xib, mid, yib, yie);
        const u_t h = subsum(xib, mid, yib, yie);
        ret = l + h;
    }
    return ret;
}

u_t Seg2D::subrow(size_t x, size_t yib, size_t yie) const
{
    u_t ret = 0;
    if (yib + 1 == yie)
    {
        ret = subrow(xib, yib, yie);
    }
    else
    {
        const size_t mid = (yib + yie)/2;
        const u_t l = subrow(x, yib, mid);
        const u_t h = subrow(x, mid, yie);
        ret = l + h;
    }
    return ret;
}

u_t Seg2D::sum_below(u_t x, u_t y) const
{
    size_t xie = idx_below(xs, x);
    size_t yie = idx_below(ys, y);
    u_t s = subsum(0, xie, 0, yie);
    return s;
}

size_t Seg2D::idx(const vu_t& a, u_t v) const
{
    return lower_bound(a.begin(), a.end(), v) - a.begin();
}

size_t Seg2D::idx_below(const vu_t& a, u_t v) const
{
    vu_t::const_iterator iter = upper_bound(a.begin(), a.end(), v - 1);
    size_t ret = iter - a.begin();
    return ret;
}

#include <iostream>
#include <string>
int main(int argc, char **argv)
{
    size_t b = stoi(argv[1]);
    size_t e = stoi(argv[2]);
    for (size_t leaves = b; leaves < e; ++leaves)
    {
         CBT cbt(leaves);
         cout << "CNT(" << leaves << "):\n" <<
             "size=" << cbt.size << '\n' <<
             "height=" << cbt.height << '\n' <<
             "low_size=" << cbt.low_size << '\n' <<
             "low_offset=" << cbt.low_offset << '\n' <<
             "high_offset=" << cbt.high_offset << '\n';
    }
    return 0;
}
