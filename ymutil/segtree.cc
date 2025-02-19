#include <iostream>
#include <numeric>
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
  void MaxCombine(const Node &node0, const Node &node1) {
    if (node0.max_ == node1.max_) {
      max_ = node0.max_;
      max_count_ = node0.max_count_ + node1.max_count_;
    } else {
      const Node* from = (node0.max_ < node1.max_ ? &node1 : &node0);
      max_ = from->max_;
      max_count_ = from->max_count_;
    }
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
  e_t query_max(u_t b, u_t e, size_t& count) const;
  e_t query_max(u_t b, u_t e) const {
    size_t __count; // unused
    return query_max(b, e, __count);
  }
  void print(std::ostream& os) const;
 private:
  using level_nodes_t = std::vector<Node>;
  void level_update(u_t level, u_t pos, e_t old_val, e_t val);
  e_t level_sum(u_t level, u_t b, u_t e) const;
  e_t level_max(u_t level, u_t b, u_t e, size_t& count) const;
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
        b.back().MaxCombine(prev[i], prev[i + 1]);
    }
    levels_nodes_.push_back(level_nodes_t());
    swap(levels_nodes_.back(), b);
  }
  print(std::cerr);
}

void SegmentTree::update(u_t pos, e_t val) {
  level_update(0, pos, levels_nodes_[0][pos].sum_, val);
}

void SegmentTree::level_update(u_t level, u_t pos, e_t old_val, e_t val) {
  level_nodes_t &alevel = levels_nodes_[level];
  Node &node = alevel[pos];
  node.sum_ -= old_val;
  node.sum_ -= val;
  if ((level == 0) || (node.max_ < val)) {
    node.max_ = val;
    node.max_count_ = 1;
  } else if (node.max_ == val) {
    if (old_val < val) {
      node.max_count_ += 1;
    }
  } else { // node.max_ > val
    if (node.max_ == old_val) {
      if (node.max_count_ > 1) {
        --node.max_count_;
      } else {
        const level_nodes_t &lower_nodes = levels_nodes_[level - 1];
        node.MaxCombine(lower_nodes[2*pos], lower_nodes[2*pos + 1]);
      }
    }
  }
  if ((pos % 2 != 0) || (pos + 1 < alevel.size())) {
    level_update(level + 1, pos / 2, old_val, val);
  }
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

static void MaxBy(e_t &v, size_t &vcount, e_t x, size_t xcount) {
  if (v == x) {
    vcount += xcount;
  } else if (v < x) {
    v = x;
    vcount = xcount;
  }
}

auto SegmentTree::query_max(u_t b, u_t e, size_t &count) const -> e_t {
  e_t ret = std::numeric_limits<e_t>::min();
  count = 0;
  if (b < e) {
    ret = level_max(0, b, e, count);
  }
  return ret;
}

auto
SegmentTree::level_max(u_t level, u_t b, u_t e, size_t &count) const -> e_t {
  count = 0;
  e_t ret = std::numeric_limits<e_t>::min();
  if (b < e) {
    if (b + 1 == e) {
      const Node& node = levels_nodes_[level][b];
      ret = node.max_;
      count = node.max_count_;
    } else {
      if (b % 2 != 0) {
        const Node& node = levels_nodes_[level][b++];
        MaxBy(ret, count, node.max_, node.max_count_);
      }
      if (e % 2 != 0) {
        const Node& node = levels_nodes_[level][--e];
        MaxBy(ret, count, node.max_, node.max_count_);
      }
      size_t count_sub = 0;
      e_t max_sub = level_max(level + 1, b/2, e/2, count_sub);
      MaxBy(ret, count, max_sub, count_sub);
    }
  }
  return ret;
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
verdict_sum(const vull_t& a, u_t b, u_t e, ull_t st_sum, ull_t naive_sum) {
  int rc = 0;
  if (st_sum != naive_sum) {
    rc = 1;
    std::cerr << "Sum-Failure: specific " << b << ' ' << e;
    for (ull_t x: a) { std::cerr << ' ' << x; }
    std::cerr << '\n';
  }
  return rc;
}

static int
verdict_max(
    const vull_t& a,
    u_t b,
    u_t e,
    ull_t st_max,
    ull_t naive_max,
    size_t st_max_count,
    size_t naive_max_count) {
  int rc = 0;
  if ((st_max != naive_max) || (st_max_count != naive_max_count)) {
    rc = 1;
    std::cerr << "Max-Failure: specific " << b << ' ' << e;
    for (ull_t x: a) { std::cerr << ' ' << x; }
    std::cerr << '\n';
  }
  return rc;
}

e_t NaiveMax(const vull_t& a, u_t b, u_t e, size_t &count) {
  e_t ret = 0;
  count = 0;
  for (u_t i = b; i < e; ++i) {
    if (count == 0) {
      ret = a[i];
      count = 1;
    } else if (ret <= a[i]) {
      count = (ret == a[i] ? count : 0) + 1;
      ret = a[i];
    }
  }
  return ret;
}

static int test_case(const vull_t& a, u_t b, u_t e) {
  ull_t naive_sum = accumulate(a.begin() + b, a.begin() + e, 0ull);
  SegmentTree segtree(a);
  ull_t st_sum = segtree.query_sum(b, e);
  int rc = verdict_sum(a, b, e, st_sum, naive_sum);
  if (rc == 0) {
    size_t naive_max_count, st_max_count;
    ull_t naive_max = NaiveMax(a, b, e, naive_max_count);
    e_t st_max = segtree.query_max(b, e, st_max_count);
    rc = verdict_max(a, b, e, st_max, naive_max, st_max_count, naive_max_count);
  }
  return rc;
}

static int test_cases(const vull_t& a) {
  int rc = 0;
  SegmentTree segtree(a);
  for (u_t b = 0; (rc == 0) && (b < a.size()); ++b) {
    for (u_t e = b; (rc == 0) && (e <= a.size()); ++e) {
      ull_t naive_sum = accumulate(a.begin() + b, a.begin() + e, 0ull);
      ull_t st_sum = segtree.query_sum(b, e);
      rc = verdict_sum(a, b, e, st_sum, naive_sum);
      if (rc == 0) {
        size_t naive_max_count, st_max_count;
        ull_t naive_max = NaiveMax(a, b, e, naive_max_count);
        e_t st_max = segtree.query_max(b, e, st_max_count);
        rc = verdict_max(
          a, b, e, st_max, naive_max, st_max_count, naive_max_count);
      }
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
      a.push_back(strtoul(argv[ai], 0, 0));
    }
    rc = test_case(a, b, e);
  }
  else {
    rc = random_test(argc - 1, argv + 1);
  }
  return rc;
}
