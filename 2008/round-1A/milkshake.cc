// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com -- Created: 2013/April/20

#include <iostream>
#include <fstream>
#include <string>
#include <set>
#include <vector>
#include <utility>

#include <cstdlib>

using namespace std;

typedef unsigned long ul_t;
typedef vector<bool> vb_t;
typedef set<unsigned> setu_t;

static unsigned dbg_flags;

class Customer
{
 public:
    Customer() : malted(-1) {}
    setu_t flavors;
    int malted;
};
typedef vector<Customer> vcustomer_t;

class Problem
{
 public:
    Problem(istream& fi);
    void solve();
    void print_solution(ostream&) const;
 private:
    unsigned N;
    unsigned M;
    vcustomer_t customers;
    bool possible;
    vb_t solution;
};

Problem::Problem(istream& fi) : possible(true)
{
    fi >> N;
    fi >> M;
    for (unsigned c = 0; c < M; ++c)
    {
        Customer customer;
        setu_t &flavors = customer.flavors;
        unsigned nt;
        fi >> nt;
        for (unsigned ti = 0; ti < nt; ++ti)
        {
            unsigned t, ft;
            fi >> t >> ft;
            flavors.insert(flavors.end(), t - 1);
            if (ft == 1)
            {
                customer.malted = t - 1;
            }
        }
        customers.push_back(customer);
    }
}


void Problem::solve()
{
    solution = vb_t(vb_t::size_type(N), false);
    vcustomer_t waiting(customers);
    bool reducing = true;
    while (reducing)
    {
        reducing = false;
        for (unsigned ci = 0; ci < waiting.size(); )
        {
            const Customer &customer = waiting[ci];
            if ((customer.flavors.size() == 1) && (customer.malted != -1))
            {
                reducing = true;
                unsigned f = customer.malted;
                solution[f] = true;
                swap(waiting[ci], waiting.back());
                waiting.pop_back();
                for (unsigned wi = 0; wi < waiting.size(); ++wi)
                {
                     Customer &w = waiting[wi];
                     if (w.malted != int(f))
                     {
                         w.flavors.erase(setu_t::key_type(f)); // may do nothing
                     }
                }
            }
            else
            {
                ++ci;
            }
        }
    }

    for (auto ci = customers.begin(); possible && (ci != customers.end()); ++ci)
    {
        const Customer &customer = *ci;
        const setu_t &flavors = customer.flavors;
        int malted = customer.malted;
        bool found = false;
        for (auto fi = flavors.begin(); (fi != flavors.end()) && !found; ++fi)
        {
            unsigned f = *fi;
            found = (malted == int(f) ? solution[f] : !solution[f]);
        }
        if (!found)
        {
            possible = false;
        }
    }
}

void Problem::print_solution(ostream &fo) const
{
    if (possible)
    {
        for (auto si = solution.begin(); si != solution.end(); ++si)
        {
            bool b = *si;
            fo << " " << (b ? '1' : '0');
        }
    }
    else
    {
        fo << " IMPOSSIBLE";
    }
}

int main(int argc, char ** argv)
{
    const string dash("-");

    istream *pfi = (argc < 2 || (string(argv[1]) == dash))
         ? &cin
         : new ifstream(argv[1]);
    ostream *pfo = (argc < 3 || (string(argv[2]) == dash))
         ? &cout
         : new ofstream(argv[2]);

    if ((!pfi) || (!pfo) || pfi->fail() || pfo->fail())
    {
        cerr << "Open file error\n";
        exit(1);
    }

    if (argc > 3) { dbg_flags = strtoul(argv[3], 0, 0); }
    // bool large = (argc > 4) && !strcmp(argv[4], "-large");

    unsigned n_cases;
    *pfi >> n_cases;

    ostream &fout = *pfo;
    for (unsigned ci = 0; ci < n_cases; ci++)
    {
        Problem problem(*pfi);
        // cerr << "Case ci="<<ci << " (ci+1)="<<ci+1 << "\n";
        problem.solve();
        fout << "Case #"<< ci+1 << ":";
        problem.print_solution(fout);
        fout << "\n";
        fout.flush();
        
    }
    
    if (pfi != &cin) { delete pfi; }
    if (pfo != &cout) { delete pfo; }
    return 0;
}

