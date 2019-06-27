#pragma once
#include "../shared_in.h"

LIB_NAMESPACE_START

	void Compress(const byte_t* input, uint64_t len, std::string* output);

	void Uncompress(const byte_t* input, std::string* output);

LIB_NAMESPACE_END

static void __compress_benchmark()
{
	static const std::string liters = "a";//"abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";

	std::vector<int> len = { 1,2,4,8,16,32,64,128,256,512,1024,2048,4096,8192,16384,32768,65536 };
	std::vector<std::string> table;

	table.resize(len.size());

	ForArray(i, len.size())
	ForArray(d, len[i])
		table[i].push_back(liters[math::rand64() % liters.size()]);


	struct metric
	{
		size_t size_in;
		size_t size_out;
		double ratio;
		double time;
	};

	std::vector<metric> metrics;

	for (const auto& str : table)
	{
		SuperTimer st;

		std::string compressed;

		st.SetStart();
		ForArray(i, 1000)
		{
			//auto s = snappy::Compress(&str[0], str.size() , &output);
			Compress((byte_t*)&str[0], str.size(), &compressed);
		}
		st.SetEnd();

		std::string restored;
		//auto out = snappy::Uncompress(&output[0], output.size() , &restored);
		Uncompress((byte_t*)&compressed[0], &restored);
		BREAK_IF(str != restored);

		metrics.push_back(
			{
				str.size(),
				compressed.size(),
				double(compressed.size()) / str.size(),
				st.GetTimeReal()
			}
		);
	}

	for (const auto& met : metrics)
	{
		log::info << "----------------------";
		log::info << str::format::insert("size in   : {}", met.size_in);
		log::info << str::format::insert("size out  : {}", met.size_out);
		log::info << str::format::insert("size ratio: {}", met.ratio);
		log::info << str::format::insert("total time: {}", met.time);
	}
}