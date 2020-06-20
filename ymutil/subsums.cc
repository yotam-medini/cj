#include <set>
#include <utility>
#include <vector>

using namespace std;
typedef unsigned u_t;
typedef vector<u_t> vu_t;
typedef vector<vu_t> vvu_t;
typedef pair<u_t, vu_t> u_vu_t;
typedef vector<u_vu_t> v_u_vu_t;
typedef set<u_vu_t> set_u_vu_t;

// assuming n < 2^inc.size()
void subsums(v_u_vu_t& sum_idx, const vu_t& inc, u_t n)
{
    
    sum_idx.clear();
    sum_idx.reserve(n);
    set_u_vu_t heap;
    heap.insert(heap.end(), u_vu_t(inc[0], vu_t(size_t(1), 0)));
    while (sum_idx.size() < n)
    {
        sum_idx.push_back(*heap.begin());
        const u_vu_t& curr = sum_idx.back();
        heap.erase(heap.begin());
        const u_t curr_sum = curr.first;
        const vu_t curr_idx = curr.second;
        if (curr_idx[0] != 0)
        {
            vu_t idx;
            idx.push_back(0);
            idx.insert(idx.end(), curr_idx.begin(), curr_idx.end());
            heap.insert(heap.end(), u_vu_t(curr_sum + inc[0], idx));
        }
        for (u_t i = 0; i < curr_idx.size(); ++i)
        {
            u_t ii = curr_idx[i];
            if ((ii + 1 < inc.size()) &&
                ((i + 1 == curr_idx.size()) || (ii + 1 < curr_idx[i + 1])))
            {
                vu_t idx(curr_idx);
                idx[i] = ii + 1;
                u_t ss = (curr_sum - inc[ii]) + inc[ii + 1];
                heap.insert(heap.end(), u_vu_t(ss, idx));
            }
        }
    }
}

#include <algorithm>
#include <iostream>
#include <sstream>
#include <string>

string vu_to_str(const vu_t& a)
{
    ostringstream os;
    os << '{';
    const char* sep = "";
    for (u_t x: a)
    {
        os << sep << x; sep = ", ";
    }
    os << '}';
    string ret = os.str();
    return ret;
}

int main(int argc, char** argv)
{
    u_t n = stoi(argv[1]);
    vu_t inc;
    for (int ai = 2; ai < argc; ++ai)
    {
        inc.push_back(stoi(argv[ai]));
    }
    sort(inc.begin(), inc.end());
    v_u_vu_t sum_idx;
    subsums(sum_idx, inc, n);
    for (const u_vu_t& sidx: sum_idx)
    {
        cout << sidx.first << " = SumIdx(" << vu_to_str(sidx.second) << ")\n";
    }
    return 0;
}
