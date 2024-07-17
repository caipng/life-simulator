#include "iom.h"

int w, h, wordwidth;
long long wh;
size_t padding_right;
ull mask_right = -1;

inline void print(ull * ptr) {
	for (int i = 2; i + 1 < h; ++i) {
		for (int j = 0; j < wordwidth; ++j) {
			log(ptr[j * h + i]);
			std::cout << "|";
		}
		std::cout << std::endl;
	}
}

struct faction {
	ull *u0, *u1, *re, *en;	
	void init();
	void setcell(int, int);
	void go();
} factions[9];

int iom(int nThreads, int nGenerations, std::vector<std::vector<int>>& startWorld, int nRows, int nCols,
		int nInvasions, std::vector<int> invasionTimes, std::vector<std::vector<std::vector<int>>> invasionPlans) {

	w = nCols;
	h = nRows + 3;
	wordwidth = (w + 63) >> 6;
	wh = wordwidth * h;
	padding_right = (64 - (w & 63)) % 64;
	if (padding_right) mask_right = (1 << (64 - padding_right)) - 1;

	for (int i = 0; i < 9; ++i) {
		factions[i].init();	
	}

	for (int i = 0; i < nRows; ++i) {
		for (int j = 0; j < nCols; ++j) if (startWorld[i][j]) {
			factions[startWorld[i][j] - 1].setcell(j, i);
		}
	}

	int ans = 0;
	for (int iter = 1; iter <= nGenerations; ++iter) {
		for (int i = 0; i < 9; ++i) {
			factions[i].go();	
			std::cout << "i:\n"; print(factions[i].u0); std::cout << std::endl;
		}
		// reprod tiebreak
		// killing + count
	}

	return ans;
}

void faction::init() {
	u0 = (ull *)calloc(wordwidth*sizeof(ull), h+1);
	u1 = (ull *)calloc(wordwidth*sizeof(ull), h+1);
	re = (ull *)calloc(wordwidth*sizeof(ull), h+1);
	en = (ull *)calloc(wordwidth*sizeof(ull), h+1);
}

void faction::setcell(int x, int y) {
	u0[(x>>6)*h+y+2] |= 1LL << (x & 63);
}

void faction::go() {
	for (int i = 0; i < wordwidth; ++i) {
		ull *col = u0 + i * h;
		col[1] = col[h - 2];
		col[h - 1] = col[2];
	}

	for (int i = 0; i < wordwidth; ++i) {
		ull w300 = 0;
		ull w301 = 0;
		ull prevprev = 0, prev = 0;
		ull *col = u0 + i * h;

		ull *pcol = u0 + (i-1) * h;
		if (i == 0) pcol = u0 + (wordwidth - 1) * h;

		ull *ncol = u0 + (i+1) * h;
		if (i+1 == wordwidth) ncol = u0;

		ull *wcol = u1 + i * h;
		ull *rcol = re + i * h;
		ull *ecol = en + i * h;
		ull w1 = *col;

		if (i + 1 == wordwidth && padding_right) {
			auto fi = (*ncol) & 1;
			if (fi) w1 |= 1 << (64 - padding_right);
		}

		ull w1l = w1 << 1;
		ull w1r = w1 >> 1;

		if (i > 0)
			w1l += *pcol >> (wordwidth-1);
		if (i == 0) {
			auto x = *pcol << padding_right;
			w1l += x >> (wordwidth-1);
		}

		if (i+1 < wordwidth)
			w1r += *ncol << (wordwidth-1);
		if (i + 1 == wordwidth) {
			if (padding_right);
			else {
				w1r += *ncol << (wordwidth-1);
			}
		}

		prev = w1l | w1r | w1;
		ull w210, w211, w310, w311;
		add2(w1l, w1r, w210, w211);
		add2(w1, w210, w310, w311);
		w311 |= w211;
#pragma unroll 8
		for (int j = 0; j + 1 < h; j++, col++, pcol++, ncol++, wcol++, rcol++, ecol++) {
			ull w2 = col[1] ;
			if (i + 1 == wordwidth && padding_right) {
				auto fi = ncol[1] & 1;
				if (fi) w2 |= 1 << (64 - padding_right);
			}

			ull w2l = w2 << 1;
			ull w2r = w2 >> 1;


			if (i > 0)
				w2l |= pcol[1] >> 63;
			if (i == 0) {
				auto x = pcol[1] << padding_right;
				w2l |= x >> 63;
			}

			if (i+1 < wordwidth)
				w2r |= ncol[1] << 63;
			if (i+1 == wordwidth) {
				if (padding_right);
				else {
					w2r |= ncol[1] << 63;
				}
			}

			ull curr = w2 | w2l | w2r;
			ull w220, w221, w320, w321, a0, a1;
			add2(w2l, w2r, w220, w221);
			add2(w2, w220, w320, w321);
			w321 |= w221;
			add3(w300, w210, w320, a0, a1);
			ull ng1 = (a1 ^ w301 ^ w211 ^ w321) & ((a1 | w301) ^ (w211 | w321)) & (a0 | w1);

			if (i + 1 == wordwidth && padding_right) {
				ng1 &= mask_right;
			}

			if (j > 1) {
				wcol[0] = ng1 ;
				rcol[0] = ng1 & ~w1;
				ecol[0] = (prevprev | prev | curr);
				if (i + 1 == wordwidth && padding_right) ecol[0] &= mask_right;
			}	

			w300 = w310;
			w301 = w311;
			w310 = w320;
			w311 = w321;
			w210 = w220;
			w211 = w221;
			w1 = w2;
			prevprev = prev;
			prev = curr;
		}
	}
	std::swap(u0, u1);
}

