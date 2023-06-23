#include <string>
#include <vector>
using namespace std;

// https://cp-algorithms.com/string/suffix-array.html
vector<int> sort_cyclic_shifts(string const& s) {
    int n = s.size();
    const int alphabet = 256;
    vector<int> p(n), c(n), cnt(max(alphabet, n), 0);
    for (int i = 0; i < n; i++)
        cnt[s[i]]++;
    for (int i = 1; i < alphabet; i++)
        cnt[i] += cnt[i-1];
    for (int i = 0; i < n; i++)
        p[--cnt[s[i]]] = i;
    c[p[0]] = 0;
    int classes = 1;
    for (int i = 1; i < n; i++) {
        if (s[p[i]] != s[p[i-1]])
            classes++;
        c[p[i]] = classes - 1;
    }
    vector<int> pn(n), cn(n);
    for (int h = 0; (1 << h) < n; ++h) {
        for (int i = 0; i < n; i++) {
            pn[i] = p[i] - (1 << h);
            if (pn[i] < 0)
                pn[i] += n;
        }
        fill(cnt.begin(), cnt.begin() + classes, 0);
        for (int i = 0; i < n; i++)
            cnt[c[pn[i]]]++;
        for (int i = 1; i < classes; i++)
            cnt[i] += cnt[i-1];
        for (int i = n-1; i >= 0; i--)
            p[--cnt[c[pn[i]]]] = pn[i];
        cn[p[0]] = 0;
        classes = 1;
        for (int i = 1; i < n; i++) {
            pair<int, int> cur = {c[p[i]], c[(p[i] + (1 << h)) % n]};
            pair<int, int> prev = {c[p[i-1]], c[(p[i-1] + (1 << h)) % n]};
            if (cur != prev)
                ++classes;
            cn[p[i]] = classes - 1;
        }
        c.swap(cn);
    }
    return p;
}

vector<int> suffix_array_construction(string s) {
    s += "$";
    vector<int> sorted_shifts = sort_cyclic_shifts(s);
    sorted_shifts.erase(sorted_shifts.begin());
    return sorted_shifts;
}

#include <algorithm>
#include <numeric>

typedef unsigned u_t;
typedef vector<u_t> vu_t;

// inspired by: cp-algorithms.com/string/suffix-array.html
void suffix_array_sort(vu_t& index, const string& s)
{
    const size_t n = s.size(), n1 = n + 1;
    const char cmin = (s.empty() ? 'a' : *min_element(s.begin(), s.end()) - 1);
    const char cmax = (s.empty() ? 'a' : *max_element(s.begin(), s.end()));
    const size_t alphabet = (cmax + 1) - cmin;
    vu_t p(n1), c(n1), cnt(max(alphabet, n1), 0);
    cnt[0] = 1;
    for (size_t i = 0; i < n; i++)
    {
        cnt[s[i] - cmin]++;
    }
    for (size_t i = 1; i < alphabet; i++)
    {
        cnt[i] += cnt[i - 1];
    }
    p[--cnt[0]] = n;
    for (size_t i = n; i-- > 0; )
    {
        p[--cnt[s[i] - cmin]] = i;
    }

    c[p[0]] = 0;
    u_t classes = 1;
    for (size_t i = 1; i < n1; i++)
    {
        if (s[p[i]] != s[p[i-1]])
        {
            classes++;
        }
        c[p[i]] = classes - 1;
    }
    vu_t  pn(n1), cn(n1);
    u_t p2h;
    for (u_t h = 0; (p2h = (1u << h)) < n1; ++h)
    {
        for (u_t i = 0; i < n1; i++)
        {
            pn[i] = ((p[i] + n1) - p2h) % n1;
        }
        fill(cnt.begin(), cnt.begin() + classes, 0);
        for (size_t i = 0; i < n1; i++)
        {
            cnt[c[pn[i]]]++;
        }
        for (size_t i = 1; i < classes; i++)
        {
            cnt[i] += cnt[i - 1];
        }
        for (size_t i = n1; i-- > 0;)
        {
            p[--cnt[c[pn[i]]]] = pn[i];
        }
        cn[p[0]] = 0;
        classes = 1;
        for (size_t i = 1; i < n1; i++)
        {
            pair<u_t, u_t> cur = {c[p[i]], c[(p[i] + (1 << h)) % n1]};
            pair<u_t, u_t> prev = {c[p[i - 1]], c[(p[i - 1] + (1 << h)) % n1]};
            if (cur != prev)
            {
                ++classes;
            }
            cn[p[i]] = classes - 1;
        }
        c.swap(cn);
    }
    index.clear();
    index.insert(index.end(), p.begin() + 1, p.end());
}

#include <iostream>

static void vu_print(const char* msg, const vu_t& vu)
{
    cerr << msg;
    for (u_t x: vu) { cerr << ' ' << x; }
    cerr << '\n';
}

int test_suffix_array(const string& s)
{
    vector<int> vi = suffix_array_construction(s);
    vu_t vu(vi.begin(), vi.end());
    vu_t index;
    suffix_array_sort(index, s);
    int rc = (index == vu ? 0 : 1);
    if (rc != 0)
    {
        vu_print("vu", vu);
        vu_print("index", index);
        cerr << __func__ << ": failed with " << s << '\n';
    }
    return rc;
}

static int test_recursive(string& s)
{
    static const size_t SZ_MAX = 6;
    static const size_t C_MAX = 6;
    int rc = 0;
    if (s.size() < SZ_MAX)
    {
        s.push_back(' ');
        for (u_t c = 0; (rc == 0) && (c < C_MAX); ++c)
        {
            s.back() = ('a' + c);
            rc = test_suffix_array(s);
            if (rc == 0)
            {
                rc = test_recursive(s);
            }
        }
        s.pop_back();
    }
    return rc;
}

int main(int argc, char** argv)
{
    int rc = 0;
    if (argc == 2)
    {
        rc = test_suffix_array(argv[1]);
    }
    else
    {
        string s;
        rc = test_suffix_array(s);
        rc = (rc ? : test_recursive(s));
    }
}
