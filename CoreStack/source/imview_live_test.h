#pragma once
#include "pch.h"


constexpr int distor(int val, int min, int max)
{
	if (val < min) return max - (min - val);
	if (val >= max) return min + (val - max);
	return val;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
namespace
{
	static void DrawWindowLV(std::string name, bool* opened_ptr)
	{
 		if (ImGui::Begin(name.c_str(), opened_ptr, 0))
		{
			enum { SIZE_W = 256 };
			enum { SIZE_H = 256 };

			static bitmap32 ram_world(SIZE_W, SIZE_H, color32{ (uint8_t)120,(uint8_t)0,(uint8_t)0,(uint8_t)255 });
			static bitmap32 temp_buff(SIZE_W, SIZE_H, color32{ (uint8_t)120,(uint8_t)0,(uint8_t)0,(uint8_t)255 });
			static GpuTexture gpu_texture;

			static GpuSampler sampler_point;
			static GpuSampler sampler_lenear;

			MakeOnce
			{
				bitmap32 filler(64, 64, color32{
				(uint8_t)190,
				(uint8_t)190,
				(uint8_t)190,
				(uint8_t)255 });

				sampler_point.CreateSampler(SamplerState::GetPresetPoint());
				sampler_lenear.CreateSampler(SamplerState::GetPresetLinear());

				gpu_texture.Update(filler);
 			}

			static SuperTimer ST;


			ImGuiIO&		io = ImGui::GetIO();
			ImGuiContext*	context = ImGui::GetCurrentContext();
			ImGuiWindow*	window = context->CurrentWindow;
			ImDrawList*		drawer = window->DrawList;


			ImItem::ShowImage(&gpu_texture, true, { 1200, 1200 });


			//---------------------------------------------------------------------// gui calls
			ImGui::SameLine();
			ImGui::BeginGroup();
			{
				auto pos = ImGui::GetCursorPos() + ImGui::GetWindowPos();
				auto size = ImGui::GetContentRegionAvail();

				//if (pos.x < pos.x + size.x && pos.y < pos.y + size.y)
				//	drawer->AddRect(pos - FLOAT2{ 2,2 }, pos + size, 0xFF000020);

				//////////////////////////////////////////////////////////////////////////
				static SuperTimer ST;
				static float _time_escaped = 0;

				static bool _auto_update = false;
				static bool _update_frame = false;

				static bool lenear_sampler = false;

				// params
				static int size_w = SIZE_W;
				static int size_h = SIZE_H;
				static float cells_fps_target = 30;

				static int stat_pass_round		= 0;//dummy
 				static int stat_alive_shells	= 0;
				static int stat_generation_seed = math::Random(0, 100000000);//dummy

				static int	 size_w_setter = size_w;
				static int	 size_h_setter = size_h;
				static float cells_fps_target_setter = cells_fps_target;


				static bool alt_generator = false;

				// dock gui

				ImGui::PushItemWidth(160);//------------------------------------------PushItemWidth

				ImGui::Text("Generation seed:");
				ImGui::InputInt("##0", &stat_generation_seed, 0, 0);
				ImGui::Text("Pass round:");
				ImGui::InputInt("##1", &stat_pass_round, 0, 0);
				ImGui::Text("Live cells:");
				ImGui::InputInt("##2", &stat_alive_shells, 0, 0);

				ImGui::Text("Fps target:");
				ImGui::InputFloat("##3", &cells_fps_target_setter, 0, 0);
				ImGui::SameLine();
				if (ImGui::SmallButton("set"))
					cells_fps_target = cells_fps_target_setter;

				ImGui::Checkbox("Lenear filter", &lenear_sampler);

				ImGui::Separator();//-------------------------------------separator

				ImGui::InputInt("MapSizeW", &size_w_setter, 1, 10);
				ImGui::InputInt("MapSizeH", &size_h_setter, 1, 10);


				ImGui::PopItemWidth();//----------------------------------------------PopItemWidth




				//=====================================================================// updaters
				if (ImGui::Button("Clear"))
				{
					ram_world.fill_by_color({ 0,0,0,255 });
					stat_pass_round = 0;
				}
				if (ImGui::Button("Resize & Randomize"))
				{
					size_w = size_w_setter;
					size_h = size_h_setter;

					ram_world = bitmap32(size_w, size_h, { 0,0,0,255 });

					ForArray(h, ram_world.size().y - ram_world.size().y * 0.3)
					ForArray(w, ram_world.size().x - ram_world.size().x * 0.3)
					{
						ram_world.raw_data(
							w + ram_world.size().x * 0.15,
							h + ram_world.size().y * 0.15)->r = math::rand64() % 2 == 0 ? 255 : 0;
					}
					stat_pass_round = 0;
				}
				if (ImGui::Button("Get from copyboard"))
				{
					//auto result = os::io::clipboard::GetClipboardImageRGBA();
					//if (result.IsSucceed())
					//{
					//	auto copypaste = result.GetValue();
					//
					//	size_w = size_w_setter = copypaste.w;
					//	size_h = size_h_setter = copypaste.h;
					//
					//	ram_world.copy_from_ptr(copypaste.w, copypaste.h, copypaste.data);
					//
					//	bitmap32 convert(size_w, size_h);
					//
					//	ForArray(w, ram_world.w)
					//	ForArray(h, ram_world.h)
					//	{
					//		convert.raw_data(w, h)->b = ram_world.raw_data(w, h)->r;
					//		convert.raw_data(w, h)->g = ram_world.raw_data(w, h)->g;
					//		convert.raw_data(w, h)->r = ram_world.raw_data(w, h)->b;
					//		convert.raw_data(w, h)->a = ram_world.raw_data(w, h)->a;
					//	}
					//	ram_world = convert;
					//
					//	stat_pass_round = 0;
					//}
				}

				if ("read file")
				{
					ImGui::SameLine();
					//if (ImGui::Button("get.1"))
					//{
					//	auto copypaste = lodepng_read_image("assets/aaa.png");
					//
					//	size_w = size_w_setter = copypaste.w;
					//	size_h = size_h_setter = copypaste.h;
					//
					//	ram_world.copy_from_ptr(copypaste.w, copypaste.h, copypaste.data);
					//
					//	stat_pass_round = 0;
					//
					//} ImGui::SameLine();
					//if (ImGui::Button("get.2"))
					//{
					//	auto copypaste = lodepng_read_image("assets/bbb.png");
					//
					//	size_w = size_w_setter = copypaste.w;
					//	size_h = size_h_setter = copypaste.h;
					//
					//	ram_world.copy_from_ptr(copypaste.w, copypaste.h, copypaste.data);
					//
					//	stat_pass_round = 0;
					//} ImGui::SameLine();
					//if (ImGui::Button("get.3"))
					//{
					//	auto copypaste = lodepng_read_image("assets/ccc.png");
					//
					//	size_w = size_w_setter = copypaste.w;
					//	size_h = size_h_setter = copypaste.h;
					//
					//	ram_world.copy_from_ptr(copypaste.w, copypaste.h, copypaste.data);
					//
					//	stat_pass_round = 0;
					//}
				}

				//=====================================================================// time update
				if (_auto_update)
				{
					_time_escaped += ST.GetRoundTime();

					if (_time_escaped > (1.0 / cells_fps_target))
					{
						_update_frame = true;
					}
				}

				if (ImGui::Button("Update")) _update_frame = true;
				ImGui::SameLine();
				ImGui::Checkbox("AutoUpdate", &_auto_update);

				//=====================================================================// gens
				ImGui::Dummy({ 10,10 }); ImGui::Separator(); ImGui::Dummy({ 10,10 });
				ImGui::Text("Gen switchers:");

				static bool enable_1_gen = true;
				static bool enable_2_gen = false;
				static bool enable_3_gen = false;
				static bool enable_alt_gen = false;

				if (enable_alt_gen)
				{
					enable_1_gen = false;
					enable_2_gen = false;
					enable_3_gen = false;
 				}

				ImGui::PushStyleColor(ImGuiCol_Button, FLOAT4(255, 0, 0, 255));
				ImGui::PushStyleColor(ImGuiCol_ButtonHovered, FLOAT4(255, 0, 0, 255));
				ImGui::PushStyleColor(ImGuiCol_ButtonActive, FLOAT4(255, 0, 0, 255));
				ImGui::Button(" ", { 20,20 });
				ImGui::SameLine();
				ImGui::Checkbox("Enable firstborn gen", &enable_1_gen);
				ImGui::PopStyleColor(3);

				ImGui::PushStyleColor(ImGuiCol_Button, FLOAT4(0, 255, 0, 255));
				ImGui::PushStyleColor(ImGuiCol_ButtonHovered, FLOAT4(0, 255, 0, 255));
				ImGui::PushStyleColor(ImGuiCol_ButtonActive, FLOAT4(0, 255, 0, 255));
				ImGui::Button(" ", { 20,20 });
				ImGui::SameLine();
				ImGui::Checkbox("Enable 2 gen", &enable_2_gen);
				ImGui::PopStyleColor(3);

				ImGui::PushStyleColor(ImGuiCol_Button, FLOAT4(0, 0, 255, 255));
				ImGui::PushStyleColor(ImGuiCol_ButtonHovered, FLOAT4(0, 0, 255, 255));
				ImGui::PushStyleColor(ImGuiCol_ButtonActive, FLOAT4(0, 0, 255, 255));
				ImGui::Button(" ", { 20,20 });
				ImGui::SameLine();
				ImGui::Checkbox("Enable 3 gen", &enable_3_gen);
				ImGui::PopStyleColor(3);

				ImGui::Checkbox("Enable alt gen", &enable_alt_gen);



				auto _imgui_colored_button = [](int& val, int id1, int id2)
				{
					std::string term;

					if (val == -1)
					{
						ImGui::PushStyleColor(ImGuiCol_Button, FLOAT4(255, 0, 0, 200));
						ImGui::PushStyleColor(ImGuiCol_ButtonHovered, FLOAT4(255, 0, 0, 230));
						ImGui::PushStyleColor(ImGuiCol_ButtonActive, FLOAT4(255, 0, 0, 255));
						term = "-1";
					}
					if (val == 0)
					{
						ImGui::PushStyleColor(ImGuiCol_Button, FLOAT4(255, 255, 255, 200));
						ImGui::PushStyleColor(ImGuiCol_ButtonHovered, FLOAT4(255, 255, 255, 230));
						ImGui::PushStyleColor(ImGuiCol_ButtonActive, FLOAT4(255, 255, 255, 255));
						term = "0";
					}
					if (val == +1)
					{
						ImGui::PushStyleColor(ImGuiCol_Button, FLOAT4(0, 255, 0, 200));
						ImGui::PushStyleColor(ImGuiCol_ButtonHovered, FLOAT4(0, 255, 0, 230));
						ImGui::PushStyleColor(ImGuiCol_ButtonActive, FLOAT4(0, 255, 0, 255));
						term = "+1";
					}


					bool press = ImGui::Button(str::format::insert("{}##{}{}", term, id1, id2).c_str(), { 30,30 });
					ImGui::PopStyleColor(3);

					if (press && val == -1) { val = +0; press = false; }
					if (press && val == +0) { val = +1; press = false; }
					if (press && val == +1) { val = -1; press = false; }
				};

				ImGui::Dummy({ 10,10 }); ImGui::Separator(); ImGui::Dummy({ 10,10 });



				static float arr[] = { +1,+1,+1,+1,+0,+1,+1,+1,+1 };
				static float bir[3] = { 3, 2, 4 };

				//=====================================================================// Gen Data
				ImGui::Text("Gen Data:");

				ImGui::PushItemWidth(20*2*2*2);//--------PushItemWidth
 				ForArray(i, 3)
				{
					ForArray(j, 3)
					{

						ImGui::InputFloat(str::format::insert("##gd_f{}{}", i, j).c_str(), &arr[i * 3 + j], 1, 1);

						ImGui::SameLine();
					}
					ImGui::Dummy({ 0,0 });
				}
				ImGui::PopItemWidth();//------------PopItemWidth

 				ImGui::PushItemWidth(160);//--------PushItemWidth
				ImGui::InputFloat("Birth",    &bir[0], 1, 1);
				ImGui::InputFloat("Live min", &bir[1], 1, 1);
				ImGui::InputFloat("Dead min", &bir[2], 1, 1);
				ImGui::PopItemWidth();//------------PopItemWidth


				if(ImGui::Button("randomize"))
				{
					ForArray(i, 9)
						arr[i] = math::RandomFastReal(-2, 2);

					bir[0] = math::RandomFastReal(0, 9);
					bir[1] = math::RandomFastReal(0, 9);
					bir[2] = math::RandomFastReal(0, 9);

					{
						size_w = size_w_setter;
						size_h = size_h_setter;

						ram_world = bitmap32(size_w, size_h, { 0,0,0,255 });

						ForArray(h, ram_world.size().y - ram_world.size().y * 0.3)
						ForArray(w, ram_world.size().x - ram_world.size().x * 0.3)
						{
							ram_world.raw_data(
								w + ram_world.size().x * 0.15,
								h + ram_world.size().y * 0.15)->r = math::rand64() % 2 == 0 ? 255 : 0;
						}

						stat_pass_round = 0;
					}
				}

				////////////////////////////////////////////////////////////////////////////////////////////////// save_load
				ImGui::Separator();
				ImGui::Text("Data");

				struct save_dat
				{
					std::string name;
					float buffer[sizeof(float) * (9 + 3)];
				};

				static vector<save_dat> saves;
				static bool saves_need_update = true;


				if (ImGui::Button("save_gen"))
				{
					float buffer[sizeof(float) * (9 + 3)];

					ForArray(i, 9)
						buffer[i] = arr[i];

					buffer[9 + 0] = bir[0];
					buffer[9 + 1] = bir[1];
					buffer[9 + 2] = bir[2];

					std::string file = str::format::insert("lv_saves/{}", math::Random(8000, 90000));

 					write_file_raw(file, (const char*)&buffer[0], sizeof(buffer), file_write_mode::Rewrite);

					saves_need_update = true;
				}
 				if (saves_need_update)
				{
					saves_need_update = false;
					saves.clear();

 					std::string path = "lv_saves/";
					for (const auto & p : fs::directory_iterator(path))
					{
 						auto read_result = read_file_raw(p.path());

						if (read_result.IsSucceed())
						{
							file_buffer file_dat = read_result.GetValue();

							save_dat sv_dat;

							sv_dat.name = p.path().string();
							memcpy(&sv_dat.buffer, file_dat.data, sizeof(float) * 12);

							saves.push_back(sv_dat);

							file_dat.release();
 						}
 					}
				}

				static int save_deleter_it = -1;

				ForArray(i, saves.size())
				{
					if (ImGui::Button(saves[i].name.c_str()))
					{
						ForArray(j, 9)
							arr[j] = saves[i].buffer[j];

						bir[0] = saves[i].buffer[9 + 0];
						bir[1] = saves[i].buffer[9 + 1];
						bir[2] = saves[i].buffer[9 + 2];
 					}

					ImGui::SameLine();
					if (ImGui::Button(("[del]##" + saves[i].name).c_str()))
						save_deleter_it = i;
 				}

				if (save_deleter_it != -1)
				{
  					std::cout <<  fs::remove(saves[save_deleter_it].name);
					saves_need_update = true;

					save_deleter_it = -1;
				}

 				//=====================================================================// _update_frame

				if (_update_frame)
				{
					_time_escaped = 0;//fmod(_time_escaped, cells_fps_target);
					_update_frame = false;

					if (temp_buff.size() != ram_world.size()) // need resize
					{
						temp_buff = ram_world.get_copy();

						SuperTimer ST;
						ST.SetStart();
						gpu_texture.Update(ram_world);
						ST.SetEnd();

						log::info << str::format::insert("gl reinit texture:[{}-{}] {}",
							ram_world.size().x,
							ram_world.size().y,
							ST.GetTimeReal());
					}
					////////////////////////////////////////////////////////////
					stat_pass_round++;



					struct gen_data
					{
						float mat_sign[9] =
						{
							+1,+1,+1,
							+1,+0,+1,
							+1,+1,+1,
						};

						float birn_val = 3;
						float live_min = 2;
						float dead_min = 4;
					};

					static gen_data normal_gen = {};
					static gen_data edit_gen = {};

					//
					ForArray(i, 9)
						edit_gen.mat_sign[i] = arr[i];

					edit_gen.birn_val = bir[0];
					edit_gen.live_min = bir[1];
					edit_gen.dead_min = bir[2];



					///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

					#define pos_macro(a, b) ((a < 0) ? (b-1) : (a > b - 1) ? (0) : a) //todo -200 if 100
   					temp_buff.fill_by_color({ 0,0,0,255 });

					ForArray(W, size_w)
					ForArray(H, size_h)
					{
 						if (true)
						{
							float i = 0;
							int w = W;
							int h = H;

							//if (w == 0 || h == 0 || w >= (size_w - 1) || h >= (size_h - 1))
							{
								const auto aa1 = ram_world.raw_data(pos_macro(w - 1, size_w), pos_macro(h - 1, size_h));
								const auto aa2 = ram_world.raw_data(pos_macro(w + 0, size_w), pos_macro(h - 1, size_h));
								const auto aa3 = ram_world.raw_data(pos_macro(w + 1, size_w), pos_macro(h - 1, size_h));
								const auto aa4 = ram_world.raw_data(pos_macro(w - 1, size_w), pos_macro(h + 0, size_h));
								const auto aa5 = ram_world.raw_data(pos_macro(w + 0, size_w), pos_macro(h + 0, size_h));
								const auto aa6 = ram_world.raw_data(pos_macro(w + 1, size_w), pos_macro(h + 0, size_h));
								const auto aa7 = ram_world.raw_data(pos_macro(w - 1, size_w), pos_macro(h + 1, size_h));
								const auto aa8 = ram_world.raw_data(pos_macro(w + 0, size_w), pos_macro(h + 1, size_h));
								const auto aa9 = ram_world.raw_data(pos_macro(w + 1, size_w), pos_macro(h + 1, size_h));


								if (aa1->r > 0 || aa1->g > 0 || aa1->b > 0) i += 1.0001 * edit_gen.mat_sign[0];// else i -= 1.0001 * alt_gen.mat_sign[0];
								if (aa2->r > 0 || aa2->g > 0 || aa2->b > 0) i += 1.0001 * edit_gen.mat_sign[1];// else i -= 1.0001 * alt_gen.mat_sign[1];
								if (aa3->r > 0 || aa3->g > 0 || aa3->b > 0) i += 1.0001 * edit_gen.mat_sign[2];// else i -= 1.0001 * alt_gen.mat_sign[2];
								if (aa4->r > 0 || aa4->g > 0 || aa4->b > 0) i += 1.0001 * edit_gen.mat_sign[3];// else i -= 1.0001 * alt_gen.mat_sign[3];
								if (aa5->r > 0 || aa5->g > 0 || aa5->b > 0) i += 1.0001 * edit_gen.mat_sign[4];// else i -= 1.0001 * alt_gen.mat_sign[4];
								if (aa6->r > 0 || aa6->g > 0 || aa6->b > 0) i += 1.0001 * edit_gen.mat_sign[5];// else i -= 1.0001 * alt_gen.mat_sign[5];
								if (aa7->r > 0 || aa7->g > 0 || aa7->b > 0) i += 1.0001 * edit_gen.mat_sign[6];// else i -= 1.0001 * alt_gen.mat_sign[6];
								if (aa8->r > 0 || aa8->g > 0 || aa8->b > 0) i += 1.0001 * edit_gen.mat_sign[7];// else i -= 1.0001 * alt_gen.mat_sign[7];
								if (aa9->r > 0 || aa9->g > 0 || aa9->b > 0) i += 1.0001 * edit_gen.mat_sign[8];// else i -= 1.0001 * alt_gen.mat_sign[8];

 							}

							auto in_p  = ram_world.raw_data(w, h);
							auto out_p = temp_buff.raw_data(w, h);

							i = i;

 							if (in_p->r == 0)
							{
 								if (std::abs(i - edit_gen.birn_val) < 0.99) out_p->r = 255;
							}
							else
							{
								out_p->r = math::max(0., in_p->r - math::RandomFastReal(0,5));

  								if (i < edit_gen.live_min) out_p->r = 0;
								if (i >= edit_gen.dead_min) out_p->r = 0;
							}
 						}
  					}

 					ram_world = temp_buff;
				}
				////////////////////////////////////////////////////////////////////////////////////////////draw

				if (lenear_sampler)
					0;// pu_texture.BindSampler(sampler_lenear);
				else
					0;// gpu_texture.BindSampler(sampler_point);

				gpu_texture.Update(ram_world);

			}
			ImGui::EndGroup();

		}
		ImGui::End();

	}

	static void DrawWindowLVS(std::string name, bool* opened_ptr)
	{
		if (ImGui::Begin(name.c_str(), opened_ptr, 0))
		{
			enum { SIZE_W = 2560 };
			enum { SIZE_H = 2560 };



			static bitmap32 ram_world(SIZE_W, SIZE_H, color32{ (uint8_t)120,(uint8_t)0,(uint8_t)0,(uint8_t)255 });
			static bitmap32 temp_buff(SIZE_W, SIZE_H, color32{ (uint8_t)120,(uint8_t)0,(uint8_t)0,(uint8_t)255 });
			static GpuTexture gpu_texture;

			static GpuSampler sampler_point;
			static GpuSampler sampler_lenear;

			MakeOnce
			{
				bitmap32 filler(64, 64, color32{
				(uint8_t)190,
				(uint8_t)190,
				(uint8_t)190,
				(uint8_t)255 });

				sampler_point.CreateSampler(SamplerState::GetPresetPoint());
				sampler_lenear.CreateSampler(SamplerState::GetPresetLinear());

				gpu_texture.Update(filler);
			}

			static SuperTimer ST;


			ImGuiIO&		io = ImGui::GetIO();
			ImGuiContext*	context = ImGui::GetCurrentContext();
			ImGuiWindow*	window = context->CurrentWindow;
			ImDrawList*		drawer = window->DrawList;


			ImItem::ShowImage(&gpu_texture, true, { 1200, 1200 });

			//---------------------------------------------------------------------// gui calls
			ImGui::SameLine();
			ImGui::BeginGroup();
			{
				auto pos  = ImGui::GetCursorPos() + ImGui::GetWindowPos();
				auto size = ImGui::GetContentRegionAvail();

				//if (pos.x < pos.x + size.x && pos.y < pos.y + size.y)
				//	drawer->AddRect(pos - FLOAT2{ 2,2 }, pos + size, 0xFF000020);

				//////////////////////////////////////////////////////////////////////////

  				static int   cells_size_w     = SIZE_W;
				static int   cells_size_h     = SIZE_H;
				static float cells_fps_target = 30;

				static int   cells_pass_round      = 0;//dummy
				static int   cells_alive_shells    = 0;
				static int   cells_generation_seed = math::Random(0, 100000000);//dummy

				static bool  lenear_sampler = false;

				static SuperTimer ST;

				static int	 _size_w_setter = cells_size_w;
				static int	 _size_h_setter = cells_size_h;
				static float _cells_fps_target_setter = cells_fps_target;
				static float _time_escaped = 0;
				static bool _auto_timed_update = false;
				static bool _need_update_frame = false;


				// dock gui

				ImGui::PushItemWidth(160);//------------------------------------------PushItemWidth

				ImGui::Text("Gene seed:");	ImGui::InputInt("##0", &cells_generation_seed, 0, 0);
				ImGui::Text("Pass round:");	ImGui::InputInt("##1", &cells_pass_round, 0, 0);
				ImGui::Text("Live cells:");	ImGui::InputInt("##2", &cells_alive_shells, 0, 0);
				ImGui::Text("Fps target:");	ImGui::InputFloat("##3", &_cells_fps_target_setter, 0, 0);

				ImGui::SameLine();
				if (ImGui::SmallButton("set"))
					cells_fps_target = _cells_fps_target_setter;

				ImGui::Checkbox("Lenear filter", &lenear_sampler);

				ImGui::Separator();//-------------------------------------separator

				ImGui::InputInt("MapSizeW", &_size_w_setter, 1, 10);
				ImGui::InputInt("MapSizeH", &_size_h_setter, 1, 10);

				ImGui::PopItemWidth();//----------------------------------------------PopItemWidth


				//=====================================================================// data editors

				bool _clean  = ImGui::Button("Clear");
				bool _resize = ImGui::Button("Resize & Randomize");
				bool _get_cb = ImGui::Button("Get from copyboard");

 				ImGui::SameLine();

				bool img1 = ImGui::Button("get.1"); ImGui::SameLine();
				bool img2 = ImGui::Button("get.2"); ImGui::SameLine();
				bool img3 = ImGui::Button("get.3");

				if (img1 || img2 || img3)
				{
					std::string path;

					if (img1) path = "assets/aaa.png";
					if (img2) path = "assets/bbb.png";
					if (img3) path = "assets/ccc.png";

					auto copypaste = png_read_image_file(path);

					cells_size_w = _size_w_setter = copypaste.size().x;
					cells_size_h = _size_h_setter = copypaste.size().y;

					ram_world = copypaste;

					cells_pass_round = 0;
				}

				if (_clean)
				{
					ram_world.fill_by_color(color32{ 0,0,0,255 });
					cells_pass_round = 0;
				}

				if (_resize)
				{
					cells_size_w = _size_w_setter;
					cells_size_h = _size_h_setter;

					ram_world = bitmap32(cells_size_w, cells_size_h, { 0,0,0,255 });


					ForArray(h, ram_world.size().y - ram_world.size().y * 0.3)
					ForArray(w, ram_world.size().x - ram_world.size().x * 0.3)
					{
						ram_world.raw_data(
							w + ram_world.size().x * 0.15,
							h + ram_world.size().y * 0.15)->r = math::rand64() % 2 == 0 ? 255 : 0;
					}
					cells_pass_round = 0;
				}

				if (_get_cb)
				{
					//auto result = os::io::clipboard::GetClipboardImageRGBA();
					//if (result.IsSucceed())
					//{
					//	auto copypaste = result.GetValue();
					//
					//	cells_size_w = _size_w_setter = copypaste.w;
					//	cells_size_h = _size_h_setter = copypaste.h;
					//
					//	ram_world.copy_from_ptr(copypaste.w, copypaste.h, copypaste.data);
					//
					//	bitmap32 convert(cells_size_w, cells_size_h);
					//
					//	ForArray(w, ram_world.w)
					//	ForArray(h, ram_world.h)
					//	{
					//		convert.raw_data(w, h)->b = ram_world.raw_data(w, h)->r;
					//		convert.raw_data(w, h)->g = ram_world.raw_data(w, h)->g;
					//		convert.raw_data(w, h)->r = ram_world.raw_data(w, h)->b;
					//		convert.raw_data(w, h)->a = ram_world.raw_data(w, h)->a;
					//	}
					//	ram_world = convert;
					//
					//	cells_pass_round = 0;
					//}
				}

 				//=====================================================================// update

				if (_auto_timed_update)
				{
					_time_escaped += ST.GetRoundTime();

					if (_time_escaped > (1.0 / cells_fps_target))
					{
						_need_update_frame = true;
					}
				}

				if (ImGui::Button("Update")) _need_update_frame = true;
				ImGui::SameLine();
				ImGui::Checkbox("AutoUpdate", &_auto_timed_update);

				//=====================================================================// gens
				ImGui::Dummy({ 10,10 }); ImGui::Separator(); ImGui::Dummy({ 10,10 });
				ImGui::Text("Gen switchers:");

				if (false && " gens")
				{
 					static bool enable_1_gen = true;
					static bool enable_2_gen = false;
					static bool enable_3_gen = false;
					static bool enable_alt_gen = false;

					if (enable_alt_gen)
					{
						enable_1_gen = false;
						enable_2_gen = false;
						enable_3_gen = false;
					}

					ImGui::PushStyleColor(ImGuiCol_Button, FLOAT4(255, 0, 0, 255));
					ImGui::PushStyleColor(ImGuiCol_ButtonHovered, FLOAT4(255, 0, 0, 255));
					ImGui::PushStyleColor(ImGuiCol_ButtonActive, FLOAT4(255, 0, 0, 255));
					ImGui::Button(" ", { 20,20 });
					ImGui::SameLine();
					ImGui::Checkbox("Enable firstborn gen", &enable_1_gen);
					ImGui::PopStyleColor(3);

					ImGui::PushStyleColor(ImGuiCol_Button, FLOAT4(0, 255, 0, 255));
					ImGui::PushStyleColor(ImGuiCol_ButtonHovered, FLOAT4(0, 255, 0, 255));
					ImGui::PushStyleColor(ImGuiCol_ButtonActive, FLOAT4(0, 255, 0, 255));
					ImGui::Button(" ", { 20,20 });
					ImGui::SameLine();
					ImGui::Checkbox("Enable 2 gen", &enable_2_gen);
					ImGui::PopStyleColor(3);

					ImGui::PushStyleColor(ImGuiCol_Button, FLOAT4(0, 0, 255, 255));
					ImGui::PushStyleColor(ImGuiCol_ButtonHovered, FLOAT4(0, 0, 255, 255));
					ImGui::PushStyleColor(ImGuiCol_ButtonActive, FLOAT4(0, 0, 255, 255));
					ImGui::Button(" ", { 20,20 });
					ImGui::SameLine();
					ImGui::Checkbox("Enable 3 gen", &enable_3_gen);
					ImGui::PopStyleColor(3);

					ImGui::Checkbox("Enable alt gen", &enable_alt_gen);

					auto _imgui_colored_button = [](int& val, int id1, int id2)
					{
						std::string term;

						if (val == -1)
						{
							ImGui::PushStyleColor(ImGuiCol_Button, FLOAT4(255, 0, 0, 200));
							ImGui::PushStyleColor(ImGuiCol_ButtonHovered, FLOAT4(255, 0, 0, 230));
							ImGui::PushStyleColor(ImGuiCol_ButtonActive, FLOAT4(255, 0, 0, 255));
							term = "-1";
						}
						if (val == 0)
						{
							ImGui::PushStyleColor(ImGuiCol_Button, FLOAT4(255, 255, 255, 200));
							ImGui::PushStyleColor(ImGuiCol_ButtonHovered, FLOAT4(255, 255, 255, 230));
							ImGui::PushStyleColor(ImGuiCol_ButtonActive, FLOAT4(255, 255, 255, 255));
							term = "0";
						}
						if (val == +1)
						{
							ImGui::PushStyleColor(ImGuiCol_Button, FLOAT4(0, 255, 0, 200));
							ImGui::PushStyleColor(ImGuiCol_ButtonHovered, FLOAT4(0, 255, 0, 230));
							ImGui::PushStyleColor(ImGuiCol_ButtonActive, FLOAT4(0, 255, 0, 255));
							term = "+1";
						}


						bool press = ImGui::Button(str::format::insert("{}##{}{}", term, id1, id2).c_str(), { 30,30 });
						ImGui::PopStyleColor(3);

						if (press && val == -1) { val = +0; press = false; }
						if (press && val == +0) { val = +1; press = false; }
						if (press && val == +1) { val = -1; press = false; }
					};

					ImGui::Dummy({ 10,10 }); ImGui::Separator(); ImGui::Dummy({ 10,10 });
				}


				static float arr[] = { +1,+1,+1,+1,+0,+1,+1,+1,+1 };
				static float bir[3] = { 3, 2, 4 };

				//=====================================================================// Gen Data
				ImGui::Text("Gen Data:");

				if (false && " gens")
				{
					ImGui::PushItemWidth(20 * 2 * 2 * 2);//--------PushItemWidth
					ForArray(i, 3)
					{
						ForArray(j, 3)
						{

							ImGui::InputFloat(str::format::insert("##gd_f{}{}", i, j).c_str(), &arr[i * 3 + j], 1, 1);

							ImGui::SameLine();
						}
						ImGui::Dummy({ 0,0 });
					}
					ImGui::PopItemWidth();//------------PopItemWidth

					ImGui::PushItemWidth(160);//--------PushItemWidth
					ImGui::InputFloat("Birth", &bir[0], 1, 1);
					ImGui::InputFloat("Live min", &bir[1], 1, 1);
					ImGui::InputFloat("Dead min", &bir[2], 1, 1);
					ImGui::PopItemWidth();//------------PopItemWidth

 					if (ImGui::Button("randomize"))
				{
					ForArray(i, 9)
						arr[i] = math::RandomFastReal(-2, 2);

					bir[0] = math::RandomFastReal(0, 9);
					bir[1] = math::RandomFastReal(0, 9);
					bir[2] = math::RandomFastReal(0, 9);

					{
						cells_size_w = _size_w_setter;
						cells_size_h = _size_h_setter;

						ram_world = bitmap32(cells_size_w, cells_size_h, { 0,0,0,255 });

						ForArray(h, ram_world.size().y - ram_world.size().y * 0.3)
						ForArray(w, ram_world.size().x - ram_world.size().x * 0.3)
						{
							ram_world.raw_data(
								w + ram_world.size().x * 0.15,
								h + ram_world.size().y * 0.15)->r = math::rand64() % 2 == 0 ? 255 : 0;
						}

						cells_pass_round = 0;
					}
				}
				}


				//=====================================================================// _update_frame
				static int selected_rule = 0;
				static int sel_random_updater = 0;


				if (ImGui::Button("rule = [ 5, 34, 45, 34, 58 ]")) selected_rule = 0; if (selected_rule == 0) { ImGui::SameLine(); ImGui::Button("[+]"); };
				if (ImGui::Button("rule = [ 5, 9,  9,  9,  9  ]")) selected_rule = 1; if (selected_rule == 1) { ImGui::SameLine(); ImGui::Button("[+]"); };
				if (ImGui::Button("rule = [ 5, 9,  9,  9,  17 ]")) selected_rule = 2; if (selected_rule == 2) { ImGui::SameLine(); ImGui::Button("[+]"); };
				if (ImGui::Button("rule = [ 5, 34, 47, 34, 60 ]")) selected_rule = 3; if (selected_rule == 3) { ImGui::SameLine(); ImGui::Button("[+]"); };
				if (ImGui::Button("rule = [ 5, 34, 41, 34, 58 ]")) selected_rule = 4; if (selected_rule == 4) { ImGui::SameLine(); ImGui::Button("[+]"); };

				if (ImGui::Button("rule = [ random.generator ]"))  selected_rule = 5; if (selected_rule == 5) { ImGui::SameLine(); ImGui::Button("[+]"); };

 				if (ImGui::Button("UPD"))
					sel_random_updater = 1;



				if (_need_update_frame)
				{
					_time_escaped = 0;
					_need_update_frame = false;

					if (temp_buff.size() != ram_world.size()) // need resize
					{
						temp_buff = ram_world.get_copy();

						SuperTimer ST;
						ST.SetStart();
						gpu_texture.Update(ram_world);
						ST.SetEnd();

						log::info << str::format::insert("gl reinit texture:[{}-{}] {}",
							ram_world.size().x,
							ram_world.size().y,
							ST.GetTimeReal());
					}
					////////////////////////////////////////////////////////////
					cells_pass_round++;


					///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

					temp_buff.fill_by_color({ 0,0,0,255 });

					struct LvLrule
					{
						int r;
						int b_left;
						int b_right;
						int d_left;
						int d_right;
					};

					static LvLrule* rules_arr = new LvLrule[64];

 					rules_arr[0] = { 5,  34, 45, 34, 58   };
					rules_arr[1] = { 5,  9,  9,  9,  9    };
					rules_arr[2] = { 5,  9,  9,  9,  17   };
					rules_arr[3] = { 5,  34, 47, 34, 60   };
					rules_arr[4] = { 5,  34, 41, 34, 58   };


					if (sel_random_updater == 1)
					{
						sel_random_updater = 0;

						rules_arr[5] =
						{
							 (int)math::RandomInt64(1,10),
							 (int)math::RandomInt64(-200,200),
							 (int)math::RandomInt64(-200,200),
							 (int)math::RandomInt64(-200,200),
							 (int)math::RandomInt64(-200,200),
						};

						cells_size_w = _size_w_setter;
						cells_size_h = _size_h_setter;

						ram_world = bitmap32(cells_size_w, cells_size_h, { 0,0,0,255 });

						ForArray(h, ram_world.size().y - ram_world.size().y * 0.3)
						ForArray(w, ram_world.size().x - ram_world.size().x * 0.3)
						{
							ram_world.raw_data(
								w + ram_world.size().x * 0.15,
								h + ram_world.size().y * 0.15)->r = math::rand64() % 2 == 0 ? 255 : 0;
						}

						cells_pass_round = 0;
					}



 					LvLrule used_rule = rules_arr[selected_rule];


					auto lam = [&used_rule](color32& p, const INT2& pos)
					{
						int r = used_rule.r;

						int hor_pos = pos.x - r;
						int ver_pos = pos.y - r;
						int size_rr = r + r + 1;

						int i = 0;

						ForArrayFrom(w, ver_pos, ver_pos + size_rr)
						ForArrayFrom(h, hor_pos, hor_pos + size_rr)
						{
							const int realX = distor(w, 0, cells_size_w);
							const int realY = distor(h, 0, cells_size_h);

							const auto aa1 = ram_world.raw_data(realX, realY);

							if (aa1->r > 0)// || aa1->g > 0 || aa1->b > 0)
								i++;// i += 1.0001 * edit_gen.mat_sign[0];// else i -= 1.0001 * alt_gen.mat_sign[0];
						}

						int x = pos.x; // distor(pos.x, 0, cells_size_w);
						int y = pos.y; // distor(pos.y, 0, cells_size_h);

						const auto& minus = ram_world.raw_data(x, y);
						const bool this_alive = minus->r > 0 || minus->g > 0 || minus->b > 0;

						if (this_alive)
							i--;
						/////////////////////////////////////////////////////////

						auto in_p  = ram_world.raw_data(pos.x, pos.y);
						auto out_p = temp_buff.raw_data(pos.x, pos.y);


						if (in_p->r > 0)
							*out_p = { 255, 255, 255, 255 };
						else
							*out_p = { 0, 0, 0, 0 };

						if (this_alive)
						{
							if (i < used_rule.d_left)  *out_p = { 0, 0, 0, 0 };
							if (i > used_rule.d_right) *out_p = { 0, 0, 0, 0 };
						}
						else
						{
							if (i >= used_rule.b_left && i <= used_rule.b_right) *out_p = { 255, 255, 255, 255 };
						}
					};

					//temp_buff.for_each(lam, 6);

					{
						auto& map = temp_buff;
						ForArray(y, map.size().y)
						ForArray(x, map.size().x)
						{
							const INT2 pos = { x,y };
							const int r_num = used_rule.r;
							const int hor_pos = pos.x - r_num;
							const int ver_pos = pos.y - r_num;
							const int size_rr = r_num + r_num + 1;

							int i = 0;

							ForArrayFrom(w, ver_pos, ver_pos + size_rr)
							ForArrayFrom(h, hor_pos, hor_pos + size_rr)
							{
								const int realX = distor(w, 0, cells_size_w);
								const int realY = distor(h, 0, cells_size_h);
								const auto mid = ram_world.raw_data(realX, realY);

								if (mid->r > 0)// || aa1->g > 0 || aa1->b > 0)
									i++;// i += 1.0001 * edit_gen.mat_sign[0];// else i -= 1.0001 * alt_gen.mat_sign[0];
							}

							const auto& it = ram_world.raw_data(x, y);
							const bool this_alive = it->r > 0 || it->g > 0 || it->b > 0;

							if (this_alive)
								i--;
							/////////////////////////////////////////////////////////

							auto in_p = ram_world.raw_data(pos.x, pos.y);
							auto out_p = temp_buff.raw_data(pos.x, pos.y);

							if (in_p->r > 0)
								*out_p = { 255, 255, 255, 255 };
							else
								*out_p = { 0, 0, 0, 0 };

							if (this_alive)
							{
								if (i < used_rule.d_left)  *out_p = { 0, 0, 0, 0 };
								if (i > used_rule.d_right) *out_p = { 0, 0, 0, 0 };
							}
							else
							{
								if (i >= used_rule.b_left && i <= used_rule.b_right) *out_p = { 255, 255, 255, 255 };
							}
						}
					}


 					ram_world = std::move(temp_buff);
				}
				////////////////////////////////////////////////////////////////////////////////////////////draw

				if (lenear_sampler)
				{
					//LOG_WARN_NO_IMPL;
					gpu_texture.SetSampler(SamplerState::GetPresetLinear());
				}
				else
				{
					//LOG_WARN_NO_IMPL;
					gpu_texture.SetSampler(SamplerState::GetPresetPoint());
				}

				gpu_texture.Update(ram_world);

			}
			ImGui::EndGroup();

		}
		ImGui::End();

	}



}


namespace imview
{
	static void view_live() { DrawWindowLV("live",nullptr); }
	static void view_live2() { DrawWindowLVS("live2", nullptr); }
}