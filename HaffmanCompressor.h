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
			rang = count = val = 0;
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
	node* cnt[W];
	node* result;
	void dfs_clear(node* arg);
	void dfs_set(node* arg, size_t& pos, char rang);
	bool __DEBUG__ISOMORPH(node* a, node* b);
public:
	static const size_t Wall = WWbit + W;
	HaffmanCompressor();
	~HaffmanCompressor() { delete result; }//dfs_clear(result); };

	bool iscomress() const { return compressing; }
	bool isdecomress() const { return decompressing; }
	pair<void*, size_t> compress(pair<void*, size_t> arg);
	void add_chunk(pair<void*, size_t> arg);
	pair<void*, size_t> prepare();
	pair<void*, size_t> compress_chunk(pair<void*, size_t> arg) ;

	bool try_decompress_codes(pair<void*, size_t> arg);
	pair<void*, size_t> decompress(pair<void*, size_t> arg);
	pair<void*, size_t> decompress_chunk(pair<void*, size_t> arg);
	pair<void*, size_t> decompress_data(pair<void*, size_t> arg);
};
