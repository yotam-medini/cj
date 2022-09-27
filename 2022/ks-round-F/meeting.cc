// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com --

#include <algorithm>
#include <array>
#include <fstream>
#include <iostream>
#include <numeric>
#include <string>
#include <tuple>
#include <utility>
#include <vector>

#include <cstdlib>

using namespace std;

typedef unsigned u_t;
typedef unsigned long ul_t;
typedef unsigned long long ull_t;
typedef vector<u_t> vu_t;
typedef vector<vu_t> vvu_t;
typedef array<u_t, 2> time_evi_t;
typedef vector<time_evi_t> vtime_evi_t;

static unsigned dbg_flags;

void combination_first(vu_t &c, u_t n, u_t k)
{
    c = vu_t(k);
    iota(c.begin(), c.end(), 0);
}

bool combination_next(vu_t &c, u_t n)
{
    u_t j = 0, k = c.size();

    // sentinels (Knuth) (Why the second (0) ??!)
    c.push_back(n);  c.push_back(0);

    while ((j < k) && (c[j] + 1 == c[j + 1]))
    {
        c[j] = j;
        ++j;
    }
    bool ret = j < k;
    if (ret)
    {
        ++(c[j]);
    }

    c.pop_back(); c.pop_back(); // the sentinels

    return ret;
}

class PreMeet
{
 public:
    PreMeet(u_t p=0, u_t l=0, u_t r=0): P(p), L(l), R(r) {}
    u_t P, L, R;
};
typedef vector<PreMeet> vpremeet_t;

class Event
{
 public:
    Event(u_t _t=0, bool b=false, u_t l=0) : t(_t), begin(b), lead(l) {}
    u_t t;
    bool begin; // end=0 < begin=1
    u_t lead;
};
bool operator<(const Event& e0, const Event& e1)
{
#if 1
    typedef tuple<u_t, bool, u_t> ubu_t;
    bool lt = ubu_t{e0.t, e0.begin, e0.lead} < ubu_t{e1.t, e1.begin, e1.lead};
#else
    bool lt = false;
    if (e0.t < e1.t)
    {
        lt = true;
    }
    else if (e0.t == e1.t)
    {
        if (e0.begin < e1.begin)
        {
            lt = true;
        }
        else if (e0.begin == e1.begin)
        {
            if (e0.lead < e1.lead)
            {
                lt = true;
            }
        }
    }
#endif
    return lt;
}
typedef vector<Event> vevent_t;

class Meeting
{
 public:
    Meeting(istream& fi);
    Meeting(const vu_t&) {}; // TBD for test_case
    void solve_naive();
    void solve();
    void print_solution(ostream&) const;
    u_t get_solution() const { return solution; }
 private:
    void set_events();
    void initial_window();
    void initial_state();
    u_t N, K, X, D;
    u_t M;
    vpremeet_t pre_meetings;
    // non-naive
    u_t lead_free;
    u_t cancel;
    vevent_t events;
    vtime_evi_t window_event;
    vu_t lead_meetings;
    vu_t multi_meet;
    vu_t multi_cancel;

    u_t solution;
};

Meeting::Meeting(istream& fi) : 
     lead_free(0), cancel(0),
     solution(0)
{
    fi >> N >> K >> X >> D;
    fi >> M;
    pre_meetings.reserve(M);
    while (pre_meetings.size() < M)
    {
        u_t p, l, r;
        fi >> p >> l >> r;
        pre_meetings.push_back(PreMeet(p, l, r));
    }
}

static void min_by(u_t& v, u_t x)
{
    if (v > x)
    {
        v = x;
    }
}

void Meeting::solve_naive()
{
    solution = M;
    vu_t comb;
    combination_first(comb, N, K);
    for (bool more = true; more; more = combination_next(comb, N))
    {
        for (u_t hb = 0, he = X; he <= D; ++hb, ++he)
        {
            u_t n_cancel = 0;
            for (u_t team_lead: comb)
            {
                for (const PreMeet& pm: pre_meetings)
                {
                    if (pm.P - 1 == team_lead)
                    {
                        bool is_free = (pm.R <= hb) || (he <= pm.L);
                        if (!is_free)
                        {
                            ++n_cancel;
                        }
                    }
                }
            }
            min_by(solution, n_cancel);
        }
    }
}

void Meeting::solve()
{
    solution = M;
    set_events();
    initial_window();
    initial_state();
    for (const time_evi_t& time_evi: window_event)
    {
        const u_t ei = time_evi[1];
        const Event& event = events[ei];
        if (event.begin)
        {
            if (lead_meetings[event.lead] == 0)
            {
                --lead_free;
                if (lead_free < K)
                {
                    
                }
            }
        }
        else // end
        {
            ;
        }
    }
}

void Meeting::set_events()
{
    events.reserve(2*M);
    for (const PreMeet& pm: pre_meetings)
    {
        events.push_back(Event(pm.L, true, pm.P - 1));
        events.push_back(Event(pm.R, false, pm.P - 1));
    }
    sort(events.begin(), events.end());
    for (u_t ei = 0; ei < events.size(); ++ei)
    {
        const Event& event = events[ei];
        if (event.begin)
        {
            if (X <= event.t)
            {
                window_event.push_back(time_evi_t{event.t - X, ei});
            }
        }
        else // end
        {
            if (event.t < D + X)
            {
                window_event.push_back(time_evi_t{event.t, ei});
            }
        }
    }
    sort(window_event.begin(), window_event.end());
    lead_meetings = vu_t(N, 0);
    multi_meet = vu_t(M + 1, 0);
    multi_cancel = vu_t(M + 1, 0);
}

void Meeting::initial_window()
{
    lead_free = N;
    u_t ei;
    for (ei = 0; (ei < events.size()) && (events[ei].t < X); ++ei)
    {
        const Event& event = events[ei];
        if (event.begin)
        {
            u_t nm = lead_meetings[event.lead];
            if (nm == 0)
            {
                --lead_free;
            }
            else
            {
                --multi_meet[nm];
            }
            ++nm;
            lead_meetings[event.lead] = nm;
            ++multi_meet[nm];
        }
    }
}

void Meeting::initial_state()
{
    cancel = 0;
    if (lead_free >= K)
    {
        min_by(solution, 0);
    }
    else
    {
        u_t missing = K - lead_free;
        for (u_t mi = 1; missing > 0; ++mi)
        {
            if (multi_meet[mi] > 0)
            {
                u_t d = min(missing, multi_meet[mi]);
                missing -= d;
                cancel += d * mi;
                multi_cancel[mi] += d;
            }
        }
    }
    min_by(solution, cancel);
}

void Meeting::print_solution(ostream &fo) const
{
    fo << ' ' << solution;
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

    void (Meeting::*psolve)() =
        (naive ? &Meeting::solve_naive : &Meeting::solve);
    if (solve_ver == 1) { psolve = &Meeting::solve; } // solve1
    ostream &fout = *pfo;
    ul_t fpos_last = pfi->tellg();
    for (unsigned ci = 0; ci < n_cases; ci++)
    {
        Meeting meeting(*pfi);
        getline(*pfi, ignore);
        if (tellg)
        {
            ul_t fpos = pfi->tellg();
            cerr << "Case (ci+1)="<<(ci+1) << ", tellg=[" <<
                fpos_last << " " << fpos << ") size=" <<
                (fpos - fpos_last) << "\n";
            fpos_last = fpos;
        }

        (meeting.*psolve)();
        fout << "Case #"<< ci+1 << ":";
        meeting.print_solution(fout);
        fout << "\n";
        fout.flush();
    }

    if (pfi != &cin) { delete pfi; }
    if (pfo != &cout) { delete pfo; }
    return 0;
}

static u_t rand_range(u_t nmin, u_t nmax)
{
    u_t r = nmin + rand() % (nmax + 1 - nmin);
    return r;
}

static int test_case(int argc, char ** argv)
{
    int rc = rand_range(0, 1);
    ull_t solution(-1), solution_naive(-1);
    bool small = rc == 0;
    if (small)
    {
        Meeting p{vu_t()};
        p.solve_naive();
        solution_naive = p.get_solution();
    }
    {
        Meeting p{vu_t()};
        p.solve();
        solution = p.get_solution();
    }
    if (small && (solution != solution_naive))
    {
        rc = 1;
        cerr << "Failed: solution="<<solution << " != " <<
            solution_naive << " = solution_naive\n";
        ofstream f("meeting-fail.in");
        f << "1\n";
        f.close();
    }
    return rc;
}

static int test_random(int argc, char ** argv)
{
    int rc = 0;
    int ai = 0;
    u_t n_tests = strtoul(argv[ai++], 0, 0);
    for (u_t ti = 0; (rc == 0) && (ti < n_tests); ++ti)
    {
        cerr << "Tested: " << ti << '/' << n_tests << '\n';
        rc = test_case(argc, argv);
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
