// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com --

#include <iostream>
#include <fstream>
#include <string>
#include <array>
#include <map>
#include <vector>
#include <list>
#include <utility>

#include <cstdlib>

using namespace std;

typedef unsigned u_t;
typedef unsigned long ul_t;
typedef unsigned long long ull_t;
typedef array<u_t, 2> u2_t;
typedef vector<bool> vb_t;
typedef vector<vb_t> vvb_t;
typedef list<u_t> listu_t;
typedef listu_t::iterator listui_t;
typedef listu_t::reverse_iterator listuri_t;
typedef vector<listu_t> vlistu_t;
typedef map<u_t, u_t> u2u_t;

static unsigned dbg_flags;

static listu_t dum_list;
static listui_t dum_iter = dum_list.end();
static listuri_t dum_riter = dum_list.rend();

class Square
{
 public:
   Square(listui_t vri=dum_iter, listui_t vci=dum_iter,
       listuri_t vrri=dum_riter, listuri_t vrci=dum_riter) : 
       ri(vri), ci(vci), rri(vrri), rci(vrci)
       {}
   listui_t ri, ci;
   listuri_t rri, rci;
};
typedef vector<Square> vsqr_t;
typedef vector<vsqr_t> vvsqr_t;


class Intervals
{
 public:
    u_t add(u_t at, bool up);
    u_t size() const { return b2e.size(); }
    const u2u_t& get_map() const { return b2e; }
 private:
    typedef u2u_t::iterator iter_t;
    typedef u2u_t::reverse_iterator riter_t;
    typedef u2u_t::value_type valt_t;
    u_t extend(iter_t iter, bool up);
    u2u_t b2e; // [begin, end)
};

u_t Intervals::add(u_t at, bool up)
{
    u_t ret = at;
    if (b2e.empty())
    {
        b2e.insert(b2e.end(), valt_t(at, at + 1));
    }
    else
    {
        auto er = b2e.equal_range(at);
        iter_t iter = er.first;
        if (er.first != er.second)
        {
            ret = extend(iter, up);
        }
        else // er.first == er.second
        {
            if (iter == b2e.end())
            {
                riter_t riter(iter);
                valt_t& old = *riter;
                if (old.second < at)
                {
                    b2e.insert(iter, valt_t(at, at + 1));
                }
                else if (old.second == at)
                {
                    ++old.second;
                }
                else
                {
                    ret = extend((++riter).base(), up);
                }
            }
            else if (iter == b2e.begin())
            {
                valt_t& old = *iter;
                if (at + 1 == old.first)
                {
                    b2e.insert(iter, valt_t(at, old.second));
                    b2e.erase(iter);
                }
                else
                {
                    b2e.insert(iter, valt_t(at, at + 1));
                }
            }
            else
            {
                valt_t& old = *iter; 
                riter_t riter(iter);
                valt_t& rold = *riter;
                if (at < rold.second)
                {
                    ret = extend((++riter).base(), up);
                }
                else if (rold.second + 1 == old.first)
                {
                    rold.second = old.second;
                    b2e.erase(iter);
                }
                else if (rold.second == at)
                {
                    ++rold.second;
                }
                else if (at + 1 == old.first)
                {
                    b2e.insert(iter, valt_t(at, old.second));
                    b2e.erase(iter);
                }
                else
                {
                    b2e.insert(iter, valt_t(at, at + 1));
                }
            }
        }
    }
    return ret;
}

u_t Intervals::extend(iter_t iter, bool up)
{
    u_t ret = (up ? iter->second : iter->first - 1);
    if (up)
    {
        iter_t inext(iter);
        ++inext;
        if ((inext == b2e.end()) || ((*iter).second + 1 < (*inext).first))
        {
            valt_t& old = *iter;
            ret = old.second++;
        }
        else
        {
            (*iter).second = (*inext).second;
            b2e.erase(inext);
        }
    }
    else // down
    {
        const valt_t& old = *iter;
        if (iter == b2e.begin())
        {
            b2e.insert(iter, valt_t(ret, old.second));
        }
        else
        {
            riter_t riter(iter);
            valt_t& rold = *riter;
            if (rold.second == ret)
            {
                rold.second = old.second;
            }
            else
            {
                b2e.insert(iter, valt_t(ret, old.second));
            }
        }
        b2e.erase(iter);
    }
    return ret;
}

typedef map<u_t, Intervals> u2intervals_t;

class Wiggle
{
 public:
    Wiggle(istream& fi);
    void solve_naive();
    void solve();
    void print_solution(ostream&) const;
 private:
    u_t find_empty(const vb_t& v, u_t b, int step) const;
    void init_squares();
    u_t sqr_find_empty(Square *psqr, listu_t &l, bool is_row, bool fwd);
    u_t n, r, c, sr, sc;
    u_t orig_sr, orig_sc;
    string moves;
    u_t curr_r, curr_c;

    vlistu_t lrows;
    vlistu_t lcols;
    vvsqr_t squares;

    u2intervals_t xy_intervals[2];
};

Wiggle::Wiggle(istream& fi) : curr_r(0), curr_c(0)
{
    fi >> n >> r >> c >> sr >> sc;

    orig_sr = sr;
    orig_sc = sc;
    // sr = 102; sc = 102;

    fi >> moves;
}

void Wiggle::solve_naive()
{
    vb_t empty_row(vb_t::size_type(210), false);
    vb_t empty_col(vb_t::size_type(2100), false);
    vvb_t rows(vvb_t::size_type(210), empty_row);
    vvb_t cols(vvb_t::size_type(210), empty_col);

    --sr;  --sc; // Dijkstra style
    rows[sr][sc] = true;
    cols[sc][sr] = true;
    curr_r = sr; curr_c = sc;
    for (u_t ci = 0; ci < n; ++ci)
    {
        const char move = moves[ci];
        switch (move)
        {
         case 'N':
            curr_r = find_empty(cols[curr_c], curr_r, -1);
            break;
         case 'S':
            curr_r = find_empty(cols[curr_c], curr_r, +1);
            break;
         case 'W':
            curr_c = find_empty(rows[curr_r], curr_c, -1);
            break;
         case 'E':
            curr_c = find_empty(rows[curr_r], curr_c, +1);
            break;
        }
        rows[curr_r][curr_c] = true;
        cols[curr_c][curr_r] = true;
    }
    ++curr_r; ++curr_c; // un-Dijkstra
}

u_t Wiggle::find_empty(const vb_t& v, u_t b, int step) const
{
    for (b = b + step; v[b]; b += step) {}
    return b;
}

u_t Wiggle::sqr_find_empty(Square *psqr, listu_t &l, bool is_row, bool fwd)
{
    u_t ret = ~0;
    Square &sqr = *psqr;
    if (fwd)
    {
        listui_t i = (is_row ? sqr.ri : sqr.ci), i1 = i;
        ++i1;
        ret = *i1;
        // l.erase(i);
    }
    else
    {
        listuri_t ri = (is_row ? sqr.rri : sqr.rci), ri1 = ri;
        ++ri1;
        ret = *ri1;
        // l.erase(i);
    }
    return ret;
}

void Wiggle::init_squares()
{
    vsqr_t row = vsqr_t(vsqr_t::size_type(c), Square());
    squares = vvsqr_t(vvb_t::size_type(r), row);
    lrows = vlistu_t(vlistu_t::size_type(r), listu_t());
    lcols = vlistu_t(vlistu_t::size_type(c), listu_t());
    for (u_t i = 0; i < r; ++i)
    {
        listu_t &lrow = lrows[i];
        u_t j;
        for (j = 0; j < c; ++j)
        {
            lrow.push_back(j);
        }
        j = 0;
        for (listui_t iter = lrow.begin(); iter != lrow.end(); ++iter, ++j)
        {
            squares[i][j].ri = iter;
        }
        j = c;
        for (listuri_t riter = lrow.rbegin(); riter != lrow.rend(); ++riter)
        {
            --j;
            squares[i][j].rri = riter;
        }
    }
    for (u_t j = 0; j < c; ++j)
    {
        listu_t &lcol = lcols[j];
        u_t i;
        for (i = 0; i < r; ++i)
        {
            lcol.push_back(i);
        }
        i = 0;
        for (listui_t iter = lcol.begin(); iter != lcol.end(); ++iter, ++i)
        {
            squares[i][j].ci = iter;
        }
        i = r;
        for (listuri_t riter = lcol.rbegin(); riter != lcol.rend(); ++riter)
        {
            --i;
            squares[i][j].rci = riter;
        }
    }
}

void Wiggle::solve()
{
    bool dont_care = true;
    u2_t curr({orig_sc, orig_sr});
    u2intervals_t::iterator iter[2];
    for (u_t dim = 0; dim != 2; ++dim)
    {
        Intervals ivals;
	ivals.add(curr[1 - dim], dont_care);
	u2intervals_t&	dint = xy_intervals[dim];
	iter[dim] = dint.insert(dint.end(),
	    u2intervals_t::value_type(curr[dim], ivals));
    }
    for (u_t ci = 0; ci < n; ++ci)
    {
        const char move = moves[ci];
	u_t dim = (((move == 'N') || (move == 'S')) ? 0 : 1);
	bool up = (move == 'S') || (move == 'E'); // increase on south!
	u_t odim = 1 - dim;
	Intervals &ival = (*(iter[dim])).second;
	curr[odim] = ival.add(curr[odim], up);
	u2intervals_t&	odint = xy_intervals[odim];
	auto er = odint.equal_range(curr[odim]);
	iter[odim] = er.first;
	if (er.first == er.second)
	{
	    Intervals oivals;
	    oivals.add(curr[dim], dont_care);
	    // iter[odim] = odint.insert(iter[odim], oivals);
	    iter[odim] = odint.insert(iter[odim],
	    	    u2intervals_t::value_type(curr[odim], oivals));
	}
	else
	{
	    Intervals &oivals = (*(iter[odim])).second;
	    oivals.add(curr[dim], true); // must return curr[dim]
	}
    }
    curr_c = curr[0];
    curr_r = curr[1];
}

void Wiggle::print_solution(ostream &fo) const
{
    fo << ' ' << curr_r << ' ' << curr_c; 
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

    void (Wiggle::*psolve)() =
        (naive ? &Wiggle::solve_naive : &Wiggle::solve);
    ostream &fout = *pfo;
    ul_t fpos_last = pfi->tellg();
    for (unsigned ci = 0; ci < n_cases; ci++)
    {
        Wiggle wiggle(*pfi);
        getline(*pfi, ignore);
        if (tellg)
        {
            ul_t fpos = pfi->tellg();
            cerr << "Case (ci+1)="<<(ci+1) << ", tellg=[" <<
                fpos_last << " " << fpos << ") size=" <<
                (fpos - fpos_last) << "\n";
            fpos_last = fpos;
        }

        (wiggle.*psolve)();
        fout << "Case #"<< ci+1 << ":";
        wiggle.print_solution(fout);
        fout << "\n";
        fout.flush();
    }

    if (pfi != &cin) { delete pfi; }
    if (pfo != &cout) { delete pfo; }
    return 0;
}
