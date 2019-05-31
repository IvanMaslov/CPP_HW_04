#include <iostream>
#include <fstream>
#include "HaffmanCompressor.h"
//#include "Test.cpp"

using namespace std;

void compress(string in_name, string out_name, size_t chunksize = 1024) {
	HaffmanCompressor compressor;
	std::ifstream infile;
	std::ofstream outfile;
	infile.open(in_name.c_str(), std::ios_base::binary);
	outfile.open(out_name.c_str(), std::ios_base::binary);
	if (!infile.is_open()) {
		std::cerr << "ERROR while open file: " << in_name << " " << std::endl;
		exit(0);
	}
	if (!outfile.is_open()) {
		std::cerr << "ERROR while open file: " << in_name << " " << std::endl;
		exit(0);
	}

	char* buf = new char[chunksize];
	size_t bufsize = 0;
	while (!infile.eof()) {
		buf[bufsize++] = infile.get();
		if (bufsize == chunksize) {
			compressor.add_chunk(std::make_pair(buf, bufsize));
			bufsize = 0;
		}
	}
	if (bufsize) {
		compressor.add_chunk(std::make_pair(buf, bufsize));
		bufsize = 0;
	}

	infile.close();
	infile.open(in_name.c_str(), std::ios_base::binary);
	
	char* prepared_data;
	prepared_data = (char*)compressor.prepare().first;
	outfile.write(static_cast<const char*>(prepared_data), compressor.Wall);
	delete[] prepared_data;

	while (!infile.eof()) {
		buf[bufsize++] = infile.get();
		if (bufsize == chunksize) {
			std::pair<void*, size_t> res = compressor.compress_chunk(std::make_pair(buf, bufsize));
			outfile.write(static_cast<const char*>(res.first), res.second);
			bufsize = 0;
		}
	}
	if (bufsize) {
		std::pair<void*, size_t> res = compressor.compress_chunk(std::make_pair(buf, bufsize));
		outfile.write(static_cast<const char*>(res.first), res.second);
		bufsize = 0;
	}


	delete[] buf;
	infile.close();
	outfile.close();

#ifdef _DEBUG
	infile.open(out_name.c_str(), ios::binary);
	buf = new char[compressor.Wall];
	bufsize = 0;
	std::cerr << "CHECK\n";
	while (!infile.eof()) {
		buf[bufsize++] = infile.get();
		if (bufsize == compressor.Wall) {
			if (!compressor.try_decompress_codes((std::make_pair(buf, bufsize)))) {
				std::cerr << "ERROR file:" << in_name << " is broken" << std::endl;
				exit(0);
			}
			std::cerr << "EQUAL\n";
			bufsize = 0;
			break;
		}
	}
	infile.close();
	delete[] buf;
#endif // _DEBUG
}



void decompress(string in_name, string out_name) {
	HaffmanCompressor decompressor;
	std::ifstream infile;
	std::ofstream outfile;
	infile.open(in_name.c_str(), std::ios_base::binary);
	outfile.open(out_name.c_str(), std::ios_base::binary);
	if (!infile.is_open()) {
		std::cerr << "ERROR while open file: " << in_name << " " << std::endl;
		exit(0);
	}
	if (!outfile.is_open()) {
		std::cerr << "ERROR while open file: " << in_name << " " << std::endl;
		exit(0);
	}

	char* buf = new char[decompressor.Wall];
	size_t bufsize = 0;
#define __N2
#ifdef __N1
	while (!infile.eof()) {
		buf[bufsize++] = infile.get();
		if (bufsize == decompressor.Wall) {
			if (!decompressor.try_decompress_codes((std::make_pair(buf, bufsize)))) {
				std::cerr << "ERROR file:" << in_name << " is broken" << std::endl;
				exit(0);
			}
			bufsize = 0;
			break;
		}
	}
#endif __N1
#ifdef __N2
	bufsize = decompressor.Wall;
	infile.read(buf, bufsize);
	if (!decompressor.try_decompress_codes((std::make_pair(buf, bufsize)))) {
		std::cerr << "ERROR file:" << in_name << " is broken" << std::endl;
		exit(0);
	}
#endif __N1

	delete[] buf;
	bufsize = 0;

	while (!infile.eof()) {
#define __L2
#ifdef __L1
		void* data = static_cast<void*>(new size_t);
		infile.get((char*)data, sizeof(size_t));
		if (infile.eof()) break;
		size_t u = *static_cast<size_t*>(data);

		u = (u >> 3) + ((u & 7) > 0);
		buf = new char[u + sizeof(size_t)];
		memcpy(buf, data, sizeof(size_t));
		delete static_cast<size_t*>(data);
		infile.get(buf + sizeof(size_t), u);
#endif //__L1
#ifdef __L2
		infile.read(((char*)(&bufsize)), sizeof(size_t));
		if (infile.eof()) break;
		size_t u = bufsize;
		u = (u >> 3) + ((u & 7) > 0);
		buf = new char[u + sizeof(size_t)];
		((size_t*)(buf))[0] = bufsize;
		infile.read(buf + sizeof(size_t), u);
#endif //__L2


		pair<void*, size_t> d;
#ifdef _DEBUG
		std::cerr << u << std::endl;
		std::cerr << infile.eof() << std::endl;
		size_t r = ((size_t*)(buf))[0];
		std::cerr << r << std::endl;
		std::cerr << (int)buf[0] << ' ' << (int)buf[1] << ' ' << (int)buf[2] << ' ' << (int)buf[3] << std::endl;
#endif
		try {
			d = decompressor.decompress_chunk(std::make_pair(buf, u + sizeof(size_t)));
		}
		catch (std::exception const e) {
			std::cerr << e.what() << std::endl;
			exit(0);
		}
		outfile.write((char*)d.first, d.second);
		delete[] d.first;
		delete[] buf;

	}

	infile.close();
	outfile.close();
}

int main(int argc, char* argv[]) {
	if (argc == 1) {
		std::cerr << "ConHafCompr [options] --mode={encode|decode} sourse destination " << std::endl;
		return 0;
	}
	if (argc == 4) {
		if (strcmp(argv[1], "--mode=encode") == 0) {
			compress(argv[2], argv[3]);
		}
		if (strcmp(argv[1], "--mode=decode") == 0) {
			decompress(argv[2], argv[3]);
			system("pause");
		}
		return 0;
	}
	std::cerr << "WRONG OPETION: " << argc << " arguments do not supported" << std::endl;
	return 0;
}
