#include <iostream>
#include <iomanip>
#include <vector>
#include <cstdlib>

using namespace std;

typedef unsigned u_t;

enum {N = 20, B = 15, NB = N*B};

int thresholds[N + 1][NB + 1];
double expectations[N + 1][NB + 1];

static void init()
{
    for (u_t a = 0; a <= N; ++a)
    {
        for (u_t d = 0; d <= NB + 1; ++d)
        {
            thresholds[a][d] = -2;
            expectations[a][d] = -2.;
        }
    }
}


static u_t na(u_t a, u_t d)
{
    u_t a_next = ((d - a) % B == 2) ? a + 1 : a;
    return a_next;
}

static double e(u_t a, u_t d); // forward

static bool possible(u_t a, u_t d)
{
    bool ret = true;
    if (d < a)
    {
        ret = false;
    }
    else if (((d - a) % B) == 1)
    {
        ret = false;
    }
    return ret;
}

static int t(u_t a, u_t d)
{
    int ret = thresholds[a][d];
    if (ret < -1)
    {
        if (!possible(a, d))
        {
            ret = -1;
        }
        else if (a == 0) 
        {
            ret = 9;
        }
        else if (d <= a)
        {
            ret = 0;
        }
        else
        {
            double exp_best = 0.;
            int t_best = -1;
            for (int _t = 0; _t <= 9; ++_t)
            {
                double pfill = (10. - _t)/10.;
                double avg = (_t + 9.)/2.;
                u_t a_next = na(a, d);
                double exp_fill = pfill * (avg + e(a - 1, d - 1));
                double exp_next = (1. - pfill) * e(a_next, d - 1);
                double texp = exp_fill + exp_next;
                if (exp_best < texp)
                {
                    exp_best = texp;
                    t_best = _t;
                }
            }
            ret = t_best;
        }
        thresholds[a][d] = ret;
    }
    return ret;
}


static double e(u_t a, u_t d)
{
    double ret = expectations[a][d];
    if (ret < -1.)
    {
        if (!possible(a, d))
        {
            ret = -1;
        }
        else if ((d == 0) || (d < a))
        {
            ret = 0;
        }
        else if (a == 0)
        {
            u_t a_next = na(a, d);
            ret = e(a_next, d - 1);
        }
        else
        {
            int tad = t(a, d);
            double avg = (tad + 9.)/2.;
            u_t a_next = na(a, d);
            double pfill = (10. - tad) / 10.;
            double exp_fill_alt = e(a - 1, d - 1);
            double exp_fill = pfill * (avg + exp_fill_alt);
            double exp_next_alt = e(a_next, d - 1);
            double exp_next = (1. - pfill) * exp_next_alt;
            ret = exp_fill + exp_next;
        }
        expectations[a][d] = ret;
    }
    return ret;
}

static int short_output(int argc, char** argv)
{
    int rc = 0;
    int ai = 1;
    u_t a_max = strtoul(argv[ai++], 0, 0);
    u_t d_max = strtoul(argv[ai++], 0, 0);
    cout << "Expectations:\n";
    for (u_t a = 0; a <= a_max; ++a)
    {
        cout << "a[" << a << "]";
        for (u_t d = 0; d <= d_max; ++d)
        {
            if (d % B == 0) { cout << ' '; }
            double _e = e(a, d);
            cout << ' ' << setw(4) << _e;
        }
        cout << "\n";
    }
    cout << "\nThresholdss;\n";
    for (u_t a = 0; a <= a_max; ++a)
    {
        cout << "a[" << a << "]";
        for (u_t d = 0; d <= d_max; ++d)
        {
            if (d % B == 0) { cout << ' '; }
            int _t = t(a, d);
            cout << ' ' << _t;
        }
        cout << "\n";
    }
    return rc;
}

static int long_output()
{
    for (u_t a = 0; a <= N; ++a)
    {
        for (u_t d = 0; d <= NB; ++d)
        {
            cout << "[" << setw(2) << a << "][" << setw(2) << d << "]"
                " T=" << t(a, d) << ", tE=" << e(a, d) << '\n';
        }
    }
    return 0;
}

int main(int argc, char** argv)
{
    int rc = 0;
    init();
    if (argc == 3)
    {
        rc = short_output(argc, argv);
    }
    else
    {
        rc = long_output();
    }
    return rc;
}

