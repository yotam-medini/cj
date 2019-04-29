// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com --

#include <iostream>
#include <fstream>
#include <string>
// #include <set>
// #include <map>
#include <vector>
#include <array>
#include <utility>

#include <cstdlib>
// #include <gmpxx.h>

using namespace std;

// typedef mpz_class mpzc_t;
typedef unsigned u_t;
typedef unsigned long ul_t;
typedef unsigned long long ull_t;
// typedef vector<ul_t> vul_t;

typedef array<int, 2> i2_t;
typedef vector<i2_t> vi2_t;
class Person
{
 public:
    Person(int x=0, int y=0, char vdir='e') : pt({x, y}), dir(vdir) {}
    i2_t pt;
    char dir;
};
typedef vector<Person> vperson_t;

static unsigned dbg_flags;

class Cart
{
 public:
    Cart(istream& fi);
    void solve_naive();
    void solve();
    void print_solution(ostream&) const;
 private:
    int dirmin(u_t xyi, char d) const;
    int dirmax(u_t xyi, char d) const;
    u_t n_ptok(const i2_t& pt) const;
    u_t p;
    int q;
    vperson_t persons;
    i2_t solution;
};

Cart::Cart(istream& fi) : solution({0, 0})
{
    fi >> p >> q;
    for (u_t i = 0; i < p; ++i)
    {
        int x, y;
        char c;
        fi >> x >> y >> c;
        Person person(x, y, c);
        persons.push_back(person);
    }
}

#if 0
void Cart::solve_naive()
{
    int xmin = dirmin(0, 'E');
    // int xmax = dirmin(0, 'W');
    int ymin = dirmin(1, 'N');
    // int ymax = dirmin(0, 'S');
    solution[0] = xmin + 1;
    solution[1] = ymin + 1;
}
#endif

void Cart::solve_naive()
{
    u_t n_best = 0;
    for (int y = 0; y <= q; ++y)
    {
        for (int x = 0; x <= q; ++x)
        {
            const i2_t pt({x, y});
            u_t n = n_ptok(pt);
            if (n_best < n)
            {
                n_best = n;
                solution = pt;
            }
        }
    }
}

int Cart::dirmin(u_t xyi, char d) const
{
    int ret = -1;
    for (u_t i = 0; i < p; ++i)
    {
        const Person &person = persons[i];
        if ((person.dir == d) && (ret < person.pt[xyi]))
        {
            ret = person.pt[xyi];
        }
    }
    return ret;
}

int Cart::dirmax(u_t xyi, char d) const
{
    int ret = q + 1;
    for (u_t i = 0; i < p; ++i)
    {
        const Person &person = persons[i];
        if ((person.dir == d) && (ret > person.pt[xyi]))
        {
            ret = person.pt[xyi];
        }
    }
    return ret;
}

u_t Cart::n_ptok(const i2_t& pt) const
{
    u_t n = 0;
    for (u_t i = 0; i < p; ++i)
    {
        const Person &person = persons[i];
        switch (person.dir)
        {
         case 'E': 
            if (person.pt[0] < pt[0])
            {
                ++n;
            }
            break;
         case 'W': 
            if (person.pt[0] > pt[0])
            {
                ++n;
            }
            break;
         case 'N': 
            if (person.pt[1] < pt[1])
            {
                ++n;
            }
            break;
         case 'S': 
            if (person.pt[1] > pt[1])
            {
                ++n;
            }
            break;
        }
    }
    return n;
}

void Cart::solve()
{
    solve_naive();
}

void Cart::print_solution(ostream &fo) const
{
    fo << ' ' << solution[0] << ' ' << solution[1];
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
    *pfi >> n_cases;
    getline(*pfi, ignore);

    void (Cart::*psolve)() =
        (naive ? &Cart::solve_naive : &Cart::solve);
    ostream &fout = *pfo;
    ul_t fpos_last = pfi->tellg();
    for (unsigned ci = 0; ci < n_cases; ci++)
    {
        Cart cart(*pfi);
        getline(*pfi, ignore);
        if (tellg)
        {
            ul_t fpos = pfi->tellg();
            cerr << "Case (ci+1)="<<(ci+1) << ", tellg=[" <<
                fpos_last << " " << fpos << ") size=" <<
                (fpos - fpos_last) << "\n";
            fpos_last = fpos;
        }

        (cart.*psolve)();
        fout << "Case #"<< ci+1 << ":";
        cart.print_solution(fout);
        fout << "\n";
        fout.flush();
    }

    if (pfi != &cin) { delete pfi; }
    if (pfo != &cout) { delete pfo; }
    return 0;
}
