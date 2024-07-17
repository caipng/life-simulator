#include <vector>
#include <iostream>
#include <bitset>

typedef unsigned long long ull;

static inline void add2(ull a, ull b,
		ull &c0, ull &c1) {
	c0 = a ^ b ;
	c1 = a & b ;
}

static inline void add3(ull a, ull b, ull c,
		ull &c0, ull &c1) {
	ull t0, t1, t2 ;
	add2(a, b, t0, t1) ;
	add2(t0, c, c0, t2) ;
	c1 = t2 | t1 ;
}

inline void log(ull m) {
	auto s = std::bitset<64>(m).to_string();
	reverse(s.begin(), s.end());
	for (auto c : s) std::cout << (c == '0' ? '.' : '#');
}

int iom(int nThreads, int nGenerations, std::vector<std::vector<int>>& startWorld, int nRows, int nCols, int nInvasions, std::vector<int> invasionTimes, std::vector<std::vector<std::vector<int>>> invasionPlans);
