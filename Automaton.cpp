#include "Automaton.h"
#include <iostream>
#include <random>
#include <set>
#include <map>
#include <queue>
#include <cassert>
#include <algorithm>

using std::sort;
using std::set;
using std::map;
using std::queue;

Automaton::Automaton(size_t v, const vector<Edge>& edges, size_t start, const vector<size_t>& finish):
    v_(v), next_(v), start_(start), finish_(v, false) {
  for (auto& i : edges) {
    next_[i.from].emplace_back(i.to, i.cost);
  }
  for (auto& i : finish) {
    finish_[i] = true;
  }
}

void Automaton::print() const {
  std::cout << "digraph NFA {" << std::endl;
  for (int i = 0; i < v_; ++i) {
    std::cout << "  " << i << " [shape=";
    if (finish_[i]) {
      std::cout << "double";
    }
    if (start_ == i) {
      std::cout << "octagon";
    } else {
      std::cout << "circle";
    }
    std::cout << "];" << std::endl;
  }
  for (int from = 0; from < v_; ++from) {
    for (auto [to, cost] : next_[from]) {
      std::cout << "  " << from << "->" << to << " [label=\"" << cost << "\"];\n";
    }
  }
  std::cout << "}" << std::endl;
}

void Automaton::dfsForAddAllEmptyEdges(size_t v, size_t start, vector<bool>& visit, vector<bool>& have_edge) {
  visit[v] = true;
  for (auto& i : next_[v]) {
    assert(i.first < visit.size());
    if (!visit[i.first] && i.second == "") {
      dfsForAddAllEmptyEdges(i.first, start, visit, have_edge);
    }
  }
}

void Automaton::addAllEmptyEdges() {
  vector<bool> visit(v_);
  vector<bool> have_edge(v_);
  for (size_t v = 0; v < v_; ++v) {
    for (size_t i = 0; i < v_; ++i) {
      visit[i] = false;
      have_edge[i] = false;
    }
    have_edge[v] = true;
    for (auto& i : next_[v]) {
      if (i.second == "") {
        have_edge[i.first] = true;
      }
    }
    dfsForAddAllEmptyEdges(v, v, visit, have_edge);
    for (size_t i = 0; i < v_; ++i) {
      if (!have_edge[i] && visit[i]) {
        next_[v].emplace_back(i, "");
      }
    }
  }
}

void Automaton::deleteEmptyEdges() {
  addAllEmptyEdges();
  for (size_t from = 0; from < v_; ++from) {
    for (size_t i = 0; i < next_[from].size(); ++i) {
      if (next_[from][i].second != "") {
        continue;
      }
      size_t to = next_[from][i].first;
      if (from != to) {
        if (finish_[to]) {
          finish_[from] = true;
        }
        for (auto& [to2, cost2] : next_[to]) {
          if (cost2 == "") {
            continue;
          }
          next_[from].emplace_back(to2, cost2);
        }
      }
      std::swap(next_[from][i], next_[from].back());
      --i;
      next_[from].pop_back();
    }
  }
  sortEdges();
  // TODO delete check on empty edges
  for (size_t from = 0; from < v_; ++from) {
    for (auto& [to, cost] : next_[from]) {
      if (cost == "") {
        std::cerr << "find empty edge after deleting" << std::endl;
        exit(1);
      }
    }
  }
}

bool Automaton::IsSameRandom(const Automaton &a, const Automaton &b, const size_t num) {
  for (size_t len = 0; len < 5; ++len) {
    for (size_t i = 0; i < num; ++i) {
      std::string q = a.getRandomWord(len);
      if (q != "-" && !b.findWord(q)) {
        std::cout << "b have no " << q << std::endl;
        return false;
      }
      q = b.getRandomWord(len);
      if (q != "-" && !a.findWord(q)) {
        std::cout << "a have no " << q << std::endl;
        return false;
      }
    }
  }
  return true;
}

void Automaton::deleteMoreThanOneLetterEdges() {
  for (size_t from = 0; from < v_; ++from) {
    for (size_t i = 0; i < next_[from].size(); ++i) {
      size_t to = next_[from][i].first;
      std::string cost = next_[from][i].second;
      if (cost.size() <= 1) {
        continue;
      }
      std::string cur = cost;
      next_[from][i].first = v_++;
      next_[from][i].second = cur[0];
      next_.emplace_back();
      for (size_t j = 1; j+1 < cur.size(); ++j) {
        std::string tmp(1, cur[j]); // convert char to string
        next_.back().emplace_back(v_++, tmp);
        next_.emplace_back();
      }
      std::string tmp(1, cur.back());
      next_.back().emplace_back(to, tmp);
    }
  }
  // TODO delete check on good edges
  return;
  for (size_t from = 0; from < v_; ++from) {
    for (auto [to, cost] : next_[from]) {
      if (cost.size() > 1) {
        std::cerr << "find >1 size edge after deleting" << std::endl;
        exit(1);
      }
    }
  }
}

void Automaton::determinize() {
  deleteEmptyEdges();
  deleteMoreThanOneLetterEdges();

  // TODO delete check on single-symbol edges
  for (size_t from = 0; from < v_; ++from) {
    for (auto& [to, cost] : next_[from]) {
      if (cost.size() != 1) {
        std::cerr << "find !=1 size edge in determinize" << std::endl;
        exit(1);
      }
    }
  }

  map<set<size_t>, size_t> vertex;
  vertex[set<size_t>{start_}] = 0;
  queue<set<size_t>> que;
  que.push(set<size_t>{start_});
  vector<vector<pair<size_t, std::string>>> new_next_(1);
  while (!que.empty()) {
    auto vertices = que.front();
    que.pop();
    set<std::string> alph;
    for (auto from : vertices) {
      for (auto& [to, cost] : next_[from]) {
        alph.insert(cost);
      }
    }
    for (auto& ch : alph) {
      set<size_t> toSet;
      for (auto from : vertices) {
        for (auto& [to, cost] : next_[from]) {
          if (cost != ch) {
            continue;
          }
          toSet.insert(to);
        }
      }
      if (vertex.find(toSet) == vertex.end()) {
        que.push(toSet);
        vertex[toSet] = vertex.size();
        new_next_.emplace_back();
      }
      new_next_[vertex[vertices]].emplace_back(vertex[toSet], ch);
    }
  }
  vector<bool> new_finish_(vertex.size(), false);
  for (auto& [v, num] : vertex) {
    for (auto i : v) {
      if (finish_[i]) {
        new_finish_[num] = true;
        break;
      }
    }
  }
  v_ = vertex.size();
  next_ = new_next_;
  start_ = 0;
  finish_ = new_finish_;
  // I hope it works
}

void Automaton::makeComplete() {
  set<std::string> alph;
  for (size_t from = 0; from < v_; ++from) {
    for (auto& [to, cost] : next_[from]) {
      alph.insert(cost);
    }
  }
  next_.emplace_back();
  ++v_;
  for (size_t from = 0; from < v_; ++from) {
    set<std::string> cur_alph(alph);
    for (auto& [to, cost] : next_[from]) {
      cur_alph.erase(cost);
    }
    for (auto& cost : cur_alph) {
      next_[from].emplace_back(v_-1, cost);
    }
  }
  finish_.emplace_back(false);
}

void Automaton::minimize() {
  determinize();
  makeComplete();
  deleteUnreachableVertices();
  sortEdges();

  /// calculate classes
  vector<size_t> color(v_);
  for (size_t i = 0; i < v_; ++i) {
    color[i] = finish_[i];
  }
  bool noChanges = false;
  while (!noChanges) {
    vector<vector<size_t>> new_class(v_);
    for (size_t i = 0; i < v_; ++i) {
      new_class[i].emplace_back(color[i]);
      for (auto& [to, cost] : next_[i]) {
        new_class[i].emplace_back(color[to]);
      }
    }
    map<vector<size_t>, size_t> new_color_v;
    for (size_t i = 0; i < v_; ++i) {
      if (new_color_v.find(new_class[i]) == new_color_v.end()) {
        new_color_v[new_class[i]] = new_color_v.size();
      }
    }
    vector<size_t> new_color(v_);
    for (size_t i = 0; i < v_; ++i) {
      new_color[i] = new_color_v[new_class[i]];
    }
    noChanges = (new_color == color);
    color = new_color;
  }

  /// rebuild graph
  size_t new_v_ = 0;
  for (size_t i = 0; i < v_; ++i) {
    new_v_ = std::max(new_v_, color[i]);
  }
  ++new_v_;
  vector<vector<pair<size_t, std::string>>> new_next_(new_v_);
  vector<bool> new_finish_(new_v_);
  for (size_t i = 0; i < v_; ++i) {
    if (!new_next_[color[i]].empty()) {
      // TODO delete this test on good classes
      for (size_t j = 0; j < next_[i].size(); ++j) {
        assert(new_next_[color[i]][j] == std::make_pair(color[next_[i][j].first], next_[i][j].second));
      }
      continue;
    }
    for (auto& [to, cost] : next_[i]) {
      new_next_[color[i]].emplace_back(color[to], cost);
    }
    new_finish_[color[i]] = finish_[i];
  }
  v_ = new_v_;
  finish_ = new_finish_;
  next_ = new_next_;
  start_ = color[start_];
}

void Automaton::sortEdges() {
  for (auto& v : next_) {
    sort(v.begin(), v.end(), [](const pair<size_t, std::string>& a, const pair<size_t, std::string>& b){
      if (a.second != b.second) {
        return a.second < b.second;
      }
      return a.first < b.first;
    });
    v.resize(std::unique(v.begin(), v.end()) - v.begin());
  }
}

void Automaton::dfsForDeleteUnreachableVertices(size_t v, vector<bool> &visit) const {
  visit[v] = true;
  for (auto& [to, cost] : next_[v]) {
    if (!visit[to]) {
      dfsForDeleteUnreachableVertices(to, visit);
    }
  }
}

void Automaton::deleteUnreachableVertices() {
  vector<bool> visit(v_, false);
  dfsForDeleteUnreachableVertices(start_, visit);
  vector<size_t> new_num(v_);
  size_t new_v_ = 0;
  for (size_t i = 0; i < v_; ++i) {
    if (visit[i]) {
      new_num[i] = new_v_++;
      next_[new_num[i]] = next_[i];
    }
  }
  next_.resize(new_v_);
  for (size_t from = 0; from < new_v_; ++from) {
    for (auto& [to, cost] : next_[from]) {
      assert(visit[to]);
      to = new_num[to];
    }
  }
  for (size_t i = 0; i < v_; ++i) {
    if (visit[i]) {
      finish_[new_num[i]] = finish_[i];
    }
  }
  finish_.resize(new_v_);
  v_ = new_v_;
}

std::string Automaton::getRandomWord(size_t min_size) const {
  return dfsRandom(start_, min_size);
}

std::string Automaton::dfsRandom(size_t v, int min_size) const {
  if (min_size <= 0 && finish_[v]) {
    return "";
  }
  if (min_size < -1) {
    return "-";
  }
  vector<size_t> order(next_[v].size());
  for (size_t i = 0; i < order.size(); ++i) {
    order[i] = i;
  }
  std::shuffle(order.begin(), order.end(), std::mt19937(std::random_device()()));
  for (auto i : order) {
    std::string s = dfsRandom(next_[v][i].first, min_size - (int)next_[v][i].second.size());
    if (s != "-") {
      return next_[v][i].second + s;
    }
  }
  return "-";
}

bool Automaton::dfsFind(size_t v, size_t l, const std::string& word, vector<vector<bool>>& visit) const {
  visit[v][l] = true;
  if (l == word.size() && finish_[v]) {
    return true;
  }
  if (l > word.size()) {
    return false;
  }
  for (auto& [to, cost] : next_[v]) {
    size_t same = 0;
    for (size_t i = l; i < word.size() && i-l < cost.size(); ++i) {
      if (word[i] != cost[i-l]) {
        break;
      }
      ++same;
    }
    if (same != cost.size() || visit[to][l+cost.size()]) {
      continue;
    }
    if (dfsFind(to, l+cost.size(), word, visit)) {
      return true;
    }
  }
  return false;
}

bool Automaton::findWord(const std::string& word) const {
  vector<vector<bool>> visit(v_, vector<bool>(word.size()+1, false));
  return dfsFind(start_, 0, word, visit);
}
void Automaton::reverse() {
  for (size_t i = 0; i < finish_.size(); ++i) {
    finish_[i] = !finish_[i];
  }
}
