#pragma once

#include "../shared_in.h"

#include "imgui.impl.h"
#include "imgui.tools.h"

#include "../matrix/glm.matrix.h"



//c++14
//#include <experimental/filesystem>
//namespace fs = std::experimental::filesystem;
//c++17
//#include <filesystem>
//namespace fs = std::filesystem;

namespace
{
	inline void ImShowWindowState(const os::Window* WS)
	{
		ImItem::Text(str::format::insert("GetX()               | {}", (int  )WS->GetX()));
		ImItem::Text(str::format::insert("GetY()               | {}", (int  )WS->GetY()));
		ImItem::Text(str::format::insert("GetWidth()           | {}", (int  )WS->GetWidth()));
		ImItem::Text(str::format::insert("GetHeight()          | {}", (int  )WS->GetHeight()));
		ImItem::Text(str::format::insert("IsActive()           | {}", (bool )WS->IsActive()));
		ImItem::Text(str::format::insert("IsExit()             | {}", (bool )WS->IsExit()));
		ImItem::Text(str::format::insert("IsMaximized()        | {}", (bool )WS->IsMaximized()));
		ImItem::Text(str::format::insert("IsMinimized()        | {}", (bool )WS->IsMinimized()));
		ImItem::Text(str::format::insert("WS.Title             | {}", (char*)WS->GetWS()->Title.c_str()));
		ImItem::Text(str::format::insert("WS.stile_borders     | {}", (int  )WS->GetWS()->stile_borders));
		ImItem::Text(str::format::insert("WS.stile_alpha       | {}", (int  )WS->GetWS()->stile_alpha));
		ImItem::Text(str::format::insert("WS.stile_transperent | {}", (int  )WS->GetWS()->stile_transperent));
		ImItem::Text(str::format::insert("WS.SpawnPos          | {}", (int  )WS->GetWS()->stile_spawn));
		ImItem::Text(str::format::insert("WS.X                 | {}", (int  )WS->GetWS()->X));
		ImItem::Text(str::format::insert("WS.Y                 | {}", (int  )WS->GetWS()->Y));
		ImItem::Text(str::format::insert("WS.Wight             | {}", (int  )WS->GetWS()->Wight));
		ImItem::Text(str::format::insert("WS.Height            | {}", (int  )WS->GetWS()->Height));
		ImItem::Text(str::format::insert("WS.Resizble          | {}", (bool )WS->GetWS()->Resizble));
		ImItem::Text(str::format::insert("WS.Visable           | {}", (bool )WS->GetWS()->Visable));
		ImItem::Text(str::format::insert("WS.IsTopMost         | {}", (bool )WS->GetWS()->TopMost));
	}

	inline void ImShowRenderState(const graphics::RenderState* RS)
	{
		ImItem::Text(str::format::insert("mRS.FullScreen          | {}", (bool)RS->FullScreen));
		ImItem::Text(str::format::insert("mRS.vSunc               | {}", (bool)RS->VSunc));
		ImItem::Text(str::format::insert("mRS.SlowOnUnFocuse      | {}", (bool)RS->SlowOnUnFocuse));
		ImItem::Text(str::format::insert("mRS.SlowOnUnFocuse      | {}", (bool)RS->IsFocused));
		ImItem::Text(str::format::insert("mRS.RealCanvasWeight    | {}", (int )RS->RealCanvasW));
		ImItem::Text(str::format::insert("mRS.RealCanvasHight     | {}", (int )RS->RealCanvasH));
		ImItem::Text(str::format::insert("mRS.VirtualCanvasWight  | {}", (int )RS->RenderCanvasW));
		ImItem::Text(str::format::insert("mRS.VirtualCanvasHeight | {}", (int )RS->RenderCanvasH));
		ImItem::Text(str::format::insert("mRS.AAMode              | {}", (int )RS->AAMode));
		ImItem::Text(str::format::insert("mRS.TSMode              | {}", (int )RS->TSMode));
		ImItem::Text(str::format::insert("mRS.MSAA_SamplerCount   | {}", (int )RS->MSAA_SamplerCount));
		ImItem::Text(str::format::insert("mRS.MSAA_QualityLevel   | {}", (int )RS->MSAA_QualityLevel));
		ImItem::Text(str::format::insert("mRS.AnisotropicQuality  | {}", (int )RS->AnisotropicQuality));
	}

}

namespace
{
	using namespace art;
	using namespace art::color;
	using namespace graphics;
}

class BasicRender : public graphics::Drawer, public virtual utka::uclass
{
public:
	BasicRender() : UCLASS("BasicRender")
	{

	}

	struct ImGuiState
	{
		~ImGuiState()
		{
			if (api_impl)
				delete api_impl;
		}

		ImGuiProvider* api_impl = nullptr;
		bool  is_skip_frames    = false;  // for saving cpu time
		bool  is_CPU_frame      = true;   //
		bool  software_raster   = false;  // software rasterizer
		float fps_target        = 144.0;  // updates per second
		const char debug_wnd_name[60] = "_hover_debug_window";
	};

protected:
	RenderState myRS;
	ImGuiState ImguiState;

	bool    ClearColorRainbow = true;
	color4f ClearColor   = { 0,0,0,0 };
	color4f ClearColorGo = { 0,0,0,0 };

	os::Window* my_window = nullptr; // for dubug

	bitmap32* soft_render = nullptr; // mb dell

public:
	bool Init(const RenderOptions& RO)
	{
		std::memcpy(&myRS, &RO, sizeof(RenderState));

		my_window = os::Window::Get();

		return Drawer::impl_init(RO);
	}

	void NotifyFocus(bool Focused)
	{
		myRS.IsFocused = Focused;
		// no impl action
	}

	void NotifyResize(size_t W, size_t H)
	{
		log::debug << str::format::insert("Resized: {} - {}", W, H);

		if (W == 0 || H == 0)
			log::warn << str::format::insert("[x0] resolution setted. Resize skiped.");
		else
		{
			myRS.RenderCanvasW = myRS.RealCanvasW = unsigned(W);
			myRS.RenderCanvasH = myRS.RealCanvasH = unsigned(H);

			Drawer::impl_output_resize(INT2{ W, H });
		}
	}

	//------------------  imgui

	bool ImGuiFrame(bool* ShowDebugWindow, bool* ShowDemo, bool* ShowStat)
	{
		MakeOnce
		{
			IMGUI_CHECKVERSION();
			//-------------------------------// impl init part
			{
				ImplParams impl_params;
				ImguiState.api_impl = new ImGuiProvider();
				ImguiState.api_impl->ImplInit(this, &impl_params);
				//ImGui::CreateContext();
			}

			ImGuiIO& io = ImGui::GetIO();

			io.IniFilename = NULL;
			io.LogFilename = NULL;

			io.Fonts->Clear();
			io.Fonts->AddFontDefault();
			//io.Fonts->AddFontFromFileTTF("assets\\fonts\\DroidSans.ttf", 16.0f, NULL, io.Fonts->GetGlyphRangesCyrillic());

			//ImFontConfig icons_config;
			//icons_config.MergeMode  = true;
			//icons_config.PixelSnapH = true;
			//io.Fonts->AddFontFromFileTTF("assets\\fonts\\icons-Awesome.ttf", param_icon_font_size, &icons_config, range_awesome);
			//io.Fonts->AddFontFromFileTTF("assets\\fonts\\icons-Material.ttf", param_icon_font_size, &icons_config, range_material);
			//io.Fonts->AddFontFromFileTTF("assets\\fonts\\Cousine-Regular.ttf", 15.0f, NULL, io.Fonts->GetGlyphRangesCyrillic());
			//io.Fonts->AddFontFromFileTTF("assets\\fonts\\DroidSans.ttf",       16.0f, NULL, io.Fonts->GetGlyphRangesCyrillic());
			//io.Fonts->AddFontFromFileTTF("assets\\fonts\\Karla-Regular.ttf",   13.0f, NULL, io.Fonts->GetGlyphRangesCyrillic());
			//io.Fonts->AddFontFromFileTTF("assets\\fonts\\ProggyClean.ttf",     15.0f, NULL, io.Fonts->GetGlyphRangesCyrillic());
			//io.Fonts->AddFontFromFileTTF("assets\\fonts\\ProggyTiny.ttf",      12.0f, NULL, io.Fonts->GetGlyphRangesCyrillic());
			//io.Fonts->AddFontFromFileTTF("assets\\fonts\\Roboto-Medium.ttf",   20.0f, NULL, io.Fonts->GetGlyphRangesCyrillic());

			ImguiState.api_impl->ImplUpdateFontTex();
		}

		//-------------------------------------------------- imgui fps set
		if (ImguiState.is_skip_frames)
		{
			static SuperTimer ST;
			ImguiState.is_CPU_frame = ST.InEach(1.0 / ImguiState.fps_target);

			MakeOnce
			{
				ImguiState.is_CPU_frame = true;
			}
		}
		else
			ImguiState.is_CPU_frame = true;

		//-------------------------------------------------- imgui canvas size update
		ImGuiIO& io = ImGui::GetIO();
		IM_ASSERT(io.Fonts->IsBuilt());
		//assert(myRS.RealCanvasW != 0 && myRS.RealCanvasH != 0);

		static SuperTimer im_timer;

		io.DeltaTime     = (float)im_timer.GetRoundTime();
		io.DisplaySize.x = (float)myRS.RenderCanvasW;
		io.DisplaySize.y = (float)myRS.RenderCanvasH;
		io.DisplayFramebufferScale = FLOAT2
		{
			(float)myRS.RealCanvasW / myRS.RenderCanvasW,
			(float)myRS.RealCanvasH / myRS.RenderCanvasH
		};

		//-------------------------------------------------- imgui activity
		if (ImguiState.is_CPU_frame)
		{
			ImguiState.api_impl->ImplNewFrame();

			// -> Call new frame in impl
			// -> ImGui::NewFrame();

			//----------------------------------------------------------------------// Debug
			if (*ShowDebugWindow)
			{
				ImGui::SetNextWindowPos(FLOAT2(16, 16), ImGuiCond_Always);
				ImGui::SetNextWindowBgAlpha(0.8f);

				if (ImGui::Begin(ImguiState.debug_wnd_name, nullptr,
					ImGuiWindowFlags_AlwaysAutoResize
					| ImGuiWindowFlags_NoTitleBar
					| ImGuiWindowFlags_NoScrollbar
					| ImGuiWindowFlags_NoMove
					| ImGuiWindowFlags_NoFocusOnAppearing
					| ImGuiWindowFlags_NoBringToFrontOnFocus
				))
				{
                    ImGui::DragFloat4("gCorrection", &gCorrection.x);
                    ImGui::InputFloat4("gCorrection ", &gCorrection.x);

					//--------------------------------------------------//	Render settings
					ImGui::Text("Render settings");

					if (ImGui::Checkbox("VSunc", &myRS.VSunc))
					{
						impl_change_vsunc(myRS.VSunc);
					}

					ImGui::Checkbox("Rainbow", &ClearColorRainbow);
					ImGui::ColorEdit3("Color", &ClearColor.x);

					if (ClearColorRainbow)
						ImGui::ColorEdit3("ColorGo", &ClearColorGo.x);
					//--------------------------------------------------//	ImGui params
					ImGui::Separator();
					ImGui::Text("ImGui params");
					ImGui::Checkbox("Software render (disabled)", &ImguiState.software_raster);
					ImGui::Checkbox("Save CPU time", &ImguiState.is_skip_frames);

					if (ImguiState.is_skip_frames)
					{
						ImGui::SameLine();
						ImGui::PushItemWidth(90);
						ImGui::SliderFloat("Target ImFps", &ImguiState.fps_target, 20, 300);
						ImGui::PopItemWidth();
					}

					static int color_style_idx = -1;
					if (ImGui::Combo("Color stile", &color_style_idx,
						"Classic\0"
						"Dark\0"
						"Light\0"
						"OrangeMix\0"
						"OrangeLight\0"
						"Orange\0"))
					{
						switch (color_style_idx)
						{
						break; case 0: ImGui::StyleColorsClassic();
						break; case 1: ImGui::StyleColorsDark();
						break; case 2: ImGui::StyleColorsLight();
						break; case 3: ImItem::set_stile_colors_light();
						break; case 4: ImItem::set_stile_colors_lighter();
						break; case 5: ImItem::set_stile_colors_orange();
						}
					}

					ImGui::ShowFontSelector("Font selector");
					//--------------------------------------------------//	Window stile
					ImGui::Separator();
					ImGui::Text("Window stile");

					static int wnd_stile = 0;
					static int wnd_hover = 0;

					if (ImGui::RadioButton("Default##ws",    &wnd_stile, 0)) my_window->SetWindowFrame(WindowBorderStile::DefaultBorders); ImGui::SameLine();
					if (ImGui::RadioButton("Borderless##ws", &wnd_stile, 1)) my_window->SetWindowFrame(WindowBorderStile::Borderless);     ImGui::SameLine();
					if (ImGui::RadioButton("Fullscreen##ws", &wnd_stile, 2)) my_window->SetWindowFrame(WindowBorderStile::BorderlessFullScr);

					if (ImGui::RadioButton("Default##wz", &wnd_hover, 0)) my_window->SetTopMost(false); ImGui::SameLine();
					if (ImGui::RadioButton("TopMost##wz", &wnd_hover, 1)) my_window->SetTopMost(true);
					//-----------------------------------------------------------------------------------//	Launch console
					ImGui::Separator();
					ImGui::Text("Console");

					if (ImGui::Button("Demo"))     *ShowDemo = !*ShowDemo; ImGui::SameLine();
					if (ImGui::Button("ShowStat")) *ShowStat = !*ShowStat;

					//-----------------------------------------------------------------------------------//	Debug info
					ImGui::Separator();
					ImGui::Text("Debug info");

					ImGui::BeginGroup();
					ImGui::Text("SizeCanvas");
					ImGui::Text("SizeRealPT");
					ImGui::Text("SizeWindow");
					ImGui::Text("MouseLocal");
					ImGui::EndGroup();

					ImGui::SameLine();

					ImGui::BeginGroup();
					ImItem::Text("  [{} - {}]", myRS.RenderCanvasW, myRS.RenderCanvasH);
					ImItem::Text("  [{} - {}]", myRS.RealCanvasW,   myRS.RealCanvasH);
					ImItem::Text("  [{} - {}]", my_window->GetWidth(), my_window->GetHeight());
					ImItem::Text("  [{} - {}]", ImGui::GetIO().MousePos.x, ImGui::GetIO().MousePos.y);
					ImGui::EndGroup();

					//ImGui::Text(str::format::insert("&Drawer() {}", (void*)Drawer::getDrawer()).c_str());
					//ImGui::Text(str::format::insert("&RState() {}", (void*)&myRS).c_str());

					//-----------------------------------------------------------------------------------//
					ImGui::Separator();

					const int plots = 360 - 2;

					static float fps_stat[plots];
					static SuperTimer timer;
					while(timer.InEach(0.999/60, plots / 2))
					{
						std::memmove(fps_stat, fps_stat + 1, sizeof(float) * ARRAY_SIZE(fps_stat) - 1);
						fps_stat[ARRAY_SIZE(fps_stat) - 1] = ImGui::GetIO().Framerate;
					}

					const FLOAT2 PLOT_SIZE = FLOAT2(plots, 60) + FLOAT2(7.5, 0);//outline correction, mb todo

					ImItem::Text("Performance histogram");
					ImGui::PlotHistogram("##perf_hist", fps_stat, ARRAY_SIZE(fps_stat), NULL, NULL, 0., FLT_MAX, PLOT_SIZE);

					//------------------------------------------------//
					//float fps_max = 0;
					//float fps_cur = fps_stat[ARRAY_SIZE(fps_stat)-1];
					//ForArray(i, ARRAY_SIZE(fps_stat))
					//{
					//	if (fps_stat[i] > fps_max)
					//		fps_max = fps_stat[i];
					//}
					//
					//ImGui::SameLine();
					//const FLOAT2 curPos = ImGui::GetCursorPos();
					//const std::string strMaxFps = str::format::insert("{0:.2f}", fps_max);
					//const std::string StrCurFps = str::format::insert("{0:.2f}", fps_cur);
					//
					//FLOAT2 tPos = curPos;
					//tPos.x -= ImGui::CalcTextSize((strMaxFps + "---").c_str()).x;
					//ImGui::SetCursorPos(tPos);
					//ImGui::Text(str::format::insert("{0:.2f}", fps_max).c_str());
					//
					//tPos = curPos;
					//tPos.x -= ImGui::CalcTextSize((StrCurFps + "---").c_str()).x;
					//tPos.y += 80 - ImGui::CalcTextSize("").y;
					//ImGui::SetCursorPos(tPos);
					//ImGui::Text(str::format::insert("{0:.2f}", fps_cur).c_str());

					ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
				}
				ImGui::End();
			}
			//----------------------------------------------------------------------// Demo
			if (*ShowDemo)
			{
				ImGui::SetNextWindowPos(FLOAT2(20, 20), ImGuiCond_Once);
				ImGui::ShowDemoWindow(ShowDemo);
			}
			//----------------------------------------------------------------------// Render state
			if (*ShowStat)
			{
				if (ImGui::Begin("Render State", nullptr, 0))
				{
					if (ImGui::CollapsingHeader("Window State"))
					{
						ImShowWindowState(my_window);
					}
					if (ImGui::CollapsingHeader("Render State"))
					{
						ImShowRenderState(&myRS);
					}
					if (ImGui::CollapsingHeader("UNIVERSAL (disabled)"))
					{
						ImGui::Text("disabled");
						//ForArray(i, UNIVERSAL::UMODULESARR.size())
						//{
						//	UMODULE* module = UNIVERSAL::UMODULESARR[i];
						//
						//	ImGui::Button(fmt::format("{} - {} [{}/{}]",
						//		module->u_mod_name.c_str(),
						//		module->u_mod_ver.c_str(),
						//		module->u_mod_file.c_str(),
						//		module->u_mod_line.c_str()).c_str());
						//
						//	if (module->my_refs.size() > 0)
						//	{
						//		//ImGui::Text("refs:");
						//		ForArray(j, module->my_refs.size())
						//		{
						//			UCLASS* cl = module->my_refs[j];
						//
						//			ImGui::Text("");
						//			ImGui::SameLine(30);
						//			ImGui::Button(fmt::format("{} [{}/{}]",
						//				cl->u_c_name.c_str(),
						//				cl->u_c_file.c_str(),
						//				cl->u_c_line.c_str()).c_str());
						//		}
						//	}
						//
						//}
					}
					if (ImGui::CollapsingHeader("Gpu Adapters (disabled)"))
					{
						ImGui::Text("disabled");
#if (false)//---------------------------------------------------------------// disabled
						struct output_test
						{
							void* output_ptr;
							graphics::gpu_output::output_info info;
							std::vector<graphics::gpu_output::output_mode> modes;

						};
						struct adp_test
						{
							void* adp_ptr;
							graphics::gpu_adapter::adapter_info adp_info;
							std::vector<output_test> outputs;
						};

						static std::vector<adp_test> a_array;

						MakeOnce
						{
							std::vector<graphics::gpu_adapter> adp_array = gpu_drawer::enum_gpu_adapters();

							ForArray(i, adp_array.size())
							{
								adp_test test;

								test.adp_ptr = (void*)adp_array[i].GetAdapter();
								test.adp_info = adp_array[i].GetAdapterInfo();

								auto outputs = gpu_drawer::enum_adapter_outputs(&adp_array[i]);

								ForArray(j, outputs.size())
								{
									output_test out;

									out.output_ptr = (void*)outputs[j].GetOutput();
									out.info = outputs[j].GetOutputInfo();
									out.modes = outputs[j].GetOutputModes();

									test.outputs.push_back(out);
								}

								a_array.push_back(test);
							}
						}

						ImGui::Text(fmt::format("Adapters count: ({})", (int)a_array.size()).c_str());
						ForArray(i, a_array.size())
						{
							ImGui::Text(fmt::format("Adapter [{}]:", (int)i).c_str());
							ImGui::Text(fmt::format("impl_ptr           | {}", (void*)a_array[i].adp_ptr).c_str());
							ImGui::Text(fmt::format("description        | {}", (char*)a_array[i].adp_info.description.c_str()).c_str());
							ImGui::Text(fmt::format("vendor_id          | {}", (uint32_t)a_array[i].adp_info.vendor_id).c_str());
							ImGui::Text(fmt::format("device_id          | {}", (uint32_t)a_array[i].adp_info.device_id).c_str());
							ImGui::Text(fmt::format("subsys_id          | {}", (uint32_t)a_array[i].adp_info.subsys_id).c_str());
							ImGui::Text(fmt::format("revision           | {}", (uint32_t)a_array[i].adp_info.revision).c_str());
							ImGui::Text(fmt::format("dedicated_video_mem| {}", (int64_t)a_array[i].adp_info.dedicated_video_mem).c_str());
							ImGui::Text(fmt::format("dedicated_sys_mem  | {}", (int64_t)a_array[i].adp_info.dedicated_sys_mem).c_str());
							ImGui::Text(fmt::format("shared_system_mem  | {}", (int64_t)a_array[i].adp_info.shared_system_mem).c_str());
							ImGui::Text(fmt::format("adapter_luid       | {}", (char*)a_array[i].adp_info.adapter_luid.c_str()).c_str());
							ImGui::Text(fmt::format("outputs count: ({})", (int)a_array[i].outputs.size()).c_str());

							ForArray(j, a_array[i].outputs.size())
							{
								ImGui::Text(fmt::format("   Output [{}]:", (int)j).c_str());
								ImGui::Text(fmt::format("   impl_ptr             | {}", (void*)a_array[i].outputs[j].output_ptr).c_str());
								ImGui::Text(fmt::format("   description          | {}", (char*)a_array[i].outputs[j].info.description.c_str()).c_str());
								ImGui::Text(fmt::format("   os_AttachedToDesktop | {}", (bool)a_array[i].outputs[j].info.os_AttachedToDesktop).c_str());
								ImGui::Text(fmt::format("   os_Coord             | {} {} {} {}", (int)a_array[i].outputs[j].info.os_Coord.left,
									(int)a_array[i].outputs[j].info.os_Coord.top,
									(int)a_array[i].outputs[j].info.os_Coord.right,
									(int)a_array[i].outputs[j].info.os_Coord.bottom).c_str());
								ImGui::Text(fmt::format("   os_Rotation          | {}", (float)a_array[i].outputs[j].info.os_Rotation).c_str());
								ImGui::Text(fmt::format("   os_hMonitor          | {}", (void*)a_array[i].outputs[j].info.os_hMonitor).c_str());
								ImGui::Text(fmt::format("   outputs modes count: ({})", (int)a_array[i].outputs[j].modes.size()).c_str());

								ForArray(q, a_array[i].outputs[j].modes.size())
								{
									ImGui::Text(fmt::format("        output mode [{}] : {}-{}   {} {}",
										(int)q,
										(int)a_array[i].outputs[j].modes[q].Width,
										(int)a_array[i].outputs[j].modes[q].Height,
										(int)a_array[i].outputs[j].modes[q].RefreshNum,
										(int)a_array[i].outputs[j].modes[q].RefreshDen).c_str());
								}
							}
						}
#endif //---------------------------------------------------------------// disabled
					}
					if (ImGui::CollapsingHeader("System Metric"))
					{
						auto resN = os_api::get_system_metric(os::os_sys_metric::VISIBLE_MONITOR_COUNT);
						auto resW = os_api::get_system_metric(os::os_sys_metric::MAIN_MONITOR_W);
						auto resH = os_api::get_system_metric(os::os_sys_metric::MAIN_MONITOR_H);

						ImItem::Text("VISIBLE_MONITOR_COUNT | {}", (int)resN.GetValue());
						ImItem::Text("MAIN_MONITOR_W        | {}", (int)resW.GetValue());
						ImItem::Text("MAIN_MONITOR_H        | {}", (int)resH.GetValue());
					}
					if (ImGui::CollapsingHeader("System Resourses"))
					{
						static bool   update = true;
						static double period = 0.5;

						static SuperTimer ST;
						static double time_escaped = 0;

						time_escaped += ST.GetRoundTime();
						if (time_escaped > period)
						{
							time_escaped = time_escaped - period;
							update = true;
						}
						//////////////////////////////////////////////////////////////////////////
						static double Vfree, Vused, Vall;
						static double Rfree, Rused, Rall;
						static double Pfree, Pused, Pall;
						static double Mload;
						static double CurV;
						static double CurR;

						if (update)
						{
							update = false;

							Vfree = os_api::get_ram_metric(os_ram_metric::MEM_VirtualMemoryAvail       ).GetValue() / 1024. / 1024.;
							Vused = os_api::get_ram_metric(os_ram_metric::MEM_VirtualMemoryLoad        ).GetValue() / 1024. / 1024.;
							Vall  = os_api::get_ram_metric(os_ram_metric::MEM_VirtualMemoryTotal       ).GetValue() / 1024. / 1024.;
							Rfree = os_api::get_ram_metric(os_ram_metric::MEM_RealMemoryAvail          ).GetValue() / 1024. / 1024.;
							Rused = os_api::get_ram_metric(os_ram_metric::MEM_RealMemoryLoad           ).GetValue() / 1024. / 1024.;
							Rall  = os_api::get_ram_metric(os_ram_metric::MEM_RealMemoryTotal          ).GetValue() / 1024. / 1024.;
							Pfree = os_api::get_ram_metric(os_ram_metric::MEM_PageFileAvail            ).GetValue() / 1024. / 1024.;
							Pused = os_api::get_ram_metric(os_ram_metric::MEM_PageFileLoad             ).GetValue() / 1024. / 1024.;
							Pall  = os_api::get_ram_metric(os_ram_metric::MEM_PageFileSize             ).GetValue() / 1024. / 1024.;
							Mload = os_api::get_ram_metric(os_ram_metric::MEM_MemoryLoadProcent        ).GetValue() / 1.; // procent
							CurV  = os_api::get_ram_metric(os_ram_metric::MEM_CurentProcVirtualMemUsage).GetValue() / 1024. / 1024.;
							CurR  = os_api::get_ram_metric(os_ram_metric::MEM_CurentProcRealMemUsage   ).GetValue() / 1024. / 1024.;
                        }

						ImItem::RawText(str::format::insert("       Memory Usage | {:.1f}%%", Mload));
						ImItem::RawText(str::format::insert("CurrentProc Mem V/R | {:.1f} / {:.1f}", CurV, CurR));
						ImItem::RawText(str::format::insert("-----------------------------------------------------"));
                        ImItem::RawText(str::format::insert("                    |   Free   |   Used   |   Total  "));
                        ImItem::RawText(str::format::insert("     Virtual Memory | {:<8.1f} | {:<8.1f} | {:<8.1f} ", Vfree, Vused, Vall));
						ImItem::RawText(str::format::insert("        Real Memory | {:<8.1f} | {:<8.1f} | {:<8.1f} ", Rfree, Rused, Rall));
						ImItem::RawText(str::format::insert("           PageFile | {:<8.1f} | {:<8.1f} | {:<8.1f} ", Pfree, Pused, Pall));

						if (ImGui::Button("Allock 64 MB"))
						{
							new char[1024 * 1024 * 64];
						}
					}
				}
				ImGui::End();
			}
		}

		return ImguiState.is_CPU_frame;
	}

	void ImGuiEnd()
	{
		if (ImguiState.is_CPU_frame)
		{
			ImGui::Render();

			if (ImguiState.software_raster == false)
			{
				ImguiState.api_impl->ImplHardDraw();
			}
			else // this block needs update
			{
				const ImGuiIO&    io           = ImGui::GetIO();
				const ImDrawData* im_draw_list = ImGui::GetDrawData();

				int fb_width  = (int)(im_draw_list->DisplaySize.x * io.DisplayFramebufferScale.x);
				int fb_height = (int)(im_draw_list->DisplaySize.y * io.DisplayFramebufferScale.y);

				static bitmap32 SOFT_REN_BMP;

				if (SOFT_REN_BMP.size() != INT2{ fb_width, fb_height })
					SOFT_REN_BMP = bitmap32(fb_width, fb_height);

				SOFT_REN_BMP.fill_by_color(color::to_color32(ClearColor));

				//-------------------------------------------------------------------// begin
				FLOAT2 DrawPos = im_draw_list->DisplayPos;

				for (int n = 0; n < im_draw_list->CmdListsCount; n++)
				{
					const ImDrawList* cmd_list   = im_draw_list->CmdLists[n];  // draw call list
					const ImDrawVert* vtx_buffer = cmd_list->VtxBuffer.Data;   // ptr to vertexes
					const ImDrawIdx*  idx_buffer = cmd_list->IdxBuffer.Data;   // ptr to index

					for (int cmd_it = 0; cmd_it < cmd_list->CmdBuffer.Size; cmd_it++)
					{
						const ImDrawCmd* pcmd = &cmd_list->CmdBuffer[cmd_it];
						if (pcmd->UserCallback)
						{
							// User callback (registered via ImDrawList::AddCallback)
							pcmd->UserCallback(cmd_list, pcmd);
						}
						else
						{
							FLOAT4 clip_rect = FLOAT4(
								pcmd->ClipRect.x - DrawPos.x,
								pcmd->ClipRect.y - DrawPos.y,
								pcmd->ClipRect.z - DrawPos.x,
								pcmd->ClipRect.w - DrawPos.y);

							if (   clip_rect.x < fb_width
								&& clip_rect.y < fb_height
								&& clip_rect.z >= 0.0f
								&& clip_rect.w >= 0.0f)
							{
								// Apply scissor/clipping rectangle
								//glScissor((int)clip_rect.x, (int)(fb_height - clip_rect.w), (int)(clip_rect.z - clip_rect.x), (int)(clip_rect.w - clip_rect.y));
								// Bind texture, Draw
								//glBindTexture(GL_TEXTURE_2D, (GLuint)(intptr_t)pcmd->TextureId);
								//glDrawElements(GL_TRIANGLES, (GLsizei)pcmd->ElemCount, sizeof(ImDrawIdx) == 2 ? GL_UNSIGNED_SHORT : GL_UNSIGNED_INT, idx_buffer);

								for (unsigned i = 0; i < pcmd->ElemCount; i += 3)
								{
                                    const auto& A = vtx_buffer[idx_buffer[i + 0]];
                                    const auto& B = vtx_buffer[idx_buffer[i + 2 / 2]];
                                    const auto& C = vtx_buffer[idx_buffer[i + 2]];

									auto unpack_color = [](unsigned col)
									{
										return color32{
											uint8_t((col >>  0) & 0xff),
											uint8_t((col >>  8) & 0xff),
											uint8_t((col >> 16) & 0xff),
											uint8_t((col >> 24) & 0xff)
										};
									};

									color32 Acol = unpack_color(A.col);
									color32 Bcol = unpack_color(B.col);
									color32 Ccol = unpack_color(C.col);

                                    const FLOAT2 uvA = A.uv;
									const FLOAT2 uvB = B.uv;
									const FLOAT2 uvC = C.uv;

									//draw::drawline2D(SOFT_REN_BMP, A.pos, B.pos, Acol, Bcol);
									//draw::drawline2D(SOFT_REN_BMP, B.pos, C.pos, Bcol, Ccol);
									//draw::drawline2D(SOFT_REN_BMP, C.pos, A.pos, Ccol, Acol);
									//draw::drawTriangle3D(
									//	SOFT_REN_BMP,
									//	nullptr,
									//	FLOAT3{ A.pos.x, A.pos.y, 2.0f },
									//	FLOAT3{ B.pos.x, B.pos.y, 2.0f },
									//	FLOAT3{ C.pos.x, C.pos.y, 2.0f },
									//	FLOAT4{ Acol.r / 256.f, Acol.g / 256.f, Acol.b / 256.f, Acol.a / 256.f },
									//	FLOAT4{ Bcol.r / 256.f, Bcol.g / 256.f, Bcol.b / 256.f, Bcol.a / 256.f },
									//	FLOAT4{ Ccol.r / 256.f, Ccol.g / 256.f, Ccol.b / 256.f, Ccol.a / 256.f },
									//	false);

									static bitmap32 font;
                                    static unsigned char* last_value = nullptr;
                                    static unsigned char* pixels = nullptr;
                                    int width, height;
                                    io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height);

									if (last_value != pixels)
									{
										last_value = pixels;
                                        font.copy_from_ptr(pixels, { width, height });
									}

									draw::vertex a = { { A.pos.x, A.pos.y, 1.0f, 1.0f }, { Acol.r / 256.f, Acol.g / 256.f, Acol.b / 256.f, Acol.a / 256.f }, uvA };
									draw::vertex b = { { B.pos.x, B.pos.y, 1.0f, 1.0f }, { Bcol.r / 256.f, Bcol.g / 256.f, Bcol.b / 256.f, Bcol.a / 256.f }, uvB };
									draw::vertex c = { { C.pos.x, C.pos.y, 1.0f, 1.0f }, { Ccol.r / 256.f, Ccol.g / 256.f, Ccol.b / 256.f, Ccol.a / 256.f }, uvC };

                                    draw::drawTriangle3D_color_tex(SOFT_REN_BMP, font, a, b, c);
								}
							}
						}
						idx_buffer += pcmd->ElemCount;
					}
				}

				if (true && "soft windows draw")
				{
					static GpuTexture gpu_res;
					gpu_res.Update(SOFT_REN_BMP);

					const FLOAT2 v_pos[] = {{-1,1},{1,1},{-1,-1},{1,-1} };
					const FLOAT2 t_pos[] = {{0,0},{1,0},{0,1},{1,1}};
					const unsigned ind[] = {0,1,3,0,3,2};

					//--mb delete, gles not support glEnableClientState
					glEnable(GL_TEXTURE_2D);
					glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
					glEnableClientState(GL_VERTEX_ARRAY);
					glEnableClientState(GL_TEXTURE_COORD_ARRAY);

					glVertexPointer  (2, GL_FLOAT, sizeof(FLOAT2), (const GLvoid*)v_pos);
					// not supported by EM
					glTexCoordPointer(2, GL_FLOAT, sizeof(FLOAT2), (const GLvoid*)t_pos);

					glBindTexture(GL_TEXTURE_2D, (GLuint)gpu_res.GetImpl()->uid);
					glDrawElements(GL_TRIANGLES, (GLsizei)6, GL_UNSIGNED_INT, ind);
					//--mb delete
				}

			}
		}
		else
			ImguiState.api_impl->ImplHardDraw();
	}

	//------------------  paint

	void BeginPaint()
	{
		if (ClearColorRainbow)
		{
			static SuperTimer clear_timer;
			while (clear_timer.InEach(1. / 30))
			{

				if (math::equal(ClearColor, ClearColorGo, 0.003f))
				{
                    ClearColorGo.x = (float)math::Random(0.0f, 0.999f);
                    ClearColorGo.y = (float)math::Random(0.0f, 0.999f);
                    ClearColorGo.z = (float)math::Random(0.0f, 0.999f);
				}
				else
				{
					//ClearColor.x += (ClearColorGo.x - ClearColor.x) / 99.0f;
					//ClearColor.y += (ClearColorGo.y - ClearColor.y) / 99.0f;
					//ClearColor.z += (ClearColorGo.z - ClearColor.z) / 99.0f;

					ClearColor.x = LERP(ClearColor.x, ClearColorGo.x, 0.02f);
					ClearColor.y = LERP(ClearColor.y, ClearColorGo.y, 0.02f);
					ClearColor.z = LERP(ClearColor.z, ClearColorGo.z, 0.02f);

				}
			}
		}

		Drawer::impl_output_clean(ClearColor);
	}

	void EndPaint()
	{
		Drawer::impl_output_present();
	}

public:
	void PaintTemplate()
	{
		BeginPaint();

		static bool debug = true;
		static bool demo = false;
		static bool stat = false;

		ImGuiFrame(&debug, &demo, &stat);

		MakeOnce
		{
			const auto curr = fs::current_path().string();

			const std::function<void(const std::string&)> path_dump = [&path_dump](const std::string& path) -> void
			{
				for (const auto& entry : fs::directory_iterator(path))
				{
					std::string path_str = entry.path().string();

					std::vector<std::string> dirs;
					std::vector<std::string> files;

					if (fs::is_directory(entry))
						dirs.emplace_back(std::move(path_str));
					else
						files.emplace_back(std::move(path_str));

					std::sort(dirs.begin(), dirs.end());
					std::sort(files.begin(), files.end());

					//////////////////////////////////////////////////////////////////////////
					for (const auto& dir : dirs)
						log::info << str::format::insert("[{}]", dir);

					for (const auto& dir : files)
						log::info << str::format::insert("{}", dir);

					//////////////////////////////////////////////////////////////////////////
					for (const auto& dir : dirs)
					{
						if (dir == "/dev")  continue;
						if (dir == "/proc") continue;
						path_dump(dir);
					}
				}
			};

			log::info << "________________________________";
			log::info << "Directory dump:";
			log::info << str::format::insert(" Current : {}", curr);
			path_dump(curr);
			log::info << "________________________________";
		}

		if (ImGui::Begin("SoftCam"))
		{
			const auto imCanvasPos  = ImGui::GetCursorPos() + ImGui::GetWindowPos();
			const auto imDrawList   = ImGui::GetWindowDrawList();
			const FLOAT2 canvasPos  = ImGui::GetCurrentWindow()->Pos + ImGui::GetCursorPos();
			const FLOAT2 canvasSize = { 360, 280 };

			//-----------------------------------------------------------//
			struct CamState
			{
				FLOAT3 pos_point = { 6.0, 6.0, 6.0 };
				FLOAT3 look_point = { 0.0, 0.0, 0.0 };
			};
			struct Object
			{
				FLOAT3 pos = { 0, 0, 0 };
				FLOAT3 root = { 0, 0, 0 };
				FLOAT3 size = { 1, 1, 1 };
			};

			//-----------------------------------------------------------//
			static CamState cam;
			static Object obj;

			static bool auto_rotate = false;
			static bool color_tex = false;

			if (auto_rotate)
				obj.root += FLOAT3{0.0f, 0.2f, 0.2f};

			static float aspect = 45.;

			ImGui::Checkbox("color/texture", &color_tex);
			ImGui::Separator();

			ImGui::DragFloat("azpect", &aspect);
			ImGui::DragFloat3("cam pos", &cam.pos_point.x);
			ImGui::DragFloat3("cam eye", &cam.look_point.x);
			ImGui::Separator();

			ImGui::Checkbox("Root", &auto_rotate);
			ImGui::DragFloat3("obj pos", &obj.pos.x);
			ImGui::DragFloat3("obj rot", &obj.root.x);
			ImGui::DragFloat3("obj siz", &obj.size.x);

			//obj.Root.x += 360;
			//obj.Root.y += 360;
			//obj.Root.z += 360;

            const glm::mat4 Proj = matProjection(aspect, canvasSize.x / canvasSize.y, 0.09f, 99.f);
            const glm::mat4 View = matView(cam.pos_point, cam.look_point);
            const glm::mat4 Model = matModel(obj.pos, obj.root, obj.size);
			const glm::mat4 WVP = Proj * View * Model;

			//-----------------------------------------------------------//

			static bitmap32		   ram_bitmap((int)canvasSize.x, (int)canvasSize.y);
			static bitmap_t<float> depth_buff((int)canvasSize.x, (int)canvasSize.y);

			static GpuTexture gpu_bitmap;

			gpu_bitmap.Update(ram_bitmap);
			//ImGui::Image((ImTextureID)gpu_bitmap.GetImpl()->uid, canvasSize * 2);
			ImItem::ShowImage(&gpu_bitmap, true, canvasSize * 2);

			ram_bitmap.fill_by_color({ 60, 60, 60, 255 });
			depth_buff.fill_by_color(999'999.f);

			//-----------------------------------------------------------//
			struct vert_pos_color_tex
			{
				glm::vec4 pos;
				FLOAT4 col;
				FLOAT2 tex;
			};

			const color4f red = { 0.99f, 0, 0, 0.99f };
			const color4f gre = { 0, 0.99f, 0, 0.99f };
			const color4f blu = { 0, 0, 0.99f, 0.99f };

			std::vector<vert_pos_color_tex> points =
			{
				{ { -0.99f, -0.99f,  0.99f, 0.99f }, red, {		0,	0		}, },
				{ {  0.99f, -0.99f,  0.99f, 0.99f }, gre, { 0.99f,	0		}, },
				{ {  0.99f,  0.99f,  0.99f, 0.99f }, red, { 0.99f,  0.99f	}, },
				{ { -0.99f, -0.99f,  0.99f, 0.99f }, red, {		0,	0		}, },
				{ { -0.99f,  0.99f,  0.99f, 0.99f }, blu, {		0,  0.99f	}, },
				{ {  0.99f,  0.99f,  0.99f, 0.99f }, red, { 0.99f,  0.99f	}, },
				{ { -0.99f, -0.99f, -0.99f, 0.99f }, red, {		0,	0		}, },
				{ {  0.99f, -0.99f, -0.99f, 0.99f }, gre, { 0.99f,	0		}, },
				{ {  0.99f,  0.99f, -0.99f, 0.99f }, red, { 0.99f,  0.99f	}, },
				{ { -0.99f, -0.99f, -0.99f, 0.99f }, red, {		0,	0		}, },
				{ { -0.99f,  0.99f, -0.99f, 0.99f }, blu, {		0,  0.99f	}, },
				{ {  0.99f,  0.99f, -0.99f, 0.99f }, red, { 0.99f,  0.99f	}, },
			};

			for (size_t i = 0; i < points.size() / 3; i++)
			{
				auto& va = points[i * 3];
				auto& vb = points[i * 3 + 2 / 2];
				auto& vc = points[i * 3 + 2];

				glm::vec4 aa = WVP * va.pos;
				glm::vec4 bb = WVP * vb.pos;
				glm::vec4 cc = WVP * vc.pos;

				if (aa.w < 0 && bb.w < 0 && cc.w < 0)
					continue;

				float az = aa.z;
				float bz = bb.z;
				float cz = cc.z;

				aa = aa / (-aa.z);
				bb = bb / (-bb.z);
				cc = cc / (-cc.z);

				FLOAT2 pos_a = (canvasSize / 2.0) + (canvasSize * FLOAT2{ aa.x, aa.y });
				FLOAT2 pos_b = (canvasSize / 2.0) + (canvasSize * FLOAT2{ bb.x, bb.y });
				FLOAT2 pos_c = (canvasSize / 2.0) + (canvasSize * FLOAT2{ cc.x, cc.y });

				FLOAT3 a = { pos_a.x, pos_a.y, az };
				FLOAT3 b = { pos_b.x, pos_b.y, bz };
				FLOAT3 c = { pos_c.x, pos_c.y, cz };

				if (color_tex) draw::drawTriangle3D(ram_bitmap, &depth_buff, a, b, c, va.col, vb.col, vc.col);
				else           draw::drawTriangle3D(ram_bitmap, &depth_buff, a, b, c, va.tex, vb.tex, vc.tex);
			}
		}
		ImGui::End();

		ImGuiEnd();
		EndPaint();
	}
};

