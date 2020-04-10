// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com -- Created:

#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <vector>
#include <utility>

#include <cstdlib>
#include <unistd.h>

using namespace std;

typedef unsigned u_t;
typedef unsigned long ul_t;
typedef unsigned long long ull_t;
typedef vector<u_t> vu_t;
typedef vector<int> vi_t;

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

class Esab
{
 public:
    Esab(istream& fi, ErrLog &el) : errlog(el) {}
    void solve_naive(istream& fi, ostream &fo);
    void solve(istream& fi, ostream &fo);
 private:
    enum { OP_COMPLEMENT, OP_REVERSE, OP_COMPREV, OP_ASIS };
    void solve10(istream& fi, ostream &fo);
    void solve20(istream& fi, ostream &fo);
    void solve100(istream& fi, ostream &fo);
    void update100(istream& fi, ostream &fo, u_t* b, u_t bb, u_t be);
    u_t qbit(istream& fi, ostream &fo, u_t bi);
    void do_answer(istream& fi, ostream &fo, const u_t* b);
    ErrLog &errlog;
    u_t T, B;
};

void Esab::solve_naive(istream& fi, ostream &fo)
{
    fi >> T >> B;
    errlog << "Got T="<<T << ", B="<<B<< '\n'; errlog.flush();
    for (u_t ti = 0; ti < T; ++ti)
    {
        errlog << "ti=" << ti << '\n'; errlog.flush();
        switch (B)
        {
         case 10:
            solve10(fi, fo);
            break;
         case 20:
            solve20(fi, fo);
            break;
         case 100:
            solve100(fi, fo);
            break;
         default:
            errlog << "Unexpected B="<<B << '\n'; errlog.flush();
            exit(1);
        }
    }
}

void Esab::solve(istream& fi, ostream &fo)
{
    solve_naive(fi, fo);
}

void Esab::solve10(istream& fi, ostream &fo)
{
    u_t b[10];
    for (u_t bi = 0; bi < 10; ++bi)
    {
        b[bi] = qbit(fi, fo, bi);
    }
    do_answer(fi, fo, b);
}

void Esab::solve20(istream& fi, ostream &fo)
{
    u_t b[20];
    for (u_t bi = 0, mbi = 19; bi < 10; ++bi, --mbi)
    {
        b[bi] = qbit(fi, fo, bi);
        b[mbi] = qbit(fi, fo, mbi);
    }
    for (u_t bi = 0, mbi = 19; bi < 10; ++bi, --mbi)
    {
        bool same = (b[bi] == b[mbi]);
        u_t newval = qbit(fi, fo, bi);
        b[bi] = newval;
        b[mbi] = (same ? newval : 1 - newval);
    }
    do_answer(fi, fo, b);
}    

void Esab::solve100(istream& fi, ostream &fo)
{
    u_t b[100];
    vu_t same, diff;
    for (u_t bi = 0, mbi = 99; bi < 50; ++bi, --mbi)
    {
        b[bi] = qbit(fi, fo, bi);
        b[mbi] = qbit(fi, fo, mbi);
        if (b[bi] == b[mbi])
        {
            same.push_back(bi);
        }
        else
        {
            diff.push_back(bi);
        }
    }
    for (u_t bb = 0, be = 5; bb < 50; bb = be, be += 5)
    {
        update100(fi, fo, b, bb, be);
    }
    update100(fi, fo, b, 0, 25);
    update100(fi, fo, b, 25, 50);
    do_answer(fi, fo, b);
}

void Esab::update100(istream& fi, ostream &fo, u_t* b, u_t bb, u_t be)
{
    u_t qsz = be - bb;
    errlog << "update100 [" << bb << ", " << be << ")\n"; errlog.flush();
    vu_t same, diff;
    for (u_t bi = bb, mbi = 99 - bb; bi < be; ++bi, --mbi)
    {
        if (b[bi] == b[mbi])
        {
            same.push_back(bi);
        }
        else
        {
            diff.push_back(bi);
        }
    }
    errlog << "Determine Op\n"; errlog.flush();
    errlog << "#(same)=" << same.size() << ", #(diff)=" << diff.size() << '\n';
    if (dbg_flags & 0x1) {
        errlog << "Pre-Op: b=\n";
        for (u_t bi = 0; bi < 100; ++bi) { errlog << b[bi]; } 
        errlog << '\n'; errlog.flush();
    }
    u_t op = OP_ASIS;
    if ((same.size() == qsz) || (diff.size() == qsz))
    {
        qbit(fi, fo, bb); // dummy - ro ensure 2 queries
        u_t newval = qbit(fi, fo, bb);
        if (newval != b[bb])
        {
            op = OP_COMPLEMENT;
        }
    }
    else
    {
        u_t bi_eq = same[0];
        u_t bi_diff = diff[0];
        u_t newval_eq = qbit(fi, fo, bi_eq);
        u_t newval_diff = qbit(fi, fo, bi_diff);
        bool change_eq = (b[bi_eq] != newval_eq);
        bool change_diff = (b[bi_diff] != newval_diff);
        errlog << "bi_eq="<<bi_eq << ", bi_diff="<<bi_diff <<
            ", newval_eq="<<newval_eq << ", newval_diff="<<newval_diff <<
            ", change_eq="<<change_eq << ", change_diff="<<change_diff << '\n';
        if (change_eq || change_diff)
        {
            if (change_eq && change_diff)
            {
                op = OP_COMPLEMENT;
            }
            else if (change_eq) // !change_diff
            {
                op = OP_COMPREV;
            }
            else // change_diff && !change_eq
            {
                op = OP_REVERSE;
            }
        }
    }
    errlog << "op=" << int(op) << "\n"; errlog.flush();
    if (op != OP_ASIS)
    {
        for (u_t bi = bb, mbi = 99 - bb; bi < be; ++bi, --mbi)
        {
            // b[bi], b[mbi]
            switch (op)
            {
             case OP_COMPLEMENT:
                b[bi] = 1 - b[bi];
                b[mbi] = 1 - b[mbi];
                break;
             case OP_REVERSE:
                swap(b[bi], b[mbi]);
                break;
             case OP_COMPREV:
                b[bi] = 1 - b[bi];
                b[mbi] = 1 - b[mbi];
                swap(b[bi], b[mbi]);
                break;
            }
        }
    }
    if (dbg_flags & 0x1) {
        errlog << "Post-Op: b=\n";
        for (u_t bi = 0; bi < 100; ++bi) { errlog << b[bi]; } 
        errlog << '\n'; errlog.flush();
    }
}

u_t Esab::qbit(istream& fi, ostream &fo, u_t bi)
{
    u_t bi1 = bi + 1;
    errlog << "bi="<< bi << ", qbit(" << bi1 << ")\n"; errlog.flush();
    fo << bi1 << '\n'; fo.flush();
    u_t ret;
    fi >> ret;
    errlog << "Got: ret=" << ret << '\n'; errlog.flush();
    return ret;
}

void Esab::do_answer(istream& fi, ostream &fo, const u_t* b)
{
    string answer;
    for (u_t bi = 0; bi < B; ++bi)
    {
        char c = "01"[b[bi]];
        answer.push_back(c);
    }
    errlog << "Answer: " << answer << '\n'; errlog.flush();
    fo << answer << '\n'; fo.flush();
    string judge;
    fi >> judge;
    errlog << "Judge: " << judge << '\n'; errlog.flush();
    if (judge == string("Y"))
    {
        errlog << "Yes - good!\n"; errlog.flush();
    }
    else
    {
        errlog << "Failed, exiting\n"; errlog.flush();
        exit(1);
    }
}

int main(int argc, char ** argv)
{
    const string dash("-");

    bool naive = false;
    int rc = 0, ai;

    for (ai = 1; (rc == 0) && (ai < argc) && (argv[ai][0] == '-') &&
        argv[ai][1] != '\0'; ++ai)
    {
        const string opt(argv[ai]);
        if (opt == string("-naive"))
        {
            naive = true;
        }
        else if (opt == string("-debug"))
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
    if (dbg_flags & 0x2) 
    {
        errlog << "pid = " << getpid() << "\n"; errlog.flush();
        int slept = 0;
        while (slept < 90)
        {
            sleep(1);
            ++slept;
        }
    }

    string ignore;
    Esab problem(*pfi, errlog);
    if (naive)
    {
         problem.solve_naive(*pfi, *pfo);
    }
    else
    {
         problem.solve_naive(*pfi, *pfo);
    }

    if (pfi != &cin) { delete pfi; }
    if (pfo != &cout) { delete pfo; }
    return 0;
}
