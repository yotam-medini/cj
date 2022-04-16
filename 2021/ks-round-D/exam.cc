// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com --

#include <algorithm>
#include <array>
#include <fstream>
#include <iostream>
#include <iterator>
#include <set>
#include <string>
#include <tuple>
#include <utility>
#include <vector>
// #include <map>

#include <cstdlib>

using namespace std;

typedef unsigned u_t;
typedef unsigned long ul_t;
typedef unsigned long long ull_t;
typedef array<ull_t, 2> aull2_t;
typedef vector<aull2_t> vaull2_t;
typedef vector<ull_t> vull_t;
// typedef tuple<ull_t, u_t> ull_u_t;

class EndPt : public tuple<ull_t, u_t>
{
 public:
    EndPt(ull_t _pos=0, ul_t _be=0) : tuple<ull_t, u_t>(_pos, _be) {}
    ull_t pos() const { return get<0>(*this); }
    void pos(ull_t p) { get<0>(*this) = p; }
    u_t be() const { return get<1>(*this); }
    void be(u_t zo) { get<1>(*this) = zo; }
};
typedef set<EndPt> set_ep_t;
typedef set<aull2_t> set_segs_t;

static unsigned dbg_flags;

class Exam
{
 public:
    Exam(istream& fi);
    Exam(const vaull2_t& _ABs, const vull_t& _S) :
        N(_ABs.size()), M(_S.size()), ABs(_ABs), S(_S) {}
    void solve_naive();
    void solve();
    void print_solution(ostream&) const;
    const vull_t& get_solution() const { return solution; }
 private:
    ul_t N, M;
    vaull2_t ABs;
    vull_t S;
    vull_t solution;
};

Exam::Exam(istream& fi)
{
    fi >> N >> M;
    ABs.reserve(N);
    S.reserve(M);
    while (ABs.size() < N)
    {
        aull2_t ab;
        fi >> ab[0] >> ab[1];
        ABs.push_back(ab);
    }
    copy_n(istream_iterator<ull_t>(fi), M, back_inserter(S));
}

void Exam::solve_naive()
{
    ull_t max_exam = 0;
    for (const aull2_t& ab: ABs)
    {
        max_exam = max(max_exam, ab[1]);
    }
    if (dbg_flags & 0x1) { cerr << "max_exam=" << max_exam << '\n'; }
    vector<bool> exams(size_t(max_exam + 1), false);
    for (const aull2_t& ab: ABs)
    {
        for (ull_t i = ab[0]; i <= ab[1]; ++i)
        {
            exams[i] = true;
        }
    }
    solution.reserve(M);
    for (ull_t si = 0; si < M; ++si)
    {
        const ull_t skill = S[si];
        ull_t exam = ull_t(-1); // undef
        for (ull_t step = 0; (exam == ull_t(-1)); ++step)
        {
            // if (skill - step >= 0)
            if ((skill >= step) && exams[skill - step])
            {
                exam = skill - step;
                exams[exam] = false;
            }
            else if ((skill + step <= max_exam) && exams[skill + step])
            {
                exam = skill + step;
                exams[exam] = false;
            }
        }
        solution.push_back(exam);
    }
}

static void insert_if_non_empty(
    set_segs_t& exams, 
    set_segs_t::iterator iter,
    const aull2_t segment)
{
    if (segment[0] < segment[1])
    {
        exams.insert(iter, segment);
    }
}

void Exam::solve()
{
    set_segs_t exams;
    for (const aull2_t& ab: ABs)
    {
        exams.insert(exams.end(), aull2_t{ab[0], ab[1] + 1});
    }
    solution.reserve(M);
    for (ull_t si = 0; si < M; ++si)
    {
        ull_t exam = ull_t(-1); // undef
        const ull_t skill = S[si];
        auto er = exams.equal_range(aull2_t{skill, skill});
        if (er.second == exams.begin())
        {
            aull2_t segment = *exams.begin();
            exams.erase(exams.begin());
            exam = segment[0];
            ++segment[0];
            if (segment[0] < segment[1])
            {
                exams.insert(exams.begin(), segment);
            }
        }
        else if (er.first == exams.end())
        {
            set_segs_t::reverse_iterator riter = exams.rbegin();
            ++riter;
            set_segs_t::iterator iter(riter.base());
            aull2_t segment = *iter;
            exam = (skill < segment[1] ? skill : segment[1] - 1);
            aull2_t pre_segment{segment[0], exam};
            aull2_t post_segment{exam + 1, segment[1]};
            for (const aull2_t& split_seg: {pre_segment, post_segment})
            {
                insert_if_non_empty(exams, iter, split_seg);
            }
            exams.erase(iter);
        }
        else if (er.first != er.second) // found exact - can never happen
        {
            aull2_t segment = *er.first;
            exam = segment[0]++;
            insert_if_non_empty(exams, er.first, segment);
            exams.erase(er.first);
        }
        else // (er.first == er.second)
        {
            set_segs_t::iterator iter_next = er.first;
            aull2_t segment_next = *iter_next;
            set_segs_t::iterator iter = iter_next;
            --iter;
            aull2_t segment = *iter;
            if (skill < segment[1])
            {
                exam = skill;
                aull2_t pre_segment{segment[0], skill};
                aull2_t post_segment{skill + 1, segment[1]};
                for (const aull2_t& split_seg: {pre_segment, post_segment})
                {
                    insert_if_non_empty(exams, iter, split_seg);
                }
                exams.erase(iter);
            } 
            else if (skill - (segment[1] - 1) <= segment_next[0] - skill)
            {
                exam = --segment[1];
                insert_if_non_empty(exams, iter, segment);
                exams.erase(iter);
            }
            else
            {
                exam = segment_next[0]++;
                insert_if_non_empty(exams, iter_next, segment_next);
                exams.erase(iter_next);
            }
        }
        solution.push_back(exam);
    }
}

void Exam::print_solution(ostream &fo) const
{
    for (ull_t x: solution)
    {
        fo << ' ' << x;
    }
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

static ull_t rand_range(ull_t nmin, ull_t nmax)
{
    u_t r = nmin + rand() % (nmax + 1 - nmin);
    return r;
}

static void save_case(const char *fn, const vaull2_t &ABs, const vull_t &S)
{
    ofstream f(fn);
    f << "1\n" << ABs.size() << ' ' << S.size() << '\n';
    for (const aull2_t& ab: ABs)
    {
        f << ab[0] << ' ' << ab[1] << '\n';
    }
    for (const ull_t skill: S)
    {
        f << skill << '\n';
    }
    f.close();
}

static int test_case(const vaull2_t& ABs, const vull_t& S)
{
    int rc = 0;
    vull_t solution, solution_naive;
    const ull_t N = ABs.size();
    const ull_t M = S.size();
    const bool small = (N < 100) && (M < 100);
    if (small)
    {
        Exam p(ABs, S);
        p.solve_naive();
        solution_naive = p.get_solution();
    }
    {
        Exam p(ABs, S);
        p.solve();
        solution = p.get_solution();
    }
    if (small && (solution != solution_naive))
    {
        rc = 1;
        cerr << "Failure: inconsistencies\n";
        save_case("exam-fail.in", ABs, S);
    }
    if (small)
    {
        cerr << "  N=" << N << ", M=" << M << '\n';
        cerr << "  Skills:   "; 
        copy(S.begin(), S.end(), ostream_iterator<ull_t>(cerr, " ")); 
        cerr << '\n';
        cerr << "  Solution: "; 
        copy(solution.begin(), solution.end(),
            ostream_iterator<ull_t>(cerr, " "));
        cerr << '\n';
    }   
    return rc;
}

static void rand_segments(vaull2_t& ABs, ull_t N, ull_t M, ull_t Bmax)
{
    ABs.reserve(M);
    for (bool found = false; !found; )
    {
        set<ull_t> set_reps;
        while (set_reps.size() < N)
        {
            set_reps.insert(rand_range(1, Bmax));
        }
        vull_t reps(set_reps.begin(), set_reps.end());
        ABs.clear();
        ull_t total_size = 0;
        for (u_t ri = 0; ri < N; ++ri)
        {
            aull2_t ab;
            ab[0] = rand_range(
                ABs.empty() ? 1 : ABs.back()[1] + 1, 
                ri + 1 < N ? reps[ri + 1] - 1 : Bmax);
            ab[1] = rand_range(ab[0], 
                ri + 1 < N ? reps[ri + 1] - 1 : Bmax);
            ABs.push_back(ab);
            ull_t sz = ab[1] + 1 - ab[0];
            total_size += sz;
        }
        found = (total_size >= M);
    }
}

static int test_random(int argc, char ** argv)
{
    int rc = 0;
    int ai = 0;
    u_t n_tests = strtoul(argv[ai++], 0, 0);
    const ull_t Nmin = strtoul(argv[ai++], 0, 0);
    const ull_t Nmax = strtoul(argv[ai++], 0, 0);
    const ull_t Mmin = strtoul(argv[ai++], 0, 0);
    const ull_t Mmax = strtoul(argv[ai++], 0, 0);
    const ull_t Smax = strtoul(argv[ai++], 0, 0);
    const ull_t cBmax = strtoul(argv[ai++], 0, 0);
    cerr << "n_tests="<<n_tests <<
       ", Nmin="<<Nmin << ", Nmax="<<Nmax <<
       ", Mmin="<<Mmin << ", Mmax="<<Mmax <<
       ", Smax="<<Smax << ", cBmax="<<cBmax << '\n';
    for (u_t ti = 0; (rc == 0) && (ti < n_tests); ++ti)
    {
        cerr << "Tested: " << ti << '/' << n_tests << '\n'; cerr.flush();
        ull_t N = rand_range(Nmin, Nmax);
        ull_t M = rand_range(Mmin, Mmax);
        ull_t Bmax = max(cBmax, M);
        vaull2_t ABs;
        rand_segments(ABs, N, M, Bmax);
        vull_t S;
        while (S.size() < M)
        {
            S.push_back(rand_range(1, Smax));
        }
        rc = test_case(ABs, S);
    }
    return rc;
}

int main(int argc, char **argv)
{
    int rc = ((argc > 1) && (string(argv[1]) == string("test"))
        ? test_random(argc - 2, argv + 2)
        : real_main(argc, argv));
    return rc;
}
