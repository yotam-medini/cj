// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com --

// #include <algorithm>
#include <algorithm>
#include <fstream>
#include <iostream>
#include <sstream>
#include <map>
#include <string>
#include <utility>
#include <vector>
#include <array>

#include <cstdlib>

using namespace std;

typedef unsigned char  uc_t;
typedef unsigned u_t;
typedef unsigned long ul_t;
typedef unsigned long long ull_t;
typedef vector<u_t> vu_t;
typedef vector<uc_t> vuc_t;
typedef array<uc_t, 3> auc3_t;

class HeadVal
{
 public:
    HeadVal(const vuc_t& h=vuc_t(), double v=0.) :
        head(h), value(v) {}
    vuc_t head;
    double value;
    string str() const
    {
        ostringstream os;
        for (u_t i: head)
        {
            os << "RPS"[i];
        }
        os << ": " << value;
        return os.str();
    }
};

static unsigned dbg_flags;

class RPS
{
 public:
    RPS(istream& fi, u_t _X, u_t _rounds=60, u_t _depth=1);
    RPS(u_t _W, u_t _E, u_t r, u_t d) :
        W(_W), E(_E), X(16400),  rounds(r), depth(d) {}
    const RPS& solve_naive();
    const RPS& solve();
    void print_solution(ostream&) const;
    double expectation() const;
    const vu_t get_solution() const { return solution; }
    void write_solution(ostream& os) const;
 private:
    typedef map<auc3_t, HeadVal> u3tohead_t;
    string str(const u3tohead_t::value_type& v) const;
    void naive_next();
    const HeadVal *get_best(const auc3_t &rps);
    u_t W, E;
    const u_t X;
    const u_t rounds, depth;
    vu_t solution;
    vu_t fwd;
    u_t best_next;
    double best_fwd_expect;
    u3tohead_t memo;
};

string RPS::str(const u3tohead_t::value_type& v) const
{
    ostringstream os;
    const auc3_t& k = v.first;
    os << '(' <<u_t(k[0]) << ", "<<u_t(k[1]) << ", "<<u_t(k[2]) << ") -> " <<
       v.second.str();
    return os.str();
}

RPS::RPS(istream& fi, u_t _X, u_t _rounds, u_t _depth) :
    X(_X), rounds(_rounds), depth(_depth)
{
    fi >> W >> E;
}

const RPS& RPS::solve_naive()
{
    for (u_t r = 0; r < rounds; ++r)
    {
        fwd.clear();
        best_fwd_expect = 0.;
        best_next = 0;
        naive_next();
        solution.push_back(best_next);
    }
    // if (dbg_flags & 0x1) { cerr << "Expectation=" << expectation() << '\n'; }
    return *this;
}

void RPS::naive_next()
{
    u_t fwd_sz = fwd.size();
    if ((fwd_sz == depth) || (solution.size() + fwd_sz == rounds))
    {
        double expect = 0.;
        u_t count[3] = {0, 0, 0};
        u_t ssz = solution.size();
        for (u_t i: solution)
        {
            ++count[i];
        }
        for (u_t i: fwd)
        {
            if (ssz == 0)
            {
                expect += (1./3.) * (W + E);
            }
            else
            {
                // Assume i ~ Rock. 
                // Win   if B choose Scissors. For this B looks at A's Paper.
                // Equal if B choose Rock.     For this B looks at A's Scissors.
                u_t i1 = (i + 1) % 3;
                u_t i2 = (i + 2) % 3;
                double prob_win = double(count[i1]) / double(ssz);
                double prob_equ = double(count[i2]) / double(ssz);
                double win = prob_win * W;
                double equ = prob_equ * E;
                expect += win + equ;
            }
            ++count[i];
            ++ssz;
        }
        if ((dbg_flags & 0x2) && (fwd_sz == rounds)) {
            for (u_t i: fwd) { cerr << "RPS"[i]; } 
            cerr << " " << expect << '\n';
        }
        if (best_fwd_expect < expect)
        {
            best_fwd_expect = expect;
            best_next = fwd[0];
        }
    }
    else
    {
        for (u_t i = 0; i < 3; ++i)
        {
            fwd.push_back(i);
            naive_next();
            fwd.pop_back();
        }
    }
}

const RPS& RPS::solve()
{
    const HeadVal *best = 0;
    for (uc_t r = 0; r <= rounds; ++r)
    {
        for (uc_t p = 0; r + p <= rounds; ++p)
        {
            uc_t s = rounds - (r + p);
            auc3_t rps{r, p, s};
            const HeadVal *headval = get_best(rps);
            if ((!best) || best->value < headval->value)
            {
                best = headval;
            }
        }
    }
    copy(best->head.begin(), best->head.end(), back_inserter(solution));
    //if (dbg_flags & 0x1) { cerr << "Expectation: " << expectation() << '\n'; }
    return *this;
}

const HeadVal *RPS::get_best(const auc3_t &rps)
{
    auto er = memo.equal_range(rps);
    u3tohead_t::iterator iter = er.first;
    if (iter == er.second)
    {
        const uc_t sum_rps = rps[0] + rps[1] + rps[2];
        if (sum_rps == 1)
        {
            uc_t i = (rps[0] ? 0 : (rps[1] ? 1: 2));
            vuc_t h; h.push_back(i); // Rock
            HeadVal hv(h, (W + E)/3.);
            iter = memo.insert(iter, u3tohead_t::value_type(rps, hv));
        }
        else // sum_rps >= 2
        {
            const double div_denom = 1./double(sum_rps - 1);
            uc_t ibest = 0;
            double expect_best = 0.;
            const HeadVal *psubs[3];
            for (uc_t i = 0; i < 3; ++i)
            {
                if (rps[i] > 0)
                {
                    auc3_t rps_sub(rps);
                    --rps_sub[i];
                    psubs[i] = get_best(rps_sub);
                    const uc_t i1 = (i + 1) % 3;
                    const uc_t i2 = (i + 2) % 3;
                    const double p_win = double(rps_sub[i1]) * div_denom;
                    const double p_equ = double(rps_sub[i2]) * div_denom;
                    const double expect_last = p_win * W + p_equ * E;
                    const double expect_i = psubs[i]->value + expect_last;
                    if (expect_best < expect_i)
                    {
                        expect_best = expect_i;
                        ibest = i;
                    }
                }
            }
            vuc_t h(psubs[ibest]->head);
            h.push_back(ibest);
            HeadVal hv(h, expect_best);
            iter = memo.insert(iter, u3tohead_t::value_type(rps, hv));
        }
        if (dbg_flags & 0x10) { cerr << str(*iter) << '\n'; }
    }
    return &(iter->second);
}

double RPS::expectation() const
{
    double expect = 0.;
    u_t count[3] = {0, 0, 0};
    u_t ssz = 0;
    for (u_t i: solution)
    {
        double add;
        if (ssz == 0)
        {
            add = (1./3.) * (W + E);
        }
        else
        {
            u_t i1 = (i + 1) % 3;
            u_t i2 = (i + 2) % 3;
            double prob_win = double(count[i1]) / double(ssz);
            double prob_equ = double(count[i2]) / double(ssz);
            double win = prob_win * W;
            double equ = prob_equ * E;
            add = win + equ;
        }
        if (dbg_flags & 0x8) { 
          if (W==50 && E==0) {
            cerr << "["<<ssz<< "], add="<<add << '\n'; 
        }}
        expect += add;
        ++count[i];
        ++ssz;
    }
    return expect;
}

void RPS::write_solution(ostream& os) const
{
    for (u_t i: solution)
    {
        os << "RPS"[i];
    }
}

void RPS::print_solution(ostream &fo) const
{
    fo << ' ';
    write_solution(fo);
}

static int test(u_t rounds_b)
{
    int rc = 0;
    for (u_t rounds = rounds_b; (rc == 0) && (rounds <= 10); ++rounds)
    {
        static const u_t W = 10;
        static const u_t Es[] = {0, 1, 2, 5, 10};
        for (u_t ei = 0; (rc == 0) && (ei < 5); ++ei)
        {
            const u_t E = Es[ei];
            RPS rps_naive(W, E, rounds, rounds);
            rps_naive.solve_naive();
            double expectation_naive = rps_naive.expectation();
            RPS rps(W, E, rounds, rounds);
            rps.solve();
            double expectation = rps.expectation();
            double delta = abs(expectation - expectation_naive) /
                (expectation + expectation_naive + 1);
            cerr << "r="<<rounds << ", W="<<W << ", E="<<E;
            if (delta > 1./42.)
            {
                cerr << ", expectation: naive=" << expectation_naive <<
                    ", real=" << expectation << '\n' <<
                    " naive: "; 
                rps_naive.write_solution(cerr);
                cerr << ", real: ";
                rps.write_solution(cerr);
                rc = 1;
            }
            else
            {
                cerr << ", expectation=" << expectation << " : ";
                rps.write_solution(cerr);
            }
            cerr << '\n';
        }
    }
    return rc;
}

int main(int argc, char ** argv)
{
    const string dash("-");

    bool naive = false;
    bool tellg = false;
    u_t solve_ver = 0;
    u_t rounds = 60, depth = 2;
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
        else if (opt == string("-rounds"))
        {
            rounds = strtoul(argv[++ai], 0, 0);
        }
        else if (opt == string("-depth"))
        {
            depth = strtoul(argv[++ai], 0, 0);
        }
        else if (opt == string("-solve1"))
        {
            solve_ver = 1;
        }
        else if (opt == string("-debug"))
        {
            dbg_flags = strtoul(argv[++ai], 0, 0);
        }
        else if (opt == string("-test"))
        {
            rc = test(strtoul(argv[++ai], 0, 0));
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

    if (dbg_flags & 0x1) {
        cerr << "sizeof(auc3_t)=" << sizeof(auc3_t) << '\n'; }
    string ignore;
    unsigned n_cases, X;
    *pfi >> n_cases >> X;
    getline(*pfi, ignore);

    const RPS& (RPS::*psolve)() =
        (naive ? &RPS::solve_naive : &RPS::solve);
    if (solve_ver == 1) { psolve = &RPS::solve; } // solve1
    ostream &fout = *pfo;
    ul_t fpos_last = pfi->tellg();
    for (unsigned ci = 0; (rc == 0) && (ci < n_cases); ci++)
    {
        RPS rps(*pfi, X, rounds, depth);
        getline(*pfi, ignore);
        if (tellg)
        {
            ul_t fpos = pfi->tellg();
            cerr << "Case (ci+1)="<<(ci+1) << ", tellg=[" <<
                fpos_last << " " << fpos << ") size=" <<
                (fpos - fpos_last) << "\n";
            fpos_last = fpos;
        }

        (rps.*psolve)();
        fout << "Case #"<< ci+1 << ":";
        rps.print_solution(fout);
        fout << "\n";
        fout.flush();
        if (dbg_flags & 1) { cerr << "ci="<<ci <<
            ", expectation=" << rps.expectation() << '\n'; }
    }

    if (pfi != &cin) { delete pfi; }
    if (pfo != &cout) { delete pfo; }
    return rc;
}
