#include "HaffmanCompressor.h"
#include <cstring>
#include <set>
#include <malloc.h>
#include <cassert>

#define ref first
#define siz second
#define _mp(x, y) make_pair(x, y)
void HaffmanCompressor::dfs_clear(node* arg)
{
	if (arg == nullptr) return;
	if (arg->l != nullptr)dfs_clear(arg->l);
	if (arg->r != nullptr)dfs_clear(arg->r);
	delete arg;
}

void HaffmanCompressor::dfs_set(node* arg, size_t& pos, char rang)
{
	if (arg == nullptr) return;
	arg->rang = rang;
	bool h = true;
	if (arg->l != nullptr) {
		h = false;
		arg->l->count = arg->count << 1;
		res_tree.set(pos++);
	}
	else res_tree.reset(pos++);

	if (arg->r != nullptr) {
		h = false;
		arg->r->count = (arg->count << 1) ^ 1;
		res_tree.set(pos++);
	}
	else res_tree.reset(pos++);

	dfs_set(arg->l, pos, rang + 1);
	dfs_set(arg->r, pos, rang + 1);

	if (h) 
		res_char.push_back(arg->val);
}

bool HaffmanCompressor::__DEBUG__ISOMORPH(node* a, node* b)
{
	assert((a == nullptr) == (b == nullptr));
	if (a == nullptr)return true;
	assert(__DEBUG__ISOMORPH(a->l, b->l));
	assert(__DEBUG__ISOMORPH(a->r, b->r));
	assert(a->val == b->val);
	return true;
}

HaffmanCompressor::HaffmanCompressor()
{
	result = nullptr;
	for (size_t i = 0; i < W; ++i)
		cnt[i] = new node(static_cast<char>(i));
}

pair<void*, size_t> HaffmanCompressor::compress(pair<void*, size_t> arg) {
	add_chunk(arg);
	pair<void*, size_t> res1 = prepare();
	pair<void*, size_t> res2 = compress_chunk(arg);
	char* res = new char[res1.siz + res2.siz];
	memcpy(res, res1.ref, res1.siz);
	memcpy(res + res1.siz, res2.ref, res2.siz);
	delete[](char*)res1.ref;
	delete[](char*)res2.ref;
	return _mp(res, res1.siz + res2.siz);
}

void HaffmanCompressor::add_chunk(pair<void*, size_t> arg)
{
	if (compressing)
		throw new runtime_error("ERROR: add_chunk() while compressing");
	for (size_t i = 0; i < arg.siz; ++i)
		++(cnt[(((char*)arg.ref)[i]) + 128]->count);
}

pair<void*, size_t> HaffmanCompressor::prepare()
{
	if (compressing)
		throw new runtime_error("ERROR: prepare() used second time");
	compressing = true;
	set<pair<pair<size_t, char>, node*>> s;
	for (size_t i = 0; i < W; ++i) {
		s.insert(make_pair(make_pair(cnt[i]->count, cnt[i]->val), cnt[i]));
	}
	while (s.size() > 1) {
		pair<pair<size_t, char>, node*> p1, p2;
		p1 = *s.begin();
		s.erase(s.begin());
		p2 = *s.begin();
		s.erase(s.begin());
		result = new node();
		result->l = p1.second;
		result->r = p2.second;
		result->count = p1.first.first + p2.first.first;
		s.insert(make_pair(make_pair(result->count, min(p1.first.second, p2.first.second)), result));
	}
	size_t index = 0;
	result->count = 0;
	dfs_set(result, index, 0);
#ifdef _DEBUG
	assert(index == WW);
	assert(res_char.size() == W);
#endif
	char* res = new char[Wall];
	memset(res, 0, Wall);
	for (size_t i = 0; i < WW; ++i)
		res[i >> 3] ^= static_cast<char>(res_tree[i]) << (i & 7);
	for (size_t u = 0; u < W; ++u)
		res[WWbit + u] = res_char[u];
	return _mp(res, Wall);
}

pair<void*, size_t> HaffmanCompressor::compress_chunk(pair<void*, size_t> arg)
{
	if (!compressing)
		throw new runtime_error("ERROR: compress_chunk() without preparing");
	vector<char> ans;
	char bf = 0;
	char size_bf = 8;
	for (size_t i = 0; i < arg.siz; ++i) {
		node* v = cnt[(((char*)(arg.ref))[i]) + 128];
		for (char u = v->rang - 1; u >= 0; --u) {
			bf ^= ((v->count >> u) & 1) << (--size_bf);
			if (!size_bf) {
				ans.push_back(bf);
				size_bf = 8;
				bf = 0;
			}
		}
	}
	if (size_bf != 8)
		ans.push_back(bf);
	else size_bf = 0;
	char* res = new char[sizeof(size_t) + ans.size()];
	((size_t*)res)[0] = static_cast<size_t>((ans.size() << 3) - size_bf);
	char* res_ = (char*)(((size_t*)(res)) + 1);
	for (size_t u = 0; u < ans.size(); ++u)
		res_[u] = ans[u];
	return _mp(res, sizeof(size_t) + ans.size());
}

bool HaffmanCompressor::try_decompress_codes(pair<void*, size_t> arg)
{
	if(decompressing)
		return false;
	decompressing = true;

	if (arg.second != Wall)
		return false;
	dfs_clear(result);
	//node* res_cop = result; result = nullptr;

	result = new node();
	vector<node*> st;
	st.push_back(result);
	size_t cnt_alph = 0;
	for (size_t i = 0; i < WW; ++i) {
		bool btl = (((char*)(arg.ref))[(i >> 3)] >> (i & 7)) & 1;
		++i;
		bool btr = (((char*)(arg.ref))[(i >> 3)] >> (i & 7)) & 1;
		node* c = st.back();
		st.pop_back();
		if (btr) {
			c->r = new node();
			st.push_back(c->r);
		}
		if (btl) {
			c->l = new node();
			st.push_back(c->l);
		}
		if (!btl && !btr) {
			if (cnt_alph == W) {
				dfs_clear(result);
				return false;
			}
			c->val = (((char*)(arg.ref))[WWbit + cnt_alph++]);
		}
	}
	if (cnt_alph != W) {
		dfs_clear(result);
		return false;
	}
#ifdef _DEBUG
	//if(compressing)
	//	__DEBUG__ISOMORPH(result, res_cop);
#endif
	//dfs_clear(res_cop);
	return true;
}

pair<void*, size_t> HaffmanCompressor::decompress(pair<void*, size_t> arg)
{
	if(!try_decompress_codes(_mp(arg.ref, Wall))) throw new runtime_error("ERROR: decompress() failed");
	return decompress_chunk(_mp((void*)(((char*)arg.ref) + Wall), arg.siz - Wall));
}
#include <iostream>
pair<void*, size_t> HaffmanCompressor::decompress_chunk(pair<void*, size_t> arg)
{
	size_t len = ((size_t*)(arg.ref))[0];
	if (len / 8 + (len % 8 != 0) != arg.siz - sizeof(size_t))
		throw new runtime_error("ERROR: decompress_chunk with different length");
	vector<char> ans;
	node* cur = result;
	for (size_t i = 0; i < len; ++i) {
		bool cur_bit = (((char*)(arg.ref))[sizeof(size_t) + (i >> 3)] >> (7 - (i & 7))) & 1;
		if (!cur_bit) 
			cur = cur->l;
		else
			cur = cur->r;
		if (cur->l == nullptr && cur->r == nullptr) {
			ans.push_back(cur->val);
			cur = result;
		}
	}
	char* res = new char[ans.size()];
	for (size_t i = 0; i < ans.size(); ++i)
		res[i] = static_cast<char>(ans[i] - 128);
	return _mp(res, ans.size());
}

pair<void*, size_t> HaffmanCompressor::decompress_data(pair<void*, size_t> arg)
{
	if (compressing)
		throw runtime_error("ERROR: decompress_data() while compressing");
	for (size_t i = 0; i < arg.siz; ++i)
		q.push_back(static_cast<char*>(arg.ref)[i]);
	if (!decompressing) {
		if (q.size() >= Wall) {
			char t[Wall];
			for (size_t i = 0; i < Wall; ++i) {
				t[i] = q.front();
				q.pop_front();
			}
			if(!try_decompress_codes(make_pair(t, sizeof(t)))){
				throw runtime_error("ERROR: No valid prepared data");
			}
		}
		else {
			return make_pair(nullptr, 0);
		}
	}
	vector<char> ans;
	while (!q.empty()) {
		size_t y = 0;
		char* t = (char*)& y;
		if (q.size() < sizeof(size_t))
			return make_pair(nullptr, 0);
		for (size_t i = 0; i < sizeof(size_t); ++i) {
			t[i] = q.front();
			q.pop_front();
		}
		size_t Y = (y >> 3) + ((y & 7) > 0);
		if (q.size() < Y) {
			for (size_t i = 0; i < sizeof(size_t); ++i) {
				q.push_front(t[sizeof(size_t) - 1 - i]);
			}
			break;
		}
		char* dat = new char[sizeof(size_t) + Y];
		for (size_t i = 0; i < sizeof(size_t); ++i) {
			dat[i] = t[i];
		}
		for (size_t i = 0; i < Y; ++i) {
			dat[sizeof(size_t) + i] = q.front();
			q.pop_front();
		}
		auto c = decompress_chunk(make_pair(dat, Y + sizeof(size_t)));
		for (size_t i = 0; i < c.second; ++i)
			ans.push_back(static_cast<char*>(c.first)[i]);
		delete[] (char*)c.first;
		delete[] (char*)dat;
	}
	char* e = new char[ans.size()];
	for (size_t i = 0; i < ans.size(); ++i)
		e[i] = ans[i];
	return make_pair(e, ans.size());
}
