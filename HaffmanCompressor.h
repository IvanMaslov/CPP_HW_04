#pragma once
#include <vector>
#include <bitset>
#include <queue>
#include <iostream>
using namespace std;
class HaffmanCompressor
{
	static const size_t W = 256;
	static const size_t WW = 2 * (W * 2 - 1);
	static const size_t WWbit = WW / 8 + (WW % 8 > 0);
	bitset<WW> res_tree;
	vector<char> res_char;
	deque<char> q;
	bool decompressing = false;
	bool compressing = false;
	struct node {
		node* l, * r;
		size_t count;
		char val;
		char rang;
		node() {
			rang = val = count = 0;
			l = r = nullptr;
			count = 1;
		}
		node(char val) : val(val) {
			rang = count = 0;
			l = r = nullptr;
			count = 1;
		}
		~node() {
			delete l;
			delete r;
		}
	};
	size_t cnt_char[W];
	node* cnt[W];
	node* result = nullptr;
	void dfs_set(node* arg, size_t& pos, char rang);
	bool __DEBUG__ISOMORPH(node* a, node* b);
public:
	static const size_t Wall = WWbit + W;
	
	HaffmanCompressor();
	~HaffmanCompressor() { delete result; }
	
	void clear();
	
	bool iscomress() const { return compressing; }
	bool isdecomress() const { return decompressing; }
	
	void add_chunk(pair<char*, size_t> arg);
	pair<char*, size_t> prepare();
	pair<char*, size_t> compress_chunk(pair<char*, size_t> arg);
	pair<char*, size_t> compress(pair<char*, size_t> arg);

	bool try_decompress_codes(pair<char*, size_t> arg);
	pair<char*, size_t> decompress_chunk(pair<char*, size_t> arg);
	pair<char*, size_t> decompress_data(pair<char*, size_t> arg);
	pair<char*, size_t> decompress(pair<char*, size_t> arg);
};
