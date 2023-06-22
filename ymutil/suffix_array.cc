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

typedef unsigned u_t;
typedef vector<u_t> vu_t;

void suffix_array_sort(vu_t& index, const string& s)
{
}

#include <iostream>

int test_suffix_array(const string& s)
{
    vector<int> vi = suffix_array_construction(s);
    vu_t index;
    suffix_array_sort(index, s);
    vector<int> iidx(index.begin(), index.end());
    int rc = (iidx == vi ? 0 : 1);
    if (rc != 0)
    {
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
