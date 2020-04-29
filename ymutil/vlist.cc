#include <vector>
#include <list>

class VList
{
 public:
     typedef unsigned u_t;
     typedef std::list<u_t> lu_t;
     typedef lu_t::const_iterator citer_t;
     VList() : sz(0) {}
     u_t size() const { return sz; }
     bool empty() const { return sz == 0; }
     u_t capacity() const { return v.size(); }
     bool has(u_t n) const
     {
         return (n < capacity()) && (v[n] != l.end());
     }
     void add(u_t n)
     {
         if (n >= capacity())
         {
             v.insert(v.end(), size_t(n + 1 - capacity()), l.end());
         }
         v[n] = l.insert(l.end(), n);
         ++sz;
     }
     void safe_add(u_t n)
     {
         if (!has(n)) { add(n); }
     }
     void del(u_t n)
     {
         l.erase(v[n]);
         v[n] = l.end();
         --sz;
     }
     void safe_del(u_t n)
     {
         if (has(n)) { del(n); }
     }
     citer_t begin() const { return l.begin(); }
     citer_t end() const { return l.end(); }
 private:
     typedef lu_t::iterator iter_t;
     typedef std::vector<iter_t> viter_t;
     viter_t v;
     lu_t l;
     u_t sz;
};

#include <iostream>
using namespace std;
typedef unsigned u_t;

static void show(const VList& vl)
{
    cout << "V:";
    u_t vsz = 0, lsz = 0;
    for (u_t n = 0; n < vl.capacity(); ++n)
    {
        if (vl.has(n))
        {
            cout << ' ' << n;
            ++vsz;
        }
    }
    cout << '\n';
    cout << "L:";
    for (VList::citer_t i = vl.begin(), e = vl.end(); i != e; ++i)
    {
        u_t n = *i;
        cout << ' ' << n;
        ++lsz;
    }
    cout << '\n';
    cout << "vsz="<<vsz << ", lsz="<<lsz << ", vl.size="<<vl.size() << '\n';
}

int main(int, char **)
{
    const u_t N = 100, p1 = 31, p2 = 37;
    VList vl;
    for (u_t i = 0; i < N; ++i)
    {
        u_t n = (i*p1) % N;
        vl.add(n);
    }
    // show(vl);
    for (u_t i = 0; i < N; ++i)
    {
        u_t n = (i*p2) % N;
        if (n % 8 != 0)
        {
            vl.del(n);
        }
    }
    show(vl);
    return 0;
}
