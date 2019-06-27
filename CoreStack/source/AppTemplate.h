#include "in.h"

class Render : public gpu_api::gpu_drawer
{
public:
 	virtual bool Init(const gpu_api::RenderOptions& RO)
	{
		mRS = RO;

		my_window = RO.MyWindow;

		if (!InitDevice		()) { log::info << "InitDevice error";        return false; }
		if (!InitTopologys	()) { log::info << "InitTopologys error";     return false; }
		if (!InitSamplers	()) { log::info << "InitSamplers error";      return false; }
		if (!InitRasterizers()) { log::info << "InitRasterizers error";   return false; }
		if (!InitBlenders	()) { log::info << "InitBlenders error";      return false; }

		return true;
	}
	virtual void Release()
	{
 	}

	// clear color
	bool   ClearColorRainbow = true;
	FLOAT4 ClearColor   = { 0,0,0,0 };
	FLOAT4 ClearColorGo = { 0,0,0,0 };

	// imgui
	 
	struct im_gui
	{
 		bool	is_skip_frames	= false;	// saves cpu time
		bool	software_raster = false;	// software raster
 		bool	is_CPU_frame	= true;		//  		
		float	fps_target		= 144.0;	// updates per second

		ImGuiProvider* api_impl;
 	};

	im_gui imgui;

public:

	void NotifyResize(size_t w, size_t h)
	{
		if (w == 0 || h == 0)
		{
			log::debug << fmt::format("Resized: {} - {}", w, h);
			log::debug << fmt::format("Warning! [x0] resolution setted. Resize skiped.");
  		}
		else
 		{
			log::debug << fmt::format("Resized: {} - {}", w, h);
			ResizeOutput(w, h);
		}
 	}
 	void NotifyFocus(bool focused)
	{
		mRS.IsFocused = focused;
	}

 	bool ImGuiStart(bool* ShowDebugWindow, bool* ShowDemo, bool* ShowStat)
	{
  		MakeOnce
		{
			IMGUI_CHECKVERSION();
			ImGui::CreateContext();

			ImGui::GetIO().IniFilename = NULL;
			ImGui::GetIO().LogFilename = NULL;
			
			ImplParams impl_params = {};
			imgui.api_impl = new ImGuiProvider();
			imgui.api_impl->ImplInit(this, &impl_params);

			ImGuiIO& io = ImGui::GetIO();
			io.Fonts->Clear();

			//ImFontConfig icons_config;
			//icons_config.MergeMode  = true;
			//icons_config.PixelSnapH = true;

			io.Fonts->AddFontDefault();
			io.Fonts->AddFontFromFileTTF("assets\\fonts\\DroidSans.ttf", 16.0f, NULL, io.Fonts->GetGlyphRangesCyrillic());

			//io.Fonts->AddFontFromFileTTF("assets\\fonts\\icons-Awesome.ttf", param_icon_font_size, &icons_config, range_awesome);
			//io.Fonts->AddFontFromFileTTF("assets\\fonts\\icons-Material.ttf", param_icon_font_size, &icons_config, range_material);
			//io.Fonts->AddFontFromFileTTF("assets\\fonts\\Cousine-Regular.ttf", 15.0f, NULL, io.Fonts->GetGlyphRangesCyrillic());
			//io.Fonts->AddFontFromFileTTF("assets\\fonts\\DroidSans.ttf",       16.0f, NULL, io.Fonts->GetGlyphRangesCyrillic());
			//io.Fonts->AddFontFromFileTTF("assets\\fonts\\Karla-Regular.ttf",   13.0f, NULL, io.Fonts->GetGlyphRangesCyrillic());
			//io.Fonts->AddFontFromFileTTF("assets\\fonts\\ProggyClean.ttf",     15.0f, NULL, io.Fonts->GetGlyphRangesCyrillic());
			//io.Fonts->AddFontFromFileTTF("assets\\fonts\\ProggyTiny.ttf",      12.0f, NULL, io.Fonts->GetGlyphRangesCyrillic());
			//io.Fonts->AddFontFromFileTTF("assets\\fonts\\Roboto-Medium.ttf",   20.0f, NULL, io.Fonts->GetGlyphRangesCyrillic());

			imgui.api_impl->ImplUpdateFontTex();
		}

  		//-------------------------------------------------- imgui fps set
		if (imgui.is_skip_frames)
		{
			static SuperTimer ST;
			static double time_escaped = 9999999;

			time_escaped += ST.GetRoundTime();

			if (time_escaped > (1.0 / imgui.fps_target))
			{
				time_escaped = 0.0;
				imgui.is_CPU_frame = true;	
			}
			else
				imgui.is_CPU_frame = false;
		}
		else
			imgui.is_CPU_frame = true;
		//-------------------------------------------------- imgui canvas size update
 		ImGuiIO& io = ImGui::GetIO();
		IM_ASSERT(io.Fonts->IsBuilt());
		
		int virt_w = mRS.RenderCanvasWight;
		int virt_h = mRS.RenderCanvasHeight;
		int real_w = mRS.RealCanvasWeight;
		int real_h = mRS.RealCanvasHight;
		
		static SuperTimer im_timer;
		
		io.DeltaTime = im_timer.GetRoundTime();
		io.DisplaySize = FLOAT2((float)virt_w, (float)virt_h);
		io.DisplayFramebufferScale = FLOAT2
		(
			real_w > 0 ? ((float)real_w / virt_w) : 0,
			real_h > 0 ? ((float)real_h / virt_h) : 0
		);
   
 		if (imgui.is_CPU_frame)
		{
			imgui.api_impl->ImplNewFrame();
			ImGui::NewFrame();

			if ("ImGui CPU work")
			{
 				if (*ShowDebugWindow)
				{
					ImGui::SetNextWindowPos(ImVec2(16, 16), ImGuiCond_Once);
					ImGui::SetNextWindowBgAlpha(0.8f);

					if (ImGui::Begin("_hover_debug_window", nullptr,
						ImGuiWindowFlags_AlwaysAutoResize 
						| ImGuiWindowFlags_NoTitleBar 
						| ImGuiWindowFlags_NoMove   
						| ImGuiWindowFlags_NoFocusOnAppearing 
						| ImGuiWindowFlags_NoBringToFrontOnFocus 
					 // | ImGuiWindowFlags_NoInputs
					))
					{
						ImVec2 min = { 0, 0 };
						ImVec2 max = { (float)mRS.RenderCanvasWight, (float)mRS.RealCanvasHight };
						ImVec4 color = { 1, 0, 0, 1 };

						//--------------------------------------------------//	Render settings
						ImGui::Text("Render settings");

						if (ImGui::Checkbox("VSunc", &mRS.vSunc))
						{
							if (mRS.vSunc == true)	glfwSwapInterval(1);
							else					glfwSwapInterval(0);
						}

						ImGui::Checkbox("Rainbow", &ClearColorRainbow);
						ImGui::ColorEdit3("Color", &ClearColor.x);

						if (ClearColorRainbow)
							ImGui::ColorEdit3("ColorGo", &ClearColorGo.x);
						//--------------------------------------------------//	ImGui params
						ImGui::Separator();
						ImGui::Text("ImGui params");
						ImGui::Checkbox("Software render (disabled)", &imgui.software_raster);
						ImGui::Checkbox("Save CPU time", &imgui.is_skip_frames);
						
						if (imgui.is_skip_frames)
						{
							ImGui::SameLine();
							ImGui::PushItemWidth(100);
							ImGui::SliderFloat("Target ImFps", &imgui.fps_target, 10, 300);
							ImGui::PopItemWidth();
						}
 
 						if (ImGui::Button("Light colors")) addon_imgui::imgui_set_colors_light();
						ImGui::SameLine();
						if (ImGui::Button("UltraLight colors")) addon_imgui::imgui_set_colors_lighter();

						//--------------------------------------------------//	Window stile
						ImGui::Separator();
						ImGui::Text("Window stile");
 
						if (ImGui::Button("Default"))    my_window->ChangeWindowFrame(WindowBorderStile::DefaultBorders); ImGui::SameLine();
						if (ImGui::Button("Borderless")) my_window->ChangeWindowFrame(WindowBorderStile::Borderless); ImGui::SameLine();
						if (ImGui::Button("Fullscreen")) my_window->ChangeWindowFrame(WindowBorderStile::BorderlessFullScr);
 
						if (ImGui::Button("TopMost")) my_window->SetTopMost(true); ImGui::SameLine();
						if (ImGui::Button("Normal"))  my_window->SetTopMost(false);
						//-----------------------------------------------------------------------------------//	Launch console
						ImGui::Separator();
						ImGui::Text("Launch console");
						
						if (ImGui::Button("Demo"))     *ShowDemo = !*ShowDemo; ImGui::SameLine();
						if (ImGui::Button("ShowStat")) *ShowStat = !*ShowStat;
  
						//-----------------------------------------------------------------------------------//	Debug info
						ImGui::Separator();
						ImGui::Text("Debug info");
						ImGui::Text(str::format::insert("RenderSize: [{} - {}]", mRS.RenderCanvasWight, mRS.RealCanvasHight).c_str());
						ImGui::Text(str::format::insert("WindowSize: [{} - {}]", my_window->GetWidth(), my_window->GetHeight()).c_str());
						ImGui::Text(str::format::insert("MousePos:   [{} - {}]", ImGui::GetIO().MousePos.x, ImGui::GetIO().MousePos.y).c_str());

						static gpu_api::gpu_texture dummy;

						ImGui::Text(str::format::insert("factory() {}", (void*)this->get_factory()).c_str());
						ImGui::Text(str::format::insert("device()  {}", (void*)this->get_device()).c_str());
						ImGui::Text(str::format::insert("rs()      {}", (void*)this->get_rs()).c_str());
						
						assert(this->get_factory()	== dummy.getf());
						assert(this->get_device()	== dummy.getd());
						assert(this->get_rs()		== dummy.getr());
  
						//-----------------------------------------------------------------------------------//
						ImGui::Separator();
						ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
					}
					ImGui::End();




				}
 				//----------------------------------------------------------------------// Demo  
				if (*ShowDemo)
				{
					ImGui::SetNextWindowPos(ImVec2(20, 20), ImGuiCond_Once);
					ImGui::ShowDemoWindow(ShowDemo);
				}
  				//----------------------------------------------------------------------// Render State
				if (*ShowStat)
				{
					if (ImGui::Begin("Render State", nullptr, 0))
					{
 						if (ImGui::CollapsingHeader("Window State"))
						{
   							//-------------------------------------------------------------------//	Window state

							ImGui::Text(fmt::format("GetX()               | {}", (int)my_window->GetX()).c_str());
							ImGui::Text(fmt::format("GetY()               | {}", (int)my_window->GetY()).c_str());
							ImGui::Text(fmt::format("GetWidth()           | {}", (int)my_window->GetWidth()).c_str());
							ImGui::Text(fmt::format("GetHeight()          | {}", (int)my_window->GetHeight()).c_str());
							ImGui::Text(fmt::format("IsActive()           | {}", (bool)my_window->IsActive()).c_str());
							ImGui::Text(fmt::format("IsExit()             | {}", (bool)my_window->IsExit()).c_str());
							ImGui::Text(fmt::format("IsMaximized()        | {}", (bool)my_window->IsMaximized()).c_str());
							ImGui::Text(fmt::format("IsMinimized()        | {}", (bool)my_window->IsMinimized()).c_str());
							ImGui::Text(fmt::format("IsResized()          | {}", (bool)my_window->IsResized()).c_str());
							ImGui::Text(fmt::format("WS.Title             | {}", (char*)my_window->GetWS()->Title.c_str()).c_str());
							ImGui::Text(fmt::format("WS.stile_borders     | {}", (int)my_window->GetWS()->stile_borders).c_str());
							ImGui::Text(fmt::format("WS.stile_alpha       | {}", (int)my_window->GetWS()->stile_alpha).c_str());
							ImGui::Text(fmt::format("WS.stile_transperent | {}", (int)my_window->GetWS()->stile_transperent).c_str());
							ImGui::Text(fmt::format("WS.SpawnPos          | {}", (int)my_window->GetWS()->SpawnPos).c_str());
							ImGui::Text(fmt::format("WS.X                 | {}", (int)my_window->GetWS()->X).c_str());
							ImGui::Text(fmt::format("WS.Y                 | {}", (int)my_window->GetWS()->Y).c_str());
							ImGui::Text(fmt::format("WS.Wight             | {}", (int)my_window->GetWS()->Wight).c_str());
							ImGui::Text(fmt::format("WS.Height            | {}", (int)my_window->GetWS()->Height).c_str());
							ImGui::Text(fmt::format("WS.Resizble          | {}", (bool)my_window->GetWS()->Resizble).c_str());
							ImGui::Text(fmt::format("WS.Visable           | {}", (bool)my_window->GetWS()->Visable).c_str());
							ImGui::Text(fmt::format("WS._IsTopMost        | {}", (void*)my_window->GetWS()->TopMost).c_str());
							ImGui::Text(fmt::format("WS._hWindow          | {}", (void*)my_window->GetWS()->_hWindow.hwnd).c_str());
							ImGui::Text(fmt::format("WS._Hmodule          | {}", (void*)my_window->GetWS()->_Hmodule.module).c_str());
						}
						if (ImGui::CollapsingHeader("Render State"))
						{
							ImGui::Text(fmt::format("mRS.WindowHwnd          | {}", (void*)mRS.WindowId.hwnd).c_str());
							ImGui::Text(fmt::format("mRS.FullScreen          | {}", (bool)mRS.FullScreen).c_str());
							ImGui::Text(fmt::format("mRS.vSunc               | {}", (bool)mRS.vSunc).c_str());
							ImGui::Text(fmt::format("mRS.SlowOnUnFocuse      | {}", (bool)mRS.SlowOnUnFocuse).c_str());
							ImGui::Text(fmt::format("mRS.RealCanvasWeight    | {}", (int)mRS.RealCanvasWeight).c_str());
							ImGui::Text(fmt::format("mRS.RealCanvasHight     | {}", (int)mRS.RealCanvasHight).c_str());
							ImGui::Text(fmt::format("mRS.VirtualCanvasWight  | {}", (int)mRS.RenderCanvasWight).c_str());
							ImGui::Text(fmt::format("mRS.VirtualCanvasHeight | {}", (int)mRS.RenderCanvasHeight).c_str());
							ImGui::Text(fmt::format("mRS.AAMode              | {}", (int)mRS.AAMode).c_str());
							ImGui::Text(fmt::format("mRS.MSAA_SamplerCount   | {}", (int)mRS.MSAA_SamplerCount).c_str());
							ImGui::Text(fmt::format("mRS.MSAA_QualityLevel   | {}", (int)mRS.MSAA_QualityLevel).c_str());
							ImGui::Text(fmt::format("mRS.AnisotropicQuality  | {}", (int)mRS.AnisotropicQuality).c_str());
						}
						if (ImGui::CollapsingHeader("UNIVERSAL"))
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
 
						if (ImGui::CollapsingHeader("Gpu Adapters"))
						{

							struct output_test
							{
								void* output_ptr;
								gpu_api::gpu_output::output_info info;
								std::vector<gpu_api::gpu_output::output_mode> modes;

							};
							struct adp_test
							{
								void*					 adp_ptr;
								gpu_api::gpu_adapter::adapter_info adp_info;
								std::vector<output_test> outputs;
							};

							static std::vector<adp_test> a_array;

							MakeOnce
							{
								std::vector<gpu_api::gpu_adapter> adp_array = gpu_drawer::enum_gpu_adapters();

								ForArray(i, adp_array.size())
								{
									adp_test test;

									test.adp_ptr = (void*)adp_array[i].GetAdapter();
									test.adp_info = adp_array[i].GetInfo();

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
						}

						if (ImGui::CollapsingHeader("System Metric"))
						{
							static auto mc = os_api::get_system_metric(os::os_system_metric::VISIBLE_MONITOR_COUNT);
							static auto mw = os_api::get_system_metric(os::os_system_metric::MAIN_MONITOR_W);
							static auto mh = os_api::get_system_metric(os::os_system_metric::MAIN_MONITOR_H);

							ImGui::Text(str::format::insert("VISIBLE_MONITOR_COUNT   | {}", (int)mc.GetValue()).c_str());;
							ImGui::Text(str::format::insert("MAIN_MONITOR_W          | {}", (int)mw.GetValue()).c_str());;
							ImGui::Text(str::format::insert("MAIN_MONITOR_H          | {}", (int)mh.GetValue()).c_str());;
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
							static float Vfree, Vused, Vall;
							static float Rfree, Rused, Rall;
							static float Pfree, Pused, Pall;
							static float Mload;
							static float CurV;
							static float CurR;

 							if (update)
							{
								update = false;

								Vfree = (float)os_api::get_ram_metric(os_ram_metric::MEM_VirtualMemoryAvail).GetValue() / 1024. / 1024.;
								Vused = (float)os_api::get_ram_metric(os_ram_metric::MEM_VirtualMemoryLoad).GetValue() / 1024. / 1024.;
								Vall  = (float)os_api::get_ram_metric(os_ram_metric::MEM_VirtualMemoryTotal).GetValue() / 1024. / 1024.;
								Rfree = (float)os_api::get_ram_metric(os_ram_metric::MEM_RealMemoryAvail).GetValue() / 1024. / 1024.;
								Rused = (float)os_api::get_ram_metric(os_ram_metric::MEM_RealMemoryLoad).GetValue() / 1024. / 1024.;
								Rall  = (float)os_api::get_ram_metric(os_ram_metric::MEM_RealMemoryTotal).GetValue() / 1024. / 1024.;
								Pfree = (float)os_api::get_ram_metric(os_ram_metric::MEM_PageFileAvail).GetValue() / 1024. / 1024.;
								Pused = (float)os_api::get_ram_metric(os_ram_metric::MEM_PageFileLoad).GetValue() / 1024. / 1024.;
								Pall  = (float)os_api::get_ram_metric(os_ram_metric::MEM_PageFileSize).GetValue() / 1024. / 1024.;
								Mload = (float)os_api::get_ram_metric(os_ram_metric::MEM_MemoryLoadProcent).GetValue();
								CurV  = (float)os_api::get_ram_metric(os_ram_metric::MEM_CurentProcVirtualMemUsage).GetValue() / 1024. / 1024.;
								CurR  = (float)os_api::get_ram_metric(os_ram_metric::MEM_CurentProcRealMemUsage).GetValue() / 1024. / 1024.;
							}

							ImGui::Text(str::format::insert("      Memory Usage | {0:.1f}%%", Mload).c_str());
							ImGui::Text(str::format::insert("CurentProc Mem V/R | {0:.1f} / {1:.1f}", CurV, CurR).c_str());
							ImGui::Text(str::format::insert("_______________________________________________").c_str());
							ImGui::Text(str::format::insert("                   |  Free   |  Used   |  Total").c_str());
							ImGui::Text(str::format::insert("    Virtual Memory | {0:.1f}  | {1:.1f}   | {2:.1f}", Vfree, Vused, Vall).c_str());
							ImGui::Text(str::format::insert("       Real Memory | {0:.1f}  | {1:.1f}  | {2:.1f}", Rfree, Rused, Rall).c_str());
							ImGui::Text(str::format::insert("          PageFile | {0:.1f} | {1:.1f} | {2:.1f}", Pfree, Pused, Pall).c_str());

							if (ImGui::Button("Allock 64 MB"))
							{
								void *aaa = new char[1024 * 1024 * 64];
							}
						}

					}
					ImGui::End();
				}
  			}
		}

		return imgui.is_CPU_frame;
  	}
	void ImGuiEnd()
	{
		if (imgui.is_CPU_frame)
		{
  			ImGui::Render();

			if (imgui.software_raster == false)
			{
				imgui.api_impl->ImplHardDraw();
			}
			else
			{
				static bitmap32 SOFT_REN_BMP;

				auto draw_data = ImGui::GetDrawData();

				ImGuiIO& io = ImGui::GetIO();
				int fb_width = (int)(draw_data->DisplaySize.x * io.DisplayFramebufferScale.x);
				int fb_height = (int)(draw_data->DisplaySize.y * io.DisplayFramebufferScale.y);

				if (SOFT_REN_BMP.size() != INT2{ fb_width, fb_height })
					SOFT_REN_BMP = bitmap32(fb_width, fb_height);

				//SOFT_REN_BMP.fill_by_color(color4f(ClearColor).to_color32());
				SOFT_REN_BMP.fill_by_color({ 0,0,0,255 });
				/////////////////////////////////////////////////////////////////////////////////
				FLOAT2 pos = draw_data->DisplayPos;
				for (int n = 0; n < draw_data->CmdListsCount; n++)
				{
					const ImDrawList* cmd_list = draw_data->CmdLists[n];
					const ImDrawVert* vtx_buffer = cmd_list->VtxBuffer.Data;
					const ImDrawIdx*  idx_buffer = cmd_list->IdxBuffer.Data;

					for (int cmd_i = 0; cmd_i < cmd_list->CmdBuffer.Size; cmd_i++)
					{
						const ImDrawCmd* pcmd = &cmd_list->CmdBuffer[cmd_i];
						if (pcmd->UserCallback)
						{
							// User callback (registered via ImDrawList::AddCallback)
							pcmd->UserCallback(cmd_list, pcmd);
						}
						else
						{
							ImVec4 clip_rect = ImVec4(pcmd->ClipRect.x - pos.x, pcmd->ClipRect.y - pos.y, pcmd->ClipRect.z - pos.x, pcmd->ClipRect.w - pos.y);
							if (clip_rect.x < fb_width && clip_rect.y < fb_height && clip_rect.z >= 0.0f && clip_rect.w >= 0.0f)
							{
								// Apply scissor/clipping rectangle
								//glScissor((int)clip_rect.x, (int)(fb_height - clip_rect.w), (int)(clip_rect.z - clip_rect.x), (int)(clip_rect.w - clip_rect.y));

								// Bind texture, Draw
								//glBindTexture(GL_TEXTURE_2D, (GLuint)(intptr_t)pcmd->TextureId);
								//glDrawElements(GL_TRIANGLES, (GLsizei)pcmd->ElemCount, sizeof(ImDrawIdx) == 2 ? GL_UNSIGNED_SHORT : GL_UNSIGNED_INT, idx_buffer);
								
								for (int i = 0; i < pcmd->ElemCount; i += 3)
								{
									auto A = vtx_buffer[idx_buffer[i + 0]];
									auto B = vtx_buffer[idx_buffer[i + 2 / 2]];
									auto C = vtx_buffer[idx_buffer[i + 2]];

									auto unpack_color = [](unsigned int col)
									{
										return color32{
											(col >> 24) & 0xff,
											(col >> 16) & 0xff,
											(col >> 8) & 0xff,
											(col >> 0) & 0xff
										};
									};
																		
									color32 Acol = unpack_color(A.col);
									color32 Bcol = unpack_color(B.col);
									color32 Ccol = unpack_color(C.col);

									draw::drawline2D(SOFT_REN_BMP, A.pos, B.pos, Acol, Bcol);
									draw::drawline2D(SOFT_REN_BMP, B.pos, C.pos, Bcol, Ccol);
									draw::drawline2D(SOFT_REN_BMP, C.pos, A.pos, Ccol, Acol);

									//ren.drawtriangle(A.pos, B.pos, C.pos, Acol, Bcol, Ccol);
								}




							}
						}
						idx_buffer += pcmd->ElemCount;
					}
				}
				
				static gpu_api::gpu_sampler sampler_lenear;
				static GpuTexture gpu_t;
				static bitmap32 ram_clip;
				//gpu_t.CreateUpdateData(SOFT_REN_BMP);
				/////////////////////////////////////////////////////////////////////////////////
				ClearColor   = { 0,0,0,255 };
				ClearColorGo = { 0,0,0,255 };

				MakeOnce 
				{
					auto call = os::io::clipboard::GetClipboardImage();
					
					if (call.IsSucceed())
					{
						ram_clip = std::move(call.GetValue());
					}

					sampler_lenear.Create(gpu_api::gpu_sampler::GetLinearPreset());
				}

				gpu_t.BindSampler(sampler_lenear);
				gpu_t.CreateUpdateData(ram_clip);//(SOFT_REN_BMP);
				
				const float w = (float)mRS.RenderCanvasWight; // os_api::get_system_metric(os_system_metric::MAIN_MONITOR_W).GetValue();
				const float h = (float)mRS.RenderCanvasHeight;//os_api::get_system_metric(os_system_metric::MAIN_MONITOR_H).GetValue();

				const FLOAT2 size = FLOAT2{ w, h }; // -h to flip horisontaly
				const FLOAT2 mid = size / 2.;

				const auto angle = math::anlage2D(mid, { size.x,0 });
				const auto dist = math::distance2D(mid, size);

				static float rootation = 0;
				rootation += 0.36f;

				FLOAT2 vert[] =
				{
					math::move_on_angle(mid,   0. - angle + rootation, dist),
					math::move_on_angle(mid,   0. + angle + rootation, dist),
					math::move_on_angle(mid,2*90. + angle + rootation, dist),
					math::move_on_angle(mid,2*90. - angle + rootation, dist),
				};
				
				ForArray(i, 4)
				{
					vert[i] = (vert[i] - (size / 2)) / (size / 2) * FLOAT2{0.999f, -0.999f};
				}


				//FLOAT2 vert[] = {
				//	{-0.999999,	+0.999999	},
				//	{+0.999999,	+0.999999	},
				//	{-0.999999,	-0.999999	},
				//	{+0.999999,	-0.999999	},
				//};

				FLOAT2 tpos[] = {
					{+0,		+0			},
					{+0.99999,	+0			},
					{+0,		+0.99999	},
					{+0.99999,	+0.99999	},
				};
				unsigned int ind[] = {
					0, 2/2, 3,
					0, 3, 2,
				};
				
				//static float r = 0;
				//r += 0.06;
				
				//glMatrixMode(GL_MODELVIEW_MATRIX);
				//glPushMatrix();
				//glLoadIdentity();
				//glRotatef(r, 0, 0, 0.999);
				{
					glEnableClientState(GL_VERTEX_ARRAY);
					glEnableClientState(GL_TEXTURE_COORD_ARRAY);

					glVertexPointer  (2, GL_FLOAT, sizeof(FLOAT2), (const GLvoid*)vert);
					glTexCoordPointer(2, GL_FLOAT, sizeof(FLOAT2), (const GLvoid*)tpos);
				
					glEnable(GL_TEXTURE_2D);
					glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

					glBindTexture(GL_TEXTURE_2D, (GLuint)(intptr_t)(void*)gpu_t.GetTexture()->uid_tex);
					glDrawElements(GL_TRIANGLES, (GLsizei)6, GL_UNSIGNED_INT, ind);
				}
				glPopMatrix();
			}
		}
		else
			imgui.api_impl->ImplHardDraw();
  	}

	void BeginPaint() 
	{
 		if (ClearColorRainbow)
		{
			static SuperTimer clear_timer;
			static float esc = 999'999;

			esc += clear_timer.GetRoundTime();

			if (esc > 2.0 / 266.0)
			{
				esc = 0;

				if (math::equal(ClearColor, ClearColorGo, 0.001f))
				{
					ClearColorGo.x = math::RandomFastReal(0.0f, 0.999f);
					ClearColorGo.y = math::RandomFastReal(0.0f, 0.999f);
					ClearColorGo.z = math::RandomFastReal(0.0f, 0.999f);
				}
				else
				{
					ClearColor.x += (ClearColorGo.x - ClearColor.x) / 99.0f;
					ClearColor.y += (ClearColorGo.y - ClearColor.y) / 99.0f;
					ClearColor.z += (ClearColorGo.z - ClearColor.z) / 99.0f;
				}
			}
		}
		
 		glViewport(0, 0, get_rs()->RenderCanvasWight, get_rs()->RenderCanvasHeight);
 		glClearColor(ClearColor.x, ClearColor.y, ClearColor.z, ClearColor.w);
		glClear(GL_COLOR_BUFFER_BIT);
	}
  	void EndPaint() 
	{
 		glfwMakeContextCurrent(glfw_window);
		glfwSwapBuffers(glfw_window);
  	}
 
public:
	virtual void Paint()
	{
		BeginPaint();

		static bool debug = true;
		static bool demo  = true;
		static bool stat  = true;

		ImGuiStart(&debug, &demo, &stat);

		// 2d code

		ImGuiEnd();

		EndPaint();
	}


protected:
	os::Window *my_window;//for dubug
};


 

