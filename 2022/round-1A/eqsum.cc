// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com -- Created:

#include <algorithm>
#include <fstream>
#include <iostream>
#include <iterator>
#include <numeric>
#include <sstream>
#include <string>
#include <utility>
#include <unordered_set>
#include <vector>

#include <cstdlib>
#include <unistd.h>

using namespace std;

typedef unsigned u_t;
typedef unsigned long ul_t;
typedef long long ll_t;
typedef unsigned long long ull_t;
typedef vector<int> vi_t;
typedef vector<ul_t> vul_t;

static unsigned dbg_flags;

class ErrLog
{
 public:
    ErrLog(const char *fn=0) : _f(fn ? new ofstream(fn) : 0) {}
    ~ErrLog() { delete _f; }
    template <class T>
    ErrLog& operator<<(const T &x)
    {
        if (_f)
        {
            write(x);
        }
        return *this;
    }
    template <class T> void write(const string& m, const T &c)
    {
        if (_f)
        {
            write(m);
            for (auto x: c)
            {
                write(' '); write(x);
            }
            write('\n');
            flush();
        }
    }
    void flush() { if (_f) { _f->flush(); } }
    bool active() const { return (_f != 0); }
 private:
    template <class T> void write(const T &x)
    {
        cerr << x;
        (*_f) << x;
    }
    ofstream *_f;
};

class EqSumServer
{
 public:
    EqSumServer() { setA(); }
    const vul_t getA() const { return A; }
    const vul_t getA3() const { return A3; }
    const vul_t getBigs() const { return Bigs; }
    void set_get(vul_t& result, const vul_t& B) const;
 private:
    void setA();
    static vul_t A;
    static vul_t A3;
    static vul_t Bigs;
}; 
vul_t EqSumServer::A;
vul_t EqSumServer::A3;
vul_t EqSumServer::Bigs;

void EqSumServer::setA()
{
    if (A.empty())
    {
        A.reserve(100); A3.reserve(100);
        unordered_set<ul_t> used;
        ul_t next_odd = 1;
        // static const ul_t b123[3] = {1, 2, 3};
        for (ull_t twop = 1; 2*twop < 1000000000; twop <<= 1)
        {
              A3.push_back(twop);
              used.insert(twop);
              while (
                  (used.find(next_odd) != used.end()) ||
                  (used.find(next_odd + twop) != used.end()))
              {
                  next_odd += 2;
              }
              A3.push_back(next_odd);
              A3.push_back(next_odd + twop);
              used.insert(next_odd);
              used.insert(next_odd + twop);
              next_odd += 2;
        }
        const size_t extra_size = 100 - A3.size();
        if (extra_size == 1)
        {
            cerr << "ERROR: extra_size=1";
            exit(1);
        }
        Bigs.push_back(1000000000);
        while (Bigs.size() < extra_size)
        {
            Bigs.push_back(Bigs.back() / 2);
        }
        reverse(Bigs.begin(), Bigs.end());
        ull_t Big_sum = accumulate(Bigs.begin(), Bigs.end(), 0ull);
        if (Big_sum % 2 != 0)
        {
            --Bigs.back();
        }
        A = A3;
        A.insert(A.end(), Bigs.begin(), Bigs.end());
        sort(A.begin(), A.end());
        if (dbg_flags & 0x1) {
            cerr << "A3:";
            for (ul_t x: A3) { cerr << ' ' << x; } cerr << '\n';
            cerr << "Bigs:";
            for (ul_t x: Bigs) { cerr << ' ' << x; } cerr << '\n';
            cerr << "A:";
            for (ul_t x: A) { cerr << ' ' << x; } cerr << '\n';
        }
        if (accumulate(A.begin(), A.end(), 0ull) % 2 != 0)
        {
            cerr << "Sum A not even\n";
            exit(1);
        }
    }
};

void EqSumServer::set_get(vul_t& result, const vul_t& B) const
{
    vul_t Bs(B);
    Bs.insert(Bs.end(), Bigs.begin(), Bigs.end());
    result.clear(); result.reserve(100);
    vul_t other; other.reserve(100);
    sort(Bs.begin(), Bs.end());
    ull_t rsum = 0, other_sum = 0;
    while (!Bs.empty())
    {
        const ull_t bb = Bs.back();
        Bs.pop_back();
        if (rsum <= other_sum)
        {
            result.push_back(bb);
            rsum += bb;
        }
        else
        {
            other.push_back(bb);
            other_sum += bb;
        }
    }
    if (rsum > other_sum)
    {
        swap(result, other);
        swap(rsum, other_sum);
    }
    ull_t delta = other_sum - rsum;
    delta /= 2;
    for (ul_t ai = 0; ai < A3.size(); ai += 3, delta /= 2)
    {
        if (A3[ai + 0] + A3[ai + 1] != A3[ai + 2])
        {
             cerr << "A3: Equaloity failed: ai=" << ai << '\n';
             exit(1);
        }
        if (A3[ai + 0] + A3[ai + 2] - A3[ai + 1] != (1ul << (ai/3 + 1)))
        {
             cerr << "A3: Binarty equaloity failed: ai=" << ai << '\n';
             exit(1);
        }
        if (delta % 2 == 0)
        {
            result.push_back(A3[ai + 2]);
            rsum += A3[ai + 2];
            other.push_back(A3[ai + 0]);
            other.push_back(A3[ai + 1]);
            other_sum += A3[ai + 0] + A3[ai + 1];
        }
        else
        {
            result.push_back(A3[ai + 0]);
            result.push_back(A3[ai + 2]);
            rsum += A3[ai + 0] + A3[ai + 2];
            other.push_back(A3[ai + 1]);
            other_sum += A3[ai + 1];
        }
    }
    if (result.size() > other.size())
    {
        swap(result, other);
    }
}

class EqSum
{
 public:
    EqSum(istream& cfi, ostream& cfo, ErrLog &el) :
        fi(cfi), fo(cfo), errlog(el) {}
    void solve();
 private:
    bool readline_ints(vul_t &v);
    istream& fi;
    ostream& fo;
    ErrLog &errlog;
};

bool EqSum::readline_ints(vul_t &v)
{
    v.clear();
    string line;
    getline(fi, line);
    istringstream  iss(line);
    while (!iss.eof())
    {
        int x;
        iss >> x;
        if (!iss.fail())
        {
            v.push_back(x);
        }
    }
    return fi.eof();
}

void EqSum::solve()
{
    const EqSumServer ess;
    const vul_t& A = ess.getA();
    vul_t B, result; B.reserve(100); result.reserve(100);

    u_t T;
    string dumline;
    fi >> T; getline(fi, dumline);
    errlog << "T="<<T << '\n';
    for (u_t ti = 0; ti < T; ++ti)
    {
        u_t N;
        fi >> N;
        errlog << "ti="<<ti << ", N="<<N << '\n'; errlog.flush();
        getline(fi, dumline);
        if (N != 100)
        {
            errlog << "N=" << N << " != 100\n";
            cerr << "N=" << N << " != 100\n";
            exit(1);
        }
        const char *sep = "";
        for (u_t i = 0; i < N; ++i)
        {
            fo << sep << A[i]; sep = " ";
        }
        fo << '\n'; fo.flush();
        B.clear();
        readline_ints(B);
        if (B.size() != N)
        {
            errlog << "#(B)=" << B.size() << " != " << N << "=N\n";
            cerr << "#(B)=" << B.size() << " != " << N << "=N\n";
            exit(1);
        }
        if (dbg_flags & 0x1) {
            errlog << "B:";
            for (ul_t x: B) { errlog << ' ' << x; } errlog << '\n';
        }
        ess.set_get(result, B);
        sep = "";
        for (u_t i = 0; i < N; ++i)
        {
            fo << sep << result[i]; sep = " ";
        }
        fo << '\n'; fo.flush();
    }
}

static void vshow(const char* msg, const vul_t& v)
{
    cerr << msg;
    for (ul_t x: v) { cerr << ' ' << x; }
    cerr << '\n';
}

static int ABtest(const EqSumServer &ess, const vul_t& A, const vul_t& B)
{
    int rc = 0;
    vul_t result;
    const ull_t Asum = accumulate(A.begin(), A.end(), 0ull);
    cerr << "Asum="<<Asum << '\n';
    vshow("B", B);
    ull_t Bsum = accumulate(B.begin(), B.end(), 0ull);
    cerr << "Bsum="<<Bsum << '\n';
    ess.set_get(result, B);
    vshow("result", result);
    cerr << '\n';
    ull_t rsum = accumulate(result.begin(), result.end(), 0ull);
    if (2*rsum != Asum + Bsum)
    {
        cerr << "2*rsum="<<2*rsum << " != Asum + Bsum = "<< Asum + Bsum <<'\n';
        rc = 1;
    }
    return rc;
}

static int test(istream& fi)
{
    int rc = 0;
    const EqSumServer ess;
    const vul_t& A = ess.getA();
    vshow("A", A);
    cerr << '\n';
    vul_t B; B.reserve(100);
    B.push_back(1000000000);
    ul_t b = 1;
    while (B.size() < 100)
    {
        while (binary_search(A.begin(), A.end(), b))
        {
            ++b;
        }
        B.push_back(b++);
    }
    if (accumulate(B.begin(), B.end(), 0ull) % 2 != 0)
    {
        while (binary_search(A.begin(), A.end(), b))
        {
            b += 2;
        }
        B.back() = b;
    }
    rc = ABtest(ess, A, B);
    if (rc == 0)
    {
        B.clear();
        copy_n(istream_iterator<ul_t>(fi), 100, back_inserter(B));
        rc = ABtest(ess, A, B);
    }
    return rc;
}

int main(int argc, char ** argv)
{
    const string dash("-");

    int rc = 0, ai;

    for (ai = 1; (rc == 0) && (ai < argc) && (argv[ai][0] == '-') &&
        argv[ai][1] != '\0'; ++ai)
    {
        const string opt(argv[ai]);
        if (opt == string("-debug"))
        {
            dbg_flags = strtoul(argv[++ai], 0, 0);
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

    ErrLog errlog(dbg_flags & 0x1 ? "/tmp/ymcj.log" : 0);
    if (dbg_flags & 0x200) 
    {
        errlog << "pid = " << getpid() << "\n"; errlog.flush();
        int slept = 0;
        while (slept < 90)
        {
            sleep(1);
            ++slept;
        }
    }

    if (dbg_flags & 0x800)
    {
        rc = test(*pfi);
    }
    else
    {
        string ignore;
        EqSum problem(*pfi, *pfo, errlog);
        problem.solve();
    }

    if (pfi != &cin) { delete pfi; }
    if (pfo != &cout) { delete pfo; }
    return rc;
}
