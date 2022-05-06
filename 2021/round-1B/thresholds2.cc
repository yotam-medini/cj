#include <iostream>
#include <iomanip>
#include <vector>
#include <cstdlib>

using namespace std;

typedef unsigned u_t;

class TeState
{
 public:
    TeState(u_t _m1=0, u_t _m2=0, u_t _pending=0) :
        m1(_m1), m2(_m2), pending(_pending) {}
    void fill_m1()
    {
        --m1;
        --pending;
    }
    void fill_m2()
    {
        ++m1;
        --m2;
        --pending;
    }
    u_t m1, m2, pending;
};

class TeResult
{
 public:
    TeResult(u_t tm1=0, u_t tm2=0, double e=0.) :
        threshold_m1(tm1), threshold_m2(tm2), expect(e) {}
    u_t threshold_m1;
    u_t threshold_m2;
    double expect;
}; 
typedef vector<TeResult> vthr_t;
typedef vector<vthr_t> vvthr_t;

class ThresholdEngine
{
 public:
    // enum {N = 20, B = 15, NB = N*B};
    ThresholdEngine(u_t _N=20, u_t _B=15) :
        N(_N), B(_B), NB(_N*_B),
        pm1m2(NB + 1, vvthr_t())
    { 
        build(); 
    }
    const TeResult& get_threshold(u_t pending, u_t m1, u_t m2) const
    {
        return pm1m2[pending][m1][m2];
    }
    const TeResult& get_state_result(const TeState s) const
    {
        const TeResult& r =  pm1m2[s.pending][s.m1][s.m2];
        return r;
    }
    int report(
        ostream& os=cerr, 
        u_t m1_max=0, u_t m2_max=0, u_t pending_max=0) const;
    int report_default(ostream& os=cerr) const
    {
        return report(os, N, B, NB);
    }
 private:
    void build();
    bool state_is_possible(const TeState& state) const;
    void get_next_state(TeState& next, const TeState& state) const;
    void ti_must_fill(TeResult& ti, const TeState& state);
    void ti_may_fill(TeResult& ti, const TeState& state);
    void ti_m1(TeResult& ti, const TeState& state);
    void ti_m2(TeResult& ti, const TeState& state);

    // vvthr_t pm1m2[NB + 1]; // indexed [pending][#m1][#m2]
    const u_t N;
    const u_t B;
    const u_t NB;
    vector<vvthr_t> pm1m2; // indexed [pending][#m1][#m2]
};

void ThresholdEngine::build()
{
    for (u_t pending = 0; pending <= NB; ++pending)
    {
        const u_t m1_max = min<u_t>(N, pending);
        vvthr_t& m1_infos = pm1m2[pending];
        for (u_t m1 = 0; m1 <= m1_max; ++m1)
        {
            const u_t m2_max = min<u_t>(N - m1, pending - m1);
            vthr_t m2_infos; m2_infos.reserve(m2_max + 1);
            for (u_t m2 = 0; m2 <= m2_max; ++m2)
            {
                const TeState state{m1, m2, pending};
                // default values for (pending == m1 + 2*m2)
                TeResult ti(0, 0, (10*m1 + m2) * (0. + 9.)/2); 
                if (!state_is_possible(state))
                {
                     ti = TeResult(); // impossible state
                }
                else if (pending == m1 + 2*m2)
                {
                    if ((m1 > 0) && (m2 > 0))
                    {
                        ti_must_fill(ti, state);
                    }
                }
                else // (pending > m1 + m2)
                {
                    if ((m1 > 0) && (m2 > 0))
                    {
                        ti_may_fill(ti, state);
                    }
                    else if (m1 > 0)
                    {
                        ti_m1(ti, state);
                    }
                    else if (m2 > 0)
                    {
                        ti_m2(ti, state);
                    }
                }
                m2_infos.push_back(ti);
            }
            m1_infos.push_back(m2_infos);
        }
    }
}

bool ThresholdEngine::state_is_possible(const TeState& s) const
{
    bool possible = (s.m1 + 2*s.m2 <= s.pending) &&
        (s.pending && s.pending <= NB);
    if (possible)
    {
        u_t next_non_m1m2_lack = (s.pending - (s.m1 + 2*s.m2)) % B;
        u_t next_non_m1m2_height = (B - next_non_m1m2_lack) % B;
        possible = next_non_m1m2_height < B - 2;
    }
    return possible;
}

void ThresholdEngine::get_next_state(TeState& next, const TeState& state) const
{
    next = state;
    --next.pending;
    u_t next_non_m1m2_lack = (next.pending - (next.m1 + 2*next.m2)) % B;
    u_t next_non_m1m2_height = (B - next_non_m1m2_lack) % B;
    if (next_non_m1m2_height == B - 2)
    {
        ++next.m2;
    }
}

void ThresholdEngine::ti_must_fill(TeResult& ti, const TeState& state)
{
    double expect_max = 0.;
    TeState state_m1(state); --state_m1.m1;
    TeState state_m2(state); --state_m2.m2;
    for (u_t d1 = 1; d1 <= 9; ++d1)
    {
        const double p1_fill = (10 - d1)/10.;
        const double v1_fill = 10*(d1 + 9.)/2;
        const double e1_fill = p1_fill * v1_fill;
        const u_t d2 = 0;
        {
            const double p2_fill = 1. - p1_fill;
            const double v2_fill = (d2 + (d1 - 1.))/10.;
            const double e2_fill = p2_fill * v2_fill;
            const double e_d1d2 = e1_fill + e2_fill;
            if (expect_max < e_d1d2)
            {
                expect_max = e_d1d2;
                ti = TeResult(d1, d2, e_d1d2);
            }
        }
    }
}

void ThresholdEngine::ti_may_fill(TeResult& ti, const TeState& state)
{
    double expect_max = 0.;
    TeState state_m1(state);
    state_m1.fill_m1();
    const double e_state_m1 = get_state_result(state_m1).expect;
    TeState state_m2(state);
    state_m2.fill_m2();
    const double e_state_m2 = get_state_result(state_m2).expect;
    TeState state_next;
    get_next_state(state_next, state);
    const double vnext = get_state_result(state_next).expect;
    for (u_t d1 = 1; d1 <= 9; ++d1)
    {
        const double p1_fill = (10 - d1)/10.;
        const double v1_fill = 10*(d1 + 9.)/2 + e_state_m1;
        const double e1_fill = p1_fill * v1_fill;
        for (u_t d2 = 0; d2 < d1; ++d2)
        {
            const double p2_fill = double(d1 - d2)/10.;
            const double v2_fill = (d2 + (d1 - 1.))/2. + e_state_m2;
            const double e2_fill = p2_fill * v2_fill;
            const double p_next = 1. - (p1_fill + p2_fill);
            const double e_next = p_next * vnext;
            const double e_d1d2 = e1_fill + e2_fill + e_next;
            if (expect_max < e_d1d2)
            {
                expect_max = e_d1d2;
                ti = TeResult(d1, d2, e_d1d2);
            }
        }
    }
}

void ThresholdEngine::ti_m1(TeResult& ti, const TeState& state)
{
    double expect_max = 0.;
    TeState state_m1(state);
    state_m1.fill_m1();
    const double e_state_m1 = get_state_result(state_m1).expect;
    TeState state_next;
    get_next_state(state_next, state);
    const double vnext = get_state_result(state_next).expect;
    for (u_t d1 = 0; d1 <= 9; ++d1)
    {
        const double p1_fill = (10 - d1)/10.;
        const double v1_fill = 10*(d1 + 9.)/2 + e_state_m1;
        const double e1_fill = p1_fill * v1_fill;
        {
            const double p_next = 1. - p1_fill;
            const double e_next = p_next * vnext;
            const double e_d1 = e1_fill + e_next;
            if (expect_max < e_d1)
            {
                expect_max = e_d1;
                ti = TeResult(d1, 0, e_d1);
            }
        }
    }
}

void ThresholdEngine::ti_m2(TeResult& ti, const TeState& state)
{
    double expect_max = 0.;
    TeState state_m2(state);
    state_m2.fill_m2();
    double e_state_m2 = get_state_result(state_m2).expect;
    TeState state_next;
    get_next_state(state_next, state);
    const double vnext = get_state_result(state_next).expect;
    {
        for (u_t d2 = 0; d2 <= 9; ++d2)
        {
            const double p2_fill = double(10 - d2)/10.;
            const double v2_fill = (d2 + 9.)/2. + e_state_m2;
            const double e2_fill = p2_fill * v2_fill;
            const double p_next = 1. - p2_fill;
            const double e_next = p_next * vnext;
            const double e_d2 = e2_fill + e_next;
            if (expect_max < e_d2)
            {
                expect_max = e_d2;
                ti = TeResult(0, d2, e_d2);
            }
        }
    }
}

int ThresholdEngine::report(
   ostream& os,
   u_t c_m1_max, 
   u_t c_m2_max,
   u_t c_pending_max) const
{
    for (u_t m1 = 0; m1 <= c_m1_max; ++m1)
    {
        const u_t m2_max = min((N - m1), c_m2_max);
        for (u_t m2 = 0; m2 <= m2_max; ++m2)
        {
            u_t pending_max = min(NB - (m1 + m2)*B, c_pending_max);
            for (u_t pending = m1 + 2*m2; pending <= pending_max; ++pending)
            {
                const TeResult& ti = get_threshold(pending, m1, m2);
                cout << "[" << setw(2) << m1 << "][" << setw(2) << m2 << "][" <<
                    setw(3) << pending << "] T1=" << ti.threshold_m1 <<
                    ", T2=" << ti.threshold_m2 << ", E=" << ti.expect << "\n";
            }
        }
    }
    return 0;
}

#if defined(TEST_TE)
static int short_output(const ThresholdEngine& te, int argc, char** argv)
{
    int ai = 1;
    u_t m1_max = strtoul(argv[ai++], 0, 0);
    u_t m2_max = strtoul(argv[ai++], 0, 0);
    u_t pending_max = strtoul(argv[ai++], 0, 0);
    te.report(cout, m1_max, m2_max, pending_max);
    return 0;
}

int main(int argc, char** argv)
{
    int rc = 0;
    ThresholdEngine te;
    if (argc == 4)
    {
        rc = short_output(te, argc, argv);
    }
    else
    {
        rc = te.report_default(cout);
    }
    return rc;
}
#endif
