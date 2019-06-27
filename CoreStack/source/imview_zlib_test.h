#pragma once
#include "pch.h"

namespace imview
{
	void zlib_test();
}

//////////////////////////////////////////////////////////////////////////

static void imview::zlib_test()
{
	ImGui::Text("disabled");
}

//static void zlib()
//
//	if (ImGui::Button("Raw data compress"))
//	{
//		static std::string str = "";
//
//		str.resize(800 * 800);
//
//		ForArray(i, str.size())
//			str[i] = (uint8_t)math::Random('A', 'z');
//
//		SuperTimer st;
//
//		st.SetStart();
//		auto buffer = str::compress::compress_data(str.c_str(), str.size());
//		std::string out(buffer.orig_size, '#');
//		str::compress::decompress_data(buffer, &out[0]);
//		buffer.release();
//		st.SetEnd();
//
//		std::cout << "c data " << st.GetTimeReal() << " " << buffer.comp_size / float(str.size()) << "\n";
//
//		st.SetStart();
//		std::string std_c = str::compress::compress_string(str);
//		std::string std_o = str::compress::decompress_string(std_c);
//		st.SetEnd();
//
//		std::cout << "c str  " << st.GetTimeReal() << " " << std_c.size() / float(str.size()) << "\n";
//
//		assert(str == out);
//		assert(str == std_o);
//
//	}
//	ImGui::Separator();
//
//	//------// data
//
//	static char dat_path_buffer[256] = "";
//	static char out_path_buffer[256] = "";
//	static char rec_path_buffer[256] = "";
//
//	static string status_in = "Add file path...";
//	static string status_out = "Out file path...";
//	static string status_rec = "Rec file path...";
//
//	//------// gui
//
//	bool str_in_changed =
//		ImGui::InputText("##iid", dat_path_buffer, IM_ARRAYSIZE(dat_path_buffer)); ImGui::SameLine(); ImGui::Text(status_in.c_str());
//	ImGui::InputText("##oid", out_path_buffer, IM_ARRAYSIZE(dat_path_buffer)); ImGui::SameLine(); ImGui::Text(status_out.c_str());
//	ImGui::InputText("##fid", rec_path_buffer, IM_ARRAYSIZE(rec_path_buffer)); ImGui::SameLine(); ImGui::Text(status_rec.c_str());
//
//	bool need_copress = ImGui::Button("Compress"); ImGui::SameLine();
//	bool need_depress = ImGui::Button("Deompress");
//
//	//------// procc
//
//	if (str_in_changed)
//	{
//		if (fs::exists(fs::path(dat_path_buffer)) && fs::is_regular_file(fs::path(dat_path_buffer)))
//		{
//			status_in = "[OK]";
//
//			string out = string(dat_path_buffer) + ".dat";
//			string rec = string(dat_path_buffer) + ".rec";
//			memcpy(out_path_buffer, out.c_str(), out.size());
//			memcpy(rec_path_buffer, rec.c_str(), rec.size());
//
//		}
//		else
//			status_in = "Bad file path";
//	}
//
//	const int flags_read = std::ios::binary;
//	const int flags_write = std::ios::binary | std::ios::trunc;
//
//	if (need_copress)
//	{
//		std::ifstream file(dat_path_buffer, flags_read);
//
//		if (file)
//		{
//			file.seekg(0, std::ios::end); string data(file.tellg(), '\0');
//			file.seekg(0, std::ios::beg); file.read(&data[0], data.size());
//			//-------------------------------//
//			std::ofstream out_stream(out_path_buffer, flags_write);
//			string cmp = str::compress::compress_string(data);
//			out_stream.write(cmp.c_str(), cmp.size());
//		}
//	}
//	//--------------------------------------------------------------
//	if (need_depress)
//	{
//		std::ifstream file(out_path_buffer, flags_read);
//
//		if (file)
//		{
//			file.seekg(0, std::ios::end); string data(file.tellg(), '\0');
//			file.seekg(0, std::ios::beg); file.read(&data[0], data.size());
//			//-------------------------------//
//			std::ofstream out_stream(rec_path_buffer, flags_write);
//			string cmp = str::compress::decompress_string(data);
//			out_stream.write(cmp.c_str(), cmp.size());
//		}
//	}
//}