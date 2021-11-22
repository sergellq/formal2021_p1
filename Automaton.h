#pragma once
#include <vector>
#include "Edge.h"

using std::vector;
using std::pair;

class Automaton {
 public:
  explicit Automaton(size_t v, const vector<Edge>& edges, size_t start, const vector<size_t>& finish);

  Automaton(const Automaton&) = default;

  void print() const;
  void addAllEmptyEdges();
  void deleteEmptyEdges();
  void deleteMoreThanOneLetterEdges();
  void determinize();
  void minimize();
  void makeComplete();
  void sortEdges();
  void deleteUnreachableVertices();
  void reverse();

  [[nodiscard]] std::string getRandomWord(size_t min_size) const;
  [[nodiscard]] bool findWord(const std::string& word) const;
  static bool IsSameRandom(const Automaton& a, const Automaton& b, size_t num = 100); // it is long

 public:
  void dfsForAddAllEmptyEdges(size_t v, size_t start, vector<bool>& visit, vector<bool>& have_edge);
  void dfsForDeleteUnreachableVertices(size_t v, vector<bool>& visit) const;
  std::string dfsRandom(size_t v, int min_size) const;
  bool dfsFind(size_t v, size_t l, const std::string& word, vector<vector<bool>>& visit) const;

  size_t v_;
  vector<vector<pair<size_t, std::string>>> next_;
  size_t start_;
  vector<bool> finish_;
};
