// Knuth Morris Pratt algorithm

#include <string>
#include <vector>

using namespace std;

class KMP_Ref
{
 public:
    KMP_Ref(size_t _i=0, size_t _q=0) : i(_i), q(_q) {}
    size_t i;
    size_t q;
};

template <typename Container>
class KMP
{
 public:
    typedef Container container_t;
    KMP(const container_t& _pattern, const container_t& _text) :
        pattern(_pattern), text(_text)
        {
            build_pi();
        }
    size_t search(KMP_Ref& ref) const;
 private:
    size_t text_size() const { return text.size(); }
    size_t pattern_size() const { return pattern.size(); }
    void build_pi();
    container_t pattern;
    container_t text;
    vector<size_t> pi;
};  

template <typename Container>
void KMP<Container>::build_pi()
{
    const size_t m = pattern.size();
    pi.assign(m + 2, 0);
    pi[0] = 0;
    pi[1] = 0;
    size_t k = 0;
    for (size_t q = 2; q <= m; ++q)
    {
        const auto pq = pattern[q - 1];
        while ((k > 0) && (pattern[k] != pq))
        {
            k = pi[k];
        }
        if (pattern[k] == pq)
        {
            ++k;
        }
        pi[q] = k;
    }
}

template <typename Container>
size_t KMP<Container>::search(KMP_Ref& ref) const
{
    const size_t m = pattern.size();
    const size_t n = text.size();
    size_t ret = n;
    size_t& i = ref.i;
    size_t& q = ref.q;
    while ((ret == n) && (i < n))
    {
        while ((q > 0) && (pattern[q] != text[i]))
        {
            q = pi[q];
        }
        if (pattern[q] == text[i])
        {
            ++q;
        }
        ++i;
        if (q == m)
        {
            ret = i - m;
            q = pi[q];
        }
    }
    return ret;
}

typedef KMP<string> kmp_str_t;

#if defined(TEST_KMP)

#include <iostream>
#include <cstring>

int kmp_test_specific(const string& pattern, const string& text)
{
    int rc = 0;
    const size_t n = text.size();

    kmp_str_t kmp_str(pattern, text);
    vector<size_t> kmp_result;
    KMP_Ref kmp_ref;
    size_t at = n + 1;
    while (at != n)
    {
        at = kmp_str.search(kmp_ref);
        if (at != n)
        {
            kmp_result.push_back(at);
        }
    }

    vector<size_t> naive_result;
    const char* p_text = text.c_str();
    const char* p_pattern = pattern.c_str();
    const char* ptxt = p_text;
    while (((ptxt = strstr(ptxt, p_pattern)) != nullptr))
    {
        naive_result.push_back(ptxt - p_text);
        ++ptxt;
    }
    if (kmp_result != naive_result)
    {
        cerr << "Non match: #(KMP)=" << kmp_result.size() << " != " <<
            naive_result.size() << " = #(naive)\n"
            "  specific " << pattern << ' ' << text << '\n';
        rc = 1;
    }

    return rc;
}

typedef unsigned u_t;

static u_t rand_range(u_t nmin, u_t nmax)
{
    u_t r = nmin + rand() % (nmax + 1 - nmin);
    return r;
}

int kmp_test_rand(int argc, char** argv)
{
    int rc = 0;
    int ai = 0;
    const u_t n_tests = strtoul(argv[ai++], nullptr, 0);
    const u_t maxchars = strtoul(argv[ai++], nullptr, 0);
    const u_t patmin = strtoul(argv[ai++], nullptr, 0);
    const u_t patmax = strtoul(argv[ai++], nullptr, 0);
    const u_t textmin = strtoul(argv[ai++], nullptr, 0);
    const u_t textmax = strtoul(argv[ai++], nullptr, 0);
    cerr << "n_tests=" << n_tests <<
        ", maxchars=" << maxchars <<
        ", patmin=" << patmin << ", patmax=" << patmax <<
        ", textmin=" << textmin << ", textmax=" << textmax <<
        '\n';
    for (u_t ti = 0; (rc == 0) && (ti < n_tests); ++ti)
    {
        cerr << "Tested: " << ti << '/' << n_tests << '\n';
        const u_t sz_pat = rand_range(patmin, patmax);
        const u_t sz_text = rand_range(textmin, textmax);
        string pattern, text;
        while (pattern.size() < sz_pat)
        {
            pattern.push_back('a' + rand() % maxchars);
        }
        while (text.size() < sz_text)
        {
            text.push_back('a' + rand() % maxchars);
        }
        kmp_test_specific(pattern, text);
    }
    return rc;
}

static void usage(const char * argv0)
{
    cerr << "Usage: " << '\n' <<
        argv0 << " specific <pattern> <text>\n" <<
        argv0 << 
            " rand <ntest> <maxchars> <patmin> <patmax> <textmin> <textmax>\n";
}

int main(int argc, char **argv)
{
    int rc = 0;
    const string cmd(argc > 1 ? argv[1] : "");
    if (cmd == string("rand"))
    {
         rc = kmp_test_rand(argc - 2, argv + 2);
    }
    else if (cmd == string("specific"))
    {
         rc = kmp_test_specific(argv[2], argv[3]);
    }
    else
    {
        usage(argv[0]);
        rc = 1;
    }
    return rc;
}

#endif
