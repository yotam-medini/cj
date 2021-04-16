// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com --

#include <algorithm>
#include <fstream>
#include <iostream>
#include <iterator>
#include <numeric>
#include <string>
#include <utility>
#include <vector>

#include <cstdlib>
#include <cmath>

using namespace std;

typedef unsigned u_t;
typedef unsigned long ul_t;
typedef unsigned long long ull_t;
typedef vector<string> vs_t;
typedef vector<u_t> vu_t;
typedef vector<double> vd_t;
typedef vector<bool> vb_t;
typedef vector<vb_t> vvb_t;

static unsigned dbg_flags;

class Cheating
{
 public:
    Cheating(istream& fi, u_t _percentage);
    void solve_naive();
    void solve();
    void print_solution(ostream&) const;
 private:
    enum
    {
        N_PLAYERS = 100,
        N_PROLEMS = 10000,
        N_SAMPLE = 2000
    };
    static u_t b2i(bool b) { return b ? 1 : 0; }
    static double sigmoid(double s, double q)
    {
        const double x = s - q;
        const double y = 1. / (1 + exp(-x));
        return y;
    }
    void set_players_problems();
    void sum_problems();
    double player_distance(double& skill, u_t pi, u_t score) const;
    double compute_difficulty(double average) const;
    double compute_skill(double average) const;
    void compute_g_difficulties();
    u_t percentage;
    vs_t lines;
    u_t solution;
    vvb_t players_problems;
    vu_t problems_sums;
    vd_t g_difficulties;
};

Cheating::Cheating(istream& fi, u_t _percentage) :
    percentage(_percentage),
    solution(0)
{
    lines.reserve(N_PLAYERS);
    copy_n(istream_iterator<string>(fi), size_t(N_PLAYERS),
        back_inserter(lines));
}

void Cheating::solve_naive()
{
    set_players_problems();
    sum_problems();
    compute_g_difficulties();
    double dist_max = 0., max_skill = 0.;
    u_t pi_max_skill = 0;
    for (u_t pi = 0; pi < N_PLAYERS; ++pi)
    {
        u_t score = 0;
        for (bool b: players_problems[pi])
        {
            score += b2i(b);
        }
        if (score > N_PROLEMS/2)
        {
            double skill = 0.;
            double dist = player_distance(skill, pi, score);
            if (dist_max < dist)
            {
                dist_max = dist;
                solution = pi + 1;
            }
            if (max_skill < skill)
            {
                max_skill = skill;
                pi_max_skill = pi;
            }
        }
    }
    if (max_skill > 3.)
    {
        if (max_skill - 3. > dist_max / N_PROLEMS)
        {
            solution = pi_max_skill + 1;
        }
    }
}

void Cheating::solve()
{
    set_players_problems();
    sum_problems();
    vu_t qhard(N_PROLEMS);
    if (dbg_flags & 0x1) { cerr << "qhard.size=" << qhard.size() << '\n'; }
    iota(qhard.begin(), qhard.end(), 0);
    sort(qhard.begin(), qhard.end(), 
        [this](u_t i0, u_t i1)
        {
            return problems_sums[i0] > problems_sums[i1];
        });
#if 1
    ull_t max_off_num = 0, max_off_denom = 1.;
#else
    double max_off = 0.;
#endif
    u_t max_off_pi = 0;
    for (u_t pi = 0; pi < N_PLAYERS; ++pi)
    {
        const vb_t player_problems = players_problems[pi];
        u_t n0 = 0, n_inv = 0;
        for (u_t qi: qhard)
        {
            if (player_problems[qi])
            {
                n_inv += n0;
            }
            else
            {
                ++n0;
            }
        }
        const ull_t numerator = n_inv;
        const ull_t denom = n0 * (N_PROLEMS - n0);
#if 1
        // if (max_off_num / max_off_denom) < (numerator / denom)
        if ((max_off_num * denom < numerator * max_off_denom) ||
            ((denom == 0) && (max_off_denom == 0) && (max_off_num < numerator)))
        {
             max_off_num = numerator;
             max_off_denom = denom;
             max_off_pi = pi;
        }
#else
        double off = n_inv;
        off /= (n0 + 1);
        off /= ((N_PROLEMS - n0) + 1);
        if (max_off < off)
        {
            max_off = off;
            max_off_pi = pi;
        }
#endif
    }
    solution = max_off_pi + 1;
}

void Cheating::set_players_problems()
{
    players_problems.reserve(N_PLAYERS);
    for (const string& line: lines)
    {
        players_problems.push_back(vb_t(size_t(N_PROLEMS), false));
        vb_t& problems = players_problems.back();
        for (u_t i = 0; i < N_PROLEMS; ++i)
        {
            problems[i] = line[i] == '1';
        }
    }
}

void Cheating::sum_problems()
{
    problems_sums = vu_t(N_PROLEMS, 0);
    for (const vb_t& player_problems: players_problems)
    {
        for (u_t iproblem = 0; iproblem < N_PROLEMS; ++iproblem)
        {
            problems_sums[iproblem] += b2i(player_problems[iproblem]);
        }
    }
}

double sigmoid_indef_integral(double x)
{
    double y = log(1. + exp(x));
    return y;
}

double sigmoid_integral(double a, double b)
{
    double ret = sigmoid_indef_integral(b) - sigmoid_indef_integral(a);
    return ret;
}

double sigmoid_average(double a, double b)
{
    double ret = sigmoid_integral(a, b) / (b - a);
    return ret;
}

double inverse_average(double average, double low=-3., double high=3.)
{
    // double low = -3., high = 3.;
    double delta = high - low;
    double old_delta = delta + 1.;
    while (delta < old_delta)
    {
        old_delta = delta;
        double mid = (low + high)/2.;
        double y = sigmoid_average(mid - 3., mid + 3.);
        if (y > average)
        {
            high = mid;
        }
        else
        {
            low = mid;
        }
        delta = high - low;
    }
    return low;
}

double Cheating::compute_difficulty(double average) const
{
    return -inverse_average(average);
}

double Cheating::compute_skill(double average) const
{
    return inverse_average(average, -3., 6.);
}

void Cheating::compute_g_difficulties()
{
    g_difficulties.reserve(N_PROLEMS);
    const double dn = 1./double(N_PLAYERS);
    for (u_t iproblem = 0; iproblem < N_PROLEMS; ++iproblem)
    {
        u_t s = problems_sums[iproblem];
        const double a = double(s) * dn;
        const double difficulty = compute_difficulty(a);
        if (dbg_flags & 0x4) { cerr << "Problem["<<iproblem <<
            "], s="<<s << ", difficulty = "<<difficulty << '\n'; }
        g_difficulties.push_back(difficulty);
    }
}

double Cheating::player_distance(double& skill, u_t pi, u_t score) const
{
    static double max_diff_diff = 0.;
    const vb_t& player_problems = players_problems[pi];
    vd_t problems_average; problems_average.reserve(N_PROLEMS);
    vd_t difficulties; difficulties.reserve(N_PROLEMS);
    const double denom = 1. / double(N_PLAYERS - 1);
    for (u_t iproblem = 0; iproblem < N_PROLEMS; ++iproblem)
    {
        u_t player_problem_score = b2i(player_problems[iproblem]);
        double a = double(problems_sums[iproblem] - player_problem_score);
        a *= denom;
        problems_average.push_back(a);
        const double difficulty = compute_difficulty(a);
        const double diff_diff = abs(difficulty - g_difficulties[iproblem]);
        if (max_diff_diff < diff_diff)
        {
            max_diff_diff = diff_diff;
            if (dbg_flags & 0x2) { cerr << "pi="<<pi<<", iproblem="<<iproblem <<
                 ", diff_diff="<<diff_diff << '\n'; }
        }
        difficulties.push_back(difficulty);
    }
    skill = compute_skill(double(score) / double(N_PROLEMS));
    double dist = 0.;
    for (u_t iproblem = 0; iproblem < N_PROLEMS; ++iproblem)
    {
        u_t zo = b2i(player_problems[iproblem]);
        const double difficulty = difficulties[iproblem];
        const double probability = sigmoid(skill, difficulty);
        double delta = probability - double(zo);
        // delta *= difficulty;
        dist += delta*delta;
    }
    if (dbg_flags & 0x1) { 
        cerr << "pi="<<pi << ", skill="<<skill << ", dist="<<dist <<'\n'; }

    return dist;
}

void Cheating::print_solution(ostream &fo) const
{
    fo << ' ' << solution;
}

int main(int argc, char ** argv)
{
    const string dash("-");

    bool naive = false;
    bool tellg = false;
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
    u_t percentage;
    *pfi >> n_cases >> percentage;
    getline(*pfi, ignore);

    void (Cheating::*psolve)() =
        (naive ? &Cheating::solve_naive : &Cheating::solve);
    ostream &fout = *pfo;
    ul_t fpos_last = pfi->tellg();
    for (unsigned ci = 0; ci < n_cases; ci++)
    {
        Cheating cheating(*pfi, percentage);
        getline(*pfi, ignore);
        if (tellg)
        {
            ul_t fpos = pfi->tellg();
            cerr << "Case (ci+1)="<<(ci+1) << ", tellg=[" <<
                fpos_last << " " << fpos << ") size=" <<
                (fpos - fpos_last) << "\n";
            fpos_last = fpos;
        }

        (cheating.*psolve)();
        fout << "Case #"<< ci+1 << ":";
        cheating.print_solution(fout);
        fout << "\n";
        fout.flush();
    }

    if (pfi != &cin) { delete pfi; }
    if (pfo != &cout) { delete pfo; }
    return 0;
}
