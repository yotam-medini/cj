#include <iostream>
#include <vector>
#include <string>
#include <cstdlib>
#include <fmt/core.h>

using u_t = unsigned;
using ul_t = unsigned long;
using ull_t = unsigned long long;
using e_t = ull_t;

class Node {
 public:
  Node(e_t sum=0, e_t max=0, size_t max_count=0) :
    sum_{sum}, max_{max}, max_count_{max_count} {
  }
  std::string str() const {
    return fmt::format("{}sum={}, max={}, #={}{}",
       "{", sum_, max_, max_count_, "}");
  }
  std::string StrShort() const {
    return fmt::format("{}S={}, M={}, #={}{}",
       "{", sum_, max_, max_count_, "}");
  }
  e_t sum_{0};
  e_t max_{0};
  size_t max_count_{0};
};

class SegmentTree {
 public:
  using ve_t = std::vector<e_t>;
  SegmentTree(const ve_t& _a);
  void update(u_t pos, e_t val);
  e_t query_sum(u_t b, u_t e) const;
  void print(std::ostream& os) const;
 private:
  using level_nodes_t = std::vector<Node>;
  e_t level_sum(u_t level, u_t b, u_t e) const;
  void level_update(u_t level, u_t pos, e_t old_val, e_t val);
  u_t height() const { return levels_nodes_.size(); }
  u_t size() const {
    return levels_nodes_.empty() ? 0 : levels_nodes_[0].size();
  }
  std::vector<level_nodes_t> levels_nodes_;
};

SegmentTree::SegmentTree(const ve_t& a) {
  levels_nodes_.push_back(level_nodes_t());
  auto &nodes = levels_nodes_.back();
  nodes.reserve(a.size());
  for (const auto x: a) {
    nodes.push_back({x, x, 1});
  }
  while (levels_nodes_.back().size() > 1) {
    const level_nodes_t& prev = levels_nodes_.back();
    const u_t sz = prev.size() / 2;
    const u_t sz2 = 2*sz;
    level_nodes_t b; b.reserve(sz);
    for (u_t i = 0; i < sz2; i += 2) {
        b.push_back(Node{prev[i].sum_ + prev[i + 1].sum_}); // JUST sum.....
    }
    levels_nodes_.push_back(level_nodes_t());
    swap(levels_nodes_.back(), b);
  }
  print(std::cerr);
}

void SegmentTree::update(u_t pos, e_t val) {
  level_update(0, pos, levels_nodes_[0][pos].sum_, val);
}

auto SegmentTree::query_sum(u_t b, u_t e) const -> e_t {
  return level_sum(0, b, e);
}

auto SegmentTree::level_sum(u_t level, u_t b, u_t e) const -> e_t {
  e_t total = 0;
  if (b < e) {
    if (b % 2 != 0) {
      total += levels_nodes_[level][b++].sum_;
    }
    if (e % 2 != 0) {
      total += levels_nodes_[level][--e].sum_;
    }
    total += level_sum(level + 1, b/2, e/2);
  }
  return total;
}

void SegmentTree::level_update(u_t level, u_t pos, e_t old_val, e_t val) {
  level_nodes_t& alevel = levels_nodes_[level];
  alevel[pos].sum_ -= old_val;
  alevel[pos].sum_ -= val;
  if ((pos % 2 != 0) || (pos + 1 < alevel.size())) {
    level_update(level + 1, pos / 2, old_val, val);
  }
}

void SegmentTree::print(std::ostream& os) const {
  for (u_t level = 0; level < levels_nodes_.size(); ++level) {
    const level_nodes_t& alevel = levels_nodes_[level];
    os << fmt::format("[{}]", level);
    for (const Node& node: alevel) {
      os << fmt::format(" {}", node.StrShort());
    }
    os << '\n';
  }
}

// ================================ Test ================================
#include <iostream>
#include <algorithm>
#include <numeric>

using vull_t = std::vector<ull_t>;

static int
verdict(const vull_t& a, u_t b, u_t e, ull_t st_sum, ull_t naive_sum) {
  int rc = 0;
  if (st_sum != naive_sum) {
    rc = 1;
    std::cerr << "Failure: specific " << b << ' ' << e;
    for (ull_t x: a) { std::cerr << ' ' << x; }
    std::cerr << '\n';
  }
  return rc;
}

static int test_case(const vull_t& a, u_t b, u_t e) {
  ull_t naive_sum = accumulate(a.begin() + b, a.begin() + e, 0ull);
  SegmentTree segtree(a);
  ull_t st_sum = segtree.query_sum(b, e);
  return verdict(a, b, e, st_sum, naive_sum);
}

static int test_cases(const vull_t& a) {
  int rc = 0;
  SegmentTree segtree(a);
  for (u_t b = 0; (rc == 0) && (b < a.size()); ++b) {
    for (u_t e = b; (rc == 0) && (e <= a.size()); ++e) {
      ull_t naive_sum = accumulate(a.begin() + b, a.begin() + e, 0ull);
      ull_t st_sum = segtree.query_sum(b, e);
      rc = verdict(a, b, e, st_sum, naive_sum);
    }
  }
  return rc;
}

static u_t rand_range(u_t nmin, u_t nmax) {
  u_t r = nmin + rand() % (nmax + 1 - nmin);
  return r;
}

static int random_test(int argc, char **argv) {
  int rc = 0;
  int ai = 0;
  u_t n_tests = strtoul(argv[ai++], 0, 0);
  u_t szmin = strtoul(argv[ai++], 0, 0);
  u_t szmax = strtoul(argv[ai++], 0, 0);
  u_t nmax = strtoul(argv[ai++], 0, 0);
  std::cerr << "n_tests="<<n_tests <<
      ", szmin="<<szmin << ", szmax="<<szmax << ", nmax="<<nmax << '\n';
  for (u_t ti = 0; (rc == 0) && (ti < n_tests); ++ti) {
    std::cerr << "Tested: " << ti << '/' << n_tests << '\n';
    u_t sz = rand_range(szmin, szmax);
    vull_t a;  a.reserve(sz);
    while (a.size() < sz) {
      a.push_back(rand_range(0, nmax));
    }
    rc = test_cases(a);
  }
  return rc;
}

int main(int argc, char **argv) {
  int rc = 0;
  if (std::string(argv[1]) == std::string("specific")) {
    int ai = 2;
    u_t b = strtoul(argv[ai++], 0, 0);
    u_t e = strtoul(argv[ai++], 0, 0);
    vull_t a;
    for (; ai < argc; ++ai) {
      a.push_back(strtoul(argv[ai++], 0, 0));
    }
    rc = test_case(a, b, e);
  }
  else {
    rc = random_test(argc - 1, argv + 1);
  }
  return rc;
}
