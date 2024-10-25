#include "../includes/utils.h"
#include <cstddef>
#include <cstdint>
#include <functional>
#include <iostream>
#include <memory>
#include <string>
#include <vector>
int main(int argc, const char **argv)
{
	std::ios::sync_with_stdio(false), std::cin.tie(nullptr);
	if (!(argc == 3))
	{
		std::cerr << "Usage: decompress input_filename output_filename\n";
		return 0;
	}
	const char *in_fn = argv[1], *out_fn = argv[2];
	Bitstream bs;
	bs.syncFrom(in_fn);
	std::string rep[128];
	for (int i = 1; i < 64; ++i)
		rep[i] += enc_to_ch(i);
	struct children
	{
		int l, r;
		children() = default;
		children(int l, int r) : l(l), r(r) { }
	};
	std::vector<children> key_ch;
	struct key_t
	{
		std::string s;
		int n;
		bool sgn;
		bool huff;
	};
	std::vector<key_t> key;
	std::vector<children> val_ch;
	std::vector<std::uint64_t> val;
	std::ofstream out_fs(out_fn);
	{
		int idx = 64;
		while (true)
		{
			int c = bs.extract(7);
			if (!c) break;
			do
			{
				rep[idx] += rep[c];
				c = bs.extract(7);
			}
			while (c);
			++idx;
		}
	}
	{
		std::function<int ()> DFS = [&]() {
			int u = key_ch.size();
			key.emplace_back();
			if (bs.extract(1))
			{
				key_ch.emplace_back();
				int l = DFS(), r = DFS();
				key_ch[u] = {l, r};
			}
			else
			{
				key_ch.emplace_back(0, 0);
				key_t &k = key.back();
				while (true)
				{
					int c = bs.extract(7);
					if (!c) break;
					k.s += rep[c];
				}
				k.n = bs.extract(6) + 1;
				k.sgn = bs.extract(1);
				k.huff = bs.extract(1);
			}
			return u;
		};
		if (bs.extract(1)) DFS();
	}
	{
		std::function<int ()> DFS = [&]() {
			int u = val_ch.size();
			if (bs.extract(1))
			{
				val_ch.emplace_back();
				val.emplace_back();
				int l = DFS(), r = DFS();
				val_ch[u] = {l, r};
			}
			else
			{
				val_ch.emplace_back(0, 0);
				val.push_back(bs.extract(8 << bs.extract(2)));
			}
			return u;
		};
		if (bs.extract(1)) DFS();
	}
	{
		auto infer_dim_cnt = [](const std::string &s) {
			int cnt = 0;
			for (std::size_t i = 0; i < s.size(); ++i)
				if (s[i] == '_' && (s[i + 1] == '_' || i + 1 == s.size()))
					++cnt;
			return cnt;
		};
		auto insert_dim_print = [&](const std::string &s, int *dim) {
			for (std::size_t i = 0; i < s.size(); ++i)
			{
				out_fs << s[i];
				if (s[i] == '_' && (s[i + 1] == '_' || i + 1 == s.size()))
					out_fs << *dim++;
			}
		};
		while (true)
		{
			bool first = true;
			while (true)
			{
				int u = 0;
				while (key_ch[u].l)
					if (!bs.extract(1))
						u = key_ch[u].l;
					else
						u = key_ch[u].r;
				auto &k = key[u];
				if (k.s.empty())
					if (first)
						return 0;
					else
						break;
				int dim_cnt = infer_dim_cnt(k.s);
				std::function<void (std::uint64_t)> print_value;
				if (k.sgn)
					print_value = [&](std::uint64_t x) {
						std::cerr << "HERE\n";
						out_fs << static_cast<std::int64_t>(x >> k.n - 1 & 1 ? ~0ll << k.n | x : x);
					};
				else
					print_value = [&](std::uint64_t x) {
						out_fs << x;
					};
				std::function<std::uint64_t ()> parse_value;
				if (k.huff)
					parse_value = [&]() {
						int u = 0;
						while (val_ch[u].l)
							if (!bs.extract(1))
								u = val_ch[u].l;
							else
								u = val_ch[u].r;
						return val[u];
					};
				else
					parse_value = [&]() {
						return bs.extract(k.n);
					};
				auto dim = std::make_unique<int[]>(dim_cnt);
				std::function<void (int)> DFS = [&](int i) {
					if (i == dim_cnt)
					{
						if (first)
						{
							first = false;
							out_fs << "{ ";
						}
						else
							out_fs << ", ";
						out_fs << "\"", insert_dim_print(k.s, dim.get()), out_fs << "\"";
						out_fs << ": ";
						print_value(parse_value());
						return;
					}
					int ni = bs.extract(3);
					for (int j = 0; j <= ni; ++j)
						dim[i] = j, DFS(i + 1);
				};
				DFS(0);
			}
			out_fs << " }\n";
		}
	}
}
