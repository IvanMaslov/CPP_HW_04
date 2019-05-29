#include <iostream>
#include <cassert>
#include <cstring>

#include "HaffmanCompressor.h"
#include "Test.h"

void short_code(pair<void*, size_t> a, bool out = true) {
	HaffmanCompressor c;
	pair<void*, size_t> rtt = c.compress(a);
	pair<void*, size_t> r = c.decompress(rtt);
	assert(a.second == r.second);
	assert(memcmp(r.first, a.first, a.second) == 0);
	std::cerr << "DECOMPRESSED: in=" << a.second << " out=" << rtt.second << " zip=" << (1.0 * rtt.second / a.second) << std::endl;
	if (out) {
		char* R = static_cast<char*>(r.first);
		for (size_t i = 0; i < r.second; ++i)
			std::cerr << static_cast<int>(R[i]) << ' ';
		std::cerr << std::endl;
	}
	std::cerr << std::endl;
	delete[] r.first;
}


void combo_test(pair<void*, size_t> a, size_t chunk, bool out = true) {
	assert(((a.second % chunk) == 0));
	HaffmanCompressor c;
	for (size_t i = 0; i < a.second; i += chunk) {
		c.add_chunk(make_pair(((char*)a.first) + i, chunk));
	}
	pair<void*, size_t> rtt = c.prepare();
	for (size_t i = 0; i < a.second; i += chunk) {
		pair<void*, size_t> ads = c.compress_chunk(make_pair(((char*)a.first) + i, chunk));
		char* new_rtt = new char[ads.second + rtt.second];
		memcpy(new_rtt, rtt.first, rtt.second);
		memcpy(new_rtt + rtt.second, ads.first, ads.second);
		delete[] ads.first;
		delete[] rtt.first;
		rtt.first = new_rtt;
		rtt.second += ads.second;
	}
	pair<void*, size_t> r;
	r.second = 0;
	r.first = nullptr;
	c.try_decompress_codes(make_pair(rtt.first, c.Wall));
	for (size_t i = c.Wall; i < rtt.second; ) {
		size_t u = ((size_t*)(((char*)rtt.first) + i))[0];
		u = (u >> 3) + ((u & 7) > 0);
		pair<void*, size_t> ads = c.decompress_chunk(make_pair((((char*)rtt.first) + i), sizeof(size_t) + u));
		i += sizeof(size_t) + u;
		char* new_r = new char[ads.second + r.second];
		if (r.first != nullptr) memcpy(new_r, r.first, r.second);
		memcpy(new_r + r.second, ads.first, ads.second);
		delete[] ads.first;
		if(r.first != nullptr) delete[] r.first;
		r.first = new_r;
		r.second += ads.second;
		
	}

	assert(a.second == r.second);
	assert(memcmp(r.first, a.first, a.second) == 0);
	std::cerr << "DECOMPRESSED: in=" << a.second << " out=" << rtt.second << " zip=" << (1.0 * rtt.second / a.second) << std::endl;
	if (out) {
		char* R = static_cast<char*>(r.first);
		for (size_t i = 0; i < r.second; ++i)
			std::cerr << static_cast<int>(R[i]) << ' ';
		std::cerr << std::endl;
	}
	std::cerr << std::endl;
	delete[] r.first;
}
void simple1(bool out = true) {
	int* A = new int[10];
	for (int i = 0; i < 10; ++i)
		A[i] = i;
	short_code(make_pair(A, 40), out);
	delete[] A;
}


void simple2(size_t val = 256, bool out = true) {
	char* A = new char[val];
	for (int i = 0; i < val; ++i)
		A[i] = i;
	short_code(make_pair(A, val), out);
	delete[] A;
	if (!out)std::cerr << "PASSED: s2(val=" << val << ")\n";
}


void simple3(size_t val = 256, char d = 147, bool out = true) {
	char* A = new char[val];
	for (int i = 0; i < val; ++i)
		A[i] = (i + d) & 255;
	short_code(make_pair(A, val), out);
	delete[] A;
	if (!out)std::cerr << "PASSED: s3(val=" << val << ", d=" << (int)d << ")\n";
}

void simple4(size_t val = 256, int d = 147, bool out = true) {
	int* A = new int[val];
	for (int i = 0; i < val; ++i)
		A[i] = i + d;
	short_code(make_pair(A, val * sizeof(int)), out);
	delete[] A;
	if (!out)std::cerr << "PASSED: s4(val=" << val << ", d=" << (int)d << ")\n";
}

void combo_simple1(size_t size = 1000, size_t d = 100, bool out = true) {
	char* A = new char[size];
	for (int i = 0; i < size; ++i)
		A[i] = i & 255;
	combo_test(make_pair(A, size), d, out);
	delete[] A;
	if (!out)std::cerr << "PASSED: c1(val=" << size << ", d=" << d << ")\n";
}

void do_my_tests() {
	combo_simple1();
	combo_simple1(2500, 50, true);
	combo_simple1(2500, 2500, true);
	combo_simple1(10000, 100, false);
	combo_simple1(1000000, 1000, false);

	simple1();

	simple2(127);
	simple2(128);
	simple2(129);
	simple2(135);
	simple2(229);
	simple2(254);
	simple3(254, 100);
	simple3(254, 127);
	simple3(254, -128);
	simple3(254);
	simple2(255);
	simple2(256);

	simple3(254, 634);
	simple3(2538644, 634, false);
	simple3(2538322, 132, false);
	simple3(1543032, 1, false);

	simple4();
	simple4(419822, 73423, false);
	simple4(412422, 41297687, false);
	simple4(1000000, 41297687, false);
	simple4(10000000, 41297687, false);
}

void stable_test(size_t c = 1000) {
	for (size_t i = 0; i < c; ++i)
		do_my_tests();
}

int main() {
	do_my_tests();
	//stable_test();
}