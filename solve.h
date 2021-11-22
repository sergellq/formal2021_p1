#include <iostream>
#include <vector>
#include "Automaton.h"
#include <cassert>
#include <queue>

struct graph {
  vector<Edge> edges;
  int start;
  int finish;
};

std::string solve(std::string s, std::string c, size_t len) {
  int node = -1;
  vector<graph> stack;
  for (auto c : s) {
    if (c == ' ') continue;
    if (c == '+') {
      auto& g = stack.back();
      auto& h = stack[stack.size() - 2];
      h.edges.emplace_back(node + 1, g.start, "");
      h.edges.emplace_back(node + 1, h.start, "");
      h.edges.emplace_back(g.finish, node + 2, "");
      h.edges.emplace_back(h.finish, node + 2, "");
      for (auto& i: g.edges) h.edges.emplace_back(std::move(i));
      h.start = node + 1;
      h.finish = node + 2;
      node += 2;
      stack.pop_back();
    } else if (c == '.') {
      auto& g = stack.back();
      auto& h = stack[stack.size() - 2];
      h.edges.emplace_back(h.finish, g.start, "");
      h.finish = g.finish;
      for (auto& i: g.edges) h.edges.emplace_back(std::move(i));
      stack.pop_back();
    } else if (c == '*') {
      auto& g = stack.back();
      g.edges.emplace_back(g.finish, g.start, "");
      g.edges.emplace_back(node + 1, g.start, "");
      g.edges.emplace_back(node + 1, g.finish, "");
      g.start = node + 1;
      node += 1;
    } else {
      if (c == '1') {
        stack.emplace_back(graph{vector<Edge>(1, Edge(node + 1, node + 2, "")), node + 1, node + 2});
      } else if (c == '0') {
        stack.emplace_back(graph{vector<Edge>(), node + 1, node + 2});
      } else {
        stack.emplace_back(graph{vector<Edge>(1, Edge(node + 1, node + 2, c)), node + 1, node + 2});
      }
      node += 2;
    }
  }
  node += 1;
  assert(stack.size() == 1);

  Automaton a(node, stack.back().edges, stack.back().start, vector<size_t>(1, stack.back().finish));
  a.minimize();

  node = a.v_;
  vector<vector<pair<size_t, std::string>>> pr(node);
  for (size_t v = 0; v < node; ++v) {
    for (auto& [to, cost] : a.next_[v]) {
      assert(cost.size() == 1);
      pr[to].emplace_back(v, cost);
    }
  }
  vector<vector<bool>> vis(node, vector<bool>(len + 1, false));
  {
    std::queue<pair<size_t, size_t>> que;
    for (size_t i = 0; i < node; ++i) {
      if (a.finish_[i]) {
        que.push({i, 0});
        vis[i][0] = true;
      }
    }
    while (!que.empty()) {
      auto[v, l] = que.front();
      que.pop();
      if (l == len) {
        continue;
      }
      for (auto& [to, cost] : pr[v]) {
        if (cost != c) continue;
        if (vis[to][l + 1]) continue;
        vis[to][l + 1] = true;
        que.push({to, l + 1});
      }
    }
  }
  size_t dist = 2e9;
  {
    vector<size_t> f(node, 2e9);
    std::queue<size_t> que;
    f[a.start_] = 0;
    que.push(a.start_);
    while (!que.empty()) {
      auto v = que.front();
      if (vis[v][len]) {
        dist = f[v];
        break;
      }
      que.pop();
      for (auto& [to, cost] : a.next_[v]) {
        if (f[to] > f[v]+1) {
          f[to] = f[v] + 1;
          que.push(to);
        }
      }
    }
  }
  if (dist > node) {
    return "INF";
  }
  return std::to_string(dist + len);
}
