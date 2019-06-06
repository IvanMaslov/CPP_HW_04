#include <iostream>
#include <cassert>
#include <cstring>
#include <random>


#ifdef _DEBUG
#include <crtdbg.h>
#define _CRTDBG_MAP_ALLOC
#endif

#include "HaffmanCompressor.h"
namespace old_test {
	void short_code(pair<char*, size_t> a, bool out = true) {
		HaffmanCompressor c;
		pair<char*, size_t> rtt = c.compress(a);
		pair<char*, size_t> r = c.decompress(rtt);
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
		delete[] rtt.first;
	}


	void combo_test(pair<char*, size_t> a, size_t chunk, bool out = true) {
		assert(((a.second % chunk) == 0));
		HaffmanCompressor c;
		for (size_t i = 0; i < a.second; i += chunk) {
			c.add_chunk(make_pair(((char*)a.first) + i, chunk));
		}
		pair<char*, size_t> rtt = c.prepare();
		for (size_t i = 0; i < a.second; i += chunk) {
			pair<char*, size_t> ads = c.compress_chunk(make_pair(((char*)a.first) + i, chunk));
			char* new_rtt = new char[ads.second + rtt.second];
			memcpy(new_rtt, rtt.first, rtt.second);
			memcpy(new_rtt + rtt.second, ads.first, ads.second);
			delete[] ads.first;
			delete[] rtt.first;
			rtt.first = new_rtt;
			rtt.second += ads.second;
		}
		pair<char*, size_t> r;
		r.second = 0;
		r.first = nullptr;
		c.try_decompress_codes(make_pair(rtt.first, c.Wall));
		for (size_t i = c.Wall; i < rtt.second; ) {
			size_t u = ((size_t*)(((char*)rtt.first) + i))[0];
			u = (u >> 3) + ((u & 7) > 0);
			pair<char*, size_t> ads = c.decompress_chunk(make_pair((((char*)rtt.first) + i), sizeof(size_t) + u));
			i += sizeof(size_t) + u;
			char* new_r = new char[ads.second + r.second];
			if (r.first != nullptr) memcpy(new_r, r.first, r.second);
			memcpy(new_r + r.second, ads.first, ads.second);
			delete[] ads.first;
			if (r.first != nullptr) delete[] r.first;
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
		delete[] rtt.first;
	}


	void combo_test_asyncchunks(pair<char*, size_t> a, vector<size_t> chunk, bool out = true) {
		size_t sm = 0;
		for (auto i : chunk)
			sm += i;
		assert(a.second == sm);
		HaffmanCompressor c;
		sm = 0;
		for (size_t i = 0; i < chunk.size(); ++i) {
			c.add_chunk(make_pair(((char*)a.first) + sm, chunk[i]));
			sm += chunk[i];
		}
		pair<char*, size_t> rtt = c.prepare();
		sm = 0;
		for (size_t i = 0; i < chunk.size(); ++i) {
			pair<char*, size_t> ads = c.compress_chunk(make_pair(((char*)a.first) + sm, chunk[i]));
			sm += chunk[i];
			char* new_rtt = new char[ads.second + rtt.second];
			memcpy(new_rtt, rtt.first, rtt.second);
			memcpy(new_rtt + rtt.second, ads.first, ads.second);
			delete[] ads.first;
			delete[] rtt.first;
			rtt.first = new_rtt;
			rtt.second += ads.second;
		}
		pair<char*, size_t> r;
		r.second = 0;
		r.first = nullptr;
		c.try_decompress_codes(make_pair(rtt.first, c.Wall));
		for (size_t i = c.Wall; i < rtt.second; ) {
			size_t u = ((size_t*)(((char*)rtt.first) + i))[0];
			u = (u >> 3) + ((u & 7) > 0);
			pair<char*, size_t> ads = c.decompress_chunk(make_pair((((char*)rtt.first) + i), sizeof(size_t) + u));
			i += sizeof(size_t) + u;
			char* new_r = new char[ads.second + r.second];
			if (r.first != nullptr) memcpy(new_r, r.first, r.second);
			memcpy(new_r + r.second, ads.first, ads.second);
			delete[] ads.first;
			if (r.first != nullptr) delete[] r.first;
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
		delete[] rtt.first;
	}


	void simple1(bool out = true) {
		int* A = new int[10];
		for (int i = 0; i < 10; ++i)
			A[i] = i;
		short_code(make_pair((char*)A, 40), out);
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
		short_code(make_pair((char*)A, val * sizeof(int)), out);
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


	void combo_simple_shufted1(size_t size = 1000, size_t len = 10, size_t minsize = 100, bool out = false) {
		char* A = new char[size];
		for (int i = 0; i < size; ++i)
			A[i] = i & 255;
		vector<size_t> d;
		assert((size >= len * minsize));
		size_t ost = size - len * minsize;
		srand(1488);
		for (int i = 0; i < len - 1; ++i) {
			size_t y = rand();
			if (y > ost) {
				d.push_back(minsize);
			}
			else {
				d.push_back(y + minsize);
				ost -= y;
			}
		}
		d.push_back(ost + minsize);
		combo_test_asyncchunks(make_pair(A, size), d, out);
		delete[] A;
		if (!out) {
			std::cerr << "PASSED: c1(val=" << size << ", d={";
			if (d.size() > 50)d.resize(50);
			for (auto i : d)std::cerr << i << ',';
			if (d.size() == 50)std::cerr << "...\n";

			std::cerr << "})\n";
		}
	}
	void do_my_tests() {
		combo_simple1();
		combo_simple1(2500, 50, true);
		combo_simple1(2500, 2500, true);
		combo_simple1(10000, 100, false);
		combo_simple1(1000000, 1000, false);

		combo_simple_shufted1(100, 3, 10);
		combo_simple_shufted1();
		combo_simple_shufted1(10000, 100, 1);
		combo_simple_shufted1(10000, 4, 1);
		combo_simple_shufted1(100000, 1, 1);
		combo_simple_shufted1(100000, 10, 1);
		combo_simple_shufted1(100000, 10000, 5);

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

	void stable_test(size_t c = 10) {
		for (size_t i = 0; i < c; ++i) {
			combo_simple1(1000, 100, false);
			combo_simple_shufted1(1000, 5, 100, false);

			simple2(255, false);
			simple3(256, 144, false);
			simple4(256, 27342, false);
		}
	}

	void testLib() {
		_CrtMemState _ms;
		_CrtMemCheckpoint(&_ms);
		do_my_tests();
		stable_test();
		_CrtMemDumpAllObjectsSince(&_ms);
		_CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_FILE);
		_CrtSetReportFile(_CRT_WARN, _CRTDBG_FILE_STDOUT);
		_CrtDumpMemoryLeaks();
	}

	int main() { testLib(); return 0; }
}



namespace new_test {
	random_device dv;
	mt19937 r(dv());
	uniform_int_distribution<> dis(0, 255);

	pair<char*, size_t> get_new_data(size_t size) {
		char* res = new char[size];
		for (size_t i = 0; i < size; ++i)
			res[i] = dis(r);
		return make_pair(res, size);
	}

	bool equalP(pair<char*, size_t> lhs, pair<char*, size_t> rhs) {
		if (lhs.second != rhs.second)return false;
		if (memcmp(lhs.first, rhs.first, lhs.second))return false;
		return true;
	}

	pair<char*, size_t> concatP(pair<char*, size_t> lhs, pair<char*, size_t> rhs) {
		char* r = new char[lhs.second + rhs.second];
		memcpy(r, lhs.first, lhs.second);
		memcpy(r + lhs.second, rhs.first, rhs.second);
		return make_pair(r, lhs.second + rhs.second);
	}

	void joinP(pair<char*, size_t>& lhs, pair<char*, size_t> rhs) {
		char* d = new char[lhs.second + rhs.second];
		memcpy(d, lhs.first, lhs.second);
		memcpy(d + lhs.second, rhs.first, rhs.second);
		delete lhs.first;
		lhs.first = d;
		lhs.second += rhs.second;
	}

	void correct0(size_t s = 1024 * 12) {
		HaffmanCompressor a, b;
		pair<char*, size_t> c = get_new_data(s);
		pair<char*, size_t> c_comp = a.compress(c);
		pair<char*, size_t> c_decomp = a.decompress(c_comp);
		assert(equalP(c, c_decomp));
		delete c.first;
		delete c_comp.first;
		delete c_decomp.first;
	}


	void correct1(size_t s = 1024 * 12) {
		HaffmanCompressor a, b;
		pair<char*, size_t> c = get_new_data(s);
		pair<char*, size_t> c_comp = a.compress(c);
		pair<char*, size_t> c_decomp = b.decompress(c_comp);
		assert(equalP(c, c_decomp));
		delete c.first;
		delete c_comp.first;
		delete c_decomp.first;
	}

	void correct2(size_t s = 1024 * 12) {
		HaffmanCompressor a, b;
		pair<char*, size_t> c = get_new_data(s);
		pair<char*, size_t> c_comp = a.compress(c);
		pair<char*, size_t> c_decomp = b.decompress_data(c_comp);
		assert(equalP(c, c_decomp));
		delete c.first;
		delete c_comp.first;
		delete c_decomp.first;
	}

	void correct3(size_t s = 1024 * 12) {
		HaffmanCompressor a, b;
		pair<char*, size_t> c = get_new_data(s);
		pair<char*, size_t> c_comp = a.compress(c);
		a.clear();
		pair<char*, size_t> c_decomp = a.decompress_data(c_comp);
		assert(equalP(c, c_decomp));
		delete c.first;
		delete c_comp.first;
		delete c_decomp.first;
	}

	void correct4(size_t s = 1024 * 12) {
		HaffmanCompressor a, b;
		pair<char*, size_t> c = get_new_data(s);
		a.add_chunk(c);
		pair<char*, size_t> c_comp = a.prepare();
		pair<char*, size_t> c_comp1 = a.compress_chunk(c);
		joinP(c_comp, c_comp1);
		a.clear();
		pair<char*, size_t> c_decomp = a.decompress_data(c_comp);
		assert(equalP(c, c_decomp));
		delete c.first;
		delete c_comp.first;
		delete c_comp1.first;
		delete c_decomp.first;
	}

	void correct5(size_t s = 1024 * 12) {
		const size_t chunksize = 512;
		HaffmanCompressor a, b;
		pair<char*, size_t> c = get_new_data(s);
		a.add_chunk(c);
		pair<char*, size_t> c_comp = a.prepare();
		for (size_t i = 0; i < s; i += chunksize) {
			pair<char*, size_t> c_comp1 = a.compress_chunk(make_pair(c.first + i, min(s - i, chunksize)));
			joinP(c_comp, c_comp1);
			delete c_comp1.first;
		}
		a.clear();
		pair<char*, size_t> c_decomp = a.decompress_data(c_comp);
		assert(equalP(c, c_decomp));
		delete c.first;
		delete c_comp.first;
		delete c_decomp.first;
	}
	
	void correct6(size_t s = 1024 * 12) {
		const size_t chunksize = 512;
		HaffmanCompressor a, b;
		pair<char*, size_t> c = get_new_data(s);
		pair<char*, size_t> c_comp = a.compress(c);
		a.clear();
		pair<char*, size_t> c_decomp = make_pair(nullptr, 0);
		for (size_t i = 0; i < c_comp.second; i += chunksize) {
			pair<char*, size_t> c_decomp1 = a.decompress_data(make_pair(c_comp.first + i, min(c_comp.second - i, chunksize)));
			joinP(c_decomp, c_decomp1);
			delete c_decomp1.first;
		}
		assert(equalP(c, c_decomp));
		delete c.first;
		delete c_comp.first;
		delete c_decomp.first;
	}


	void correct7(size_t s = 1024 * 12) {
		const size_t chunksize = 512;
		HaffmanCompressor a, b;
		pair<char*, size_t> c = get_new_data(s);
		a.add_chunk(c);
		pair<char*, size_t> c_comp = a.prepare();
		for (size_t i = 0; i < s; i += chunksize) {
			pair<char*, size_t> c_comp1 = a.compress_chunk(make_pair(c.first + i, min(s - i, chunksize)));
			joinP(c_comp, c_comp1);
			delete c_comp1.first;
		}
		a.clear();
		pair<char*, size_t> c_etalon = b.decompress_data(c_comp);
		pair<char*, size_t> c_decomp = make_pair(nullptr, 0);
		for (size_t i = 0; i < c_comp.second; i += chunksize) {
			pair<char*, size_t> c_decomp1 = a.decompress_data(make_pair(c_comp.first + i, min(c_comp.second - i, chunksize)));
			joinP(c_decomp, c_decomp1);
			delete c_decomp1.first;
		}

		assert(equalP(c_etalon, c));
		assert(equalP(c, c_decomp));
		delete c.first;
		delete c_comp.first;
		delete c_decomp.first;
		delete c_etalon.first;
	}

	const size_t sizes[] = { 10, 100, 1000, 1023, 1024, 1025, 16 * 1024 - 1, 16 * 1024, 16 * 1024 + 1, 1488228 };
	bool correct_test(void (*f)(size_t), string index = ".") {
		cerr << "CORRECT TEST: " << index << "\t | ";
		try {
			for (auto i : sizes) {
				cerr << '.';
				f(i);
			}
		}
		catch (exception e) {
			cerr << " ERROR(except) " << endl;
			cerr << e.what() << endl;
			return false;
		}
		cerr << " OK " << endl;
		return true;
	}

	void correct_tests() {
		cerr << "TESTING: correct" << endl;

		assert(correct_test(correct0, "correct0"));
		assert(correct_test(correct1, "correct1"));
		assert(correct_test(correct2, "correct2"));
		assert(correct_test(correct3, "correct3"));
		assert(correct_test(correct4, "correct4"));
		assert(correct_test(correct5, "correct5"));
		assert(correct_test(correct6, "correct6"));


		assert(correct_test(correct7, "correct7.1"));
		assert(correct_test(correct7, "correct7.2"));
		assert(correct_test(correct7, "correct7.3"));

		cerr << "TESTS PASSED: correct" << endl;

	}


	void except0() {
		HaffmanCompressor a, b;
		pair<char*, size_t> t = get_new_data(4100);
		try {
			a.decompress_data(t);
		}
		catch (exception e) {
			delete t.first;
			throw e;
		}
	}

	void except1() {
		HaffmanCompressor a, b;
		pair<char*, size_t> t1 = get_new_data(4100);
		pair<char*, size_t> t2 = get_new_data(4100 - a.Wall);
		pair<char*, size_t> tc1 = a.compress(t1);
		b.decompress_data(make_pair(tc1.first, a.Wall));
		try {
			b.compress(t1);
		}
		catch (exception e) {
			delete t1.first;
			delete t2.first;
			delete tc1.first;
			throw e;
		}
	}

	void except2() {
		HaffmanCompressor a, b;
		pair<char*, size_t> t1 = get_new_data(4100);
		pair<char*, size_t> t2 = get_new_data(4100 - a.Wall);
		pair<char*, size_t> tc1 = a.compress(t1);
		try {
			b.decompress(make_pair(tc1.first, a.Wall - 1));
		}
		catch (exception e) {
			delete t1.first;
			delete t2.first;
			delete tc1.first;
			throw e;
		}
	}


	void except3() {
		HaffmanCompressor a, b;
		pair<char*, size_t> t1 = get_new_data(4100);
		pair<char*, size_t> t2 = get_new_data(4100 - a.Wall);
		pair<char*, size_t> tc1 = a.compress(t1);
		try {
			b.decompress_chunk(make_pair(tc1.first, a.Wall));
		}
		catch (exception e) {
			delete t1.first;
			delete t2.first;
			delete tc1.first;
			throw e;
		}
	}


	void except4() {
		HaffmanCompressor a, b;
		pair<char*, size_t> t1 = get_new_data(4100);
		pair<char*, size_t> t2 = get_new_data(4100 - a.Wall);
		pair<char*, size_t> tc1 = a.compress(t1);
		pair<char*, size_t> t1_dec = a.decompress(tc1);
		try {
			a.compress(t1);
		}
		catch (exception e) {
			delete t1.first;
			delete t2.first;
			delete tc1.first;
			delete t1_dec.first;
			throw e;
		}
	}


	bool exception_test(void (*f)(void), string index = ".") {
		cerr << "EXCEPTION TEST: " << index << "\t | ";
		try {
			f();
		}
		catch (exception e) {
			cerr << " OK " << endl;
			return true;
		}
		cerr << " ERROR(no except) " << endl;
		return false;
	}

	void exceprton_tests() {
		cerr << "TESTING: exception" << endl;

		assert(exception_test(except0, "except0"));
		assert(exception_test(except1, "except1"));
		assert(exception_test(except2, "except2"));
		assert(exception_test(except3, "except3"));
		assert(exception_test(except4, "except4"));

		cerr << "TESTS PASSED: exception" << endl;

	}

	void testing() {
		cerr << "TESTING: all" << endl;

		correct_tests();

		exceprton_tests();

		cerr << "TESTS PASSED: all" << endl;
	}

	void testLib() {
		_CrtMemState _ms;
		_CrtMemCheckpoint(&_ms);
		testing();
		_CrtMemDumpAllObjectsSince(&_ms);
		_CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_FILE);
		_CrtSetReportFile(_CRT_WARN, _CRTDBG_FILE_STDOUT);
		_CrtDumpMemoryLeaks();
	}

}

int main() { new_test::testLib(); return 0; }
