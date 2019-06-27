#pragma once

#include "pch.h"
#include "MyInput.h"

// views
#include "imview_zlib_test.h"
#include "imview.h"

// live
#include "imview_live_test.h"
#include "imview.playground.h"
#include "imview_opencl.h"

// bitmap font
#include "img_bitmap_font.h"


int TRG(bitmap32& ram_bitmap, bool comm, FLOAT2& a, FLOAT2& b, FLOAT2& c)
{
	//static bitmap32 ram_bitmap(2048, 2048);
	SuperTimer st;

	//while (true)
	{
		st.SetStart();

		const int w = ram_bitmap.size().x;
		const int h = ram_bitmap.size().y;
		uint8_t* data = &ram_bitmap.raw_data()->r;



		const int w_start = std::min(std::min(a.x, b.x), c.x);
		const int w_end   = std::max(std::max(a.x, b.x), c.x);

		const int h_start = std::min(std::min(a.y, b.y), c.y);
		const int h_end   = std::max(std::max(a.y, b.y), c.y);

 		for (int y = h_start; y < h_end; y++)
		{
			FLOAT2 pos(0, y);
			int ls = y * w;

			for (int x = w_start; x < w_end; x++)
			{
				pos.x = x;

				if (math::is_point_in_triangle2D(pos, a, b, c))
					data[(ls + x) * 4] = 255;
			}
		}

		st.SetEnd();

		if (comm)
			log::debug << std::to_string(st.GetTimeReal());
	}

	return 0;
}

//win32
namespace
{
	std::string Win32errorToString(DWORD err)
	{
		//Get the error message, if any.
		DWORD errorMessageID = err;
		if (errorMessageID == 0)
			return std::string(); //No error message has been recorded

		LPSTR messageBuffer = nullptr;
		size_t size = FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
			NULL, errorMessageID, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&messageBuffer, 0, NULL);

		std::string message(messageBuffer, size);

		ForArray(i, message.size())
		{
			if (message[i] == '\n' || message[i] == '\r')
				message.erase(i);
		}

		//Free the buffer.
		LocalFree(messageBuffer);

		return message;
	}

	struct ScreenGrabber
	{
		HDC hScreen;
		HDC hCaptureDC;
		HBITMAP hCaptureBitmap;
		HGDIOBJ hOld;
		BITMAPINFO MyBMInfo = { 0 };
		////////////////////////////////
		INT2 grabSize;
		HWND lastWindow = NULL;
		bool captured = false;
		bitmap32 captured_bitmap;

		void grubFromPoint(INT2 point, INT2 size, bool desktop)
		{
			if (point.x < size.x / 2) point.x = size.x / 2;
			if (point.y < size.y / 2) point.y = size.y / 2;

			grabSize = size;

			POINT mPos{ point.x, point.y };
			HWND hTargetWindow = WindowFromPoint(mPos);
			//HWND hTopWindow = GetParent(hPointWindow);

			//hTargetWindow = GetWindow(hTargetWindow, GW_OWNER);

			if (desktop)
				hTargetWindow = NULL;

			POINT winLocalPos{ mPos.x, mPos.y };
			ScreenToClient(hTargetWindow, &winLocalPos);

			RECT rect;
			rect.left = -grabSize.x / 2;
			rect.top = -grabSize.y / 2;
			rect.right = +grabSize.x / 2;
			rect.bottom = +grabSize.y / 2;

			int x1 = winLocalPos.x + rect.left;
			int y1 = winLocalPos.y + rect.top;

			if (captured && lastWindow != hTargetWindow)
			{
				lastWindow = hTargetWindow;
				captured = false;

				SelectObject(hCaptureDC, hOld);
				DeleteDC(hCaptureDC);
				ReleaseDC(NULL, hScreen);
				DeleteObject(hCaptureBitmap);
			}
			if (!captured)
			{
				captured = true;
				//////////////////////////////////////////////////////////////////////////
				hScreen = GetDC(hTargetWindow);
				hCaptureDC = CreateCompatibleDC(hScreen);
				hCaptureBitmap = CreateCompatibleBitmap(hScreen, grabSize.x, grabSize.y);
				hOld = SelectObject(hCaptureDC, hCaptureBitmap);
				//////////////////////////////////////////////////////////////////////////
				MyBMInfo.bmiHeader.biSize = sizeof(MyBMInfo.bmiHeader);

				if (0 == BitBlt(hCaptureDC, 0, 0, grabSize.x, grabSize.y, hScreen, x1, y1, SRCCOPY | CAPTUREBLT))
					log::error << "BitBlt failed";

				if (0 == GetDIBits(hCaptureDC, hCaptureBitmap, 0, 0, NULL, &MyBMInfo, DIB_RGB_COLORS))
					log::error << "GetDIBits failed";

				ReleaseDC(hTargetWindow, hScreen);

				MyBMInfo.bmiHeader.biCompression = BI_RGB;
				MyBMInfo.bmiHeader.biHeight = -grabSize.x;
				MyBMInfo.bmiHeader.biWidth = grabSize.y;
			}

			hScreen = GetDC(hTargetWindow);

			if (captured_bitmap.size() != grabSize)
				captured_bitmap = bitmap32(grabSize.x, grabSize.y);

			if (0 == BitBlt(hCaptureDC, 0, 0, grabSize.x, grabSize.y, hScreen, x1, y1, SRCCOPY | CAPTUREBLT))
				log::error << "BitBlt failed";

			if (0 == GetDIBits(hCaptureDC, hCaptureBitmap, 0, MyBMInfo.bmiHeader.biHeight, (LPVOID)captured_bitmap.raw_data(), &MyBMInfo, DIB_RGB_COLORS))
				log::error << "GetDIBits failed";

			ReleaseDC(hTargetWindow, hScreen);
		}

		void grubScreen()
		{
			grabSize.x = os_api::get_system_metric(os_sys_metric::MAIN_MONITOR_W).GetValue();
			grabSize.y = os_api::get_system_metric(os_sys_metric::MAIN_MONITOR_H).GetValue();

			HWND hTargetWindow = FindWindowA("SysListView32", "FolderView");///NULL;

			if (lastWindow != hTargetWindow)
			{
				lastWindow = hTargetWindow;
				captured = false;

				SelectObject(hCaptureDC, hOld);
				DeleteDC(hCaptureDC);
				ReleaseDC(NULL, hScreen);
				DeleteObject(hCaptureBitmap);
			}
			if (!captured)
			{
				captured = true;
				//////////////////////////////////////////////////////////////////////////
				hScreen = GetDC(hTargetWindow);
				hCaptureDC = CreateCompatibleDC(hScreen);
				hCaptureBitmap = CreateCompatibleBitmap(hScreen, grabSize.x, grabSize.y);
				hOld = SelectObject(hCaptureDC, hCaptureBitmap);
				//////////////////////////////////////////////////////////////////////////
				MyBMInfo.bmiHeader.biSize = sizeof(MyBMInfo.bmiHeader);


				if (0 == BitBlt(hCaptureDC, 0, 0, grabSize.x, grabSize.y, hScreen, 0, 0, SRCCOPY))
					log::error << "BitBlt failed";

				if (0 == GetDIBits(hCaptureDC, hCaptureBitmap, 0, 0, NULL, &MyBMInfo, DIB_RGB_COLORS))
					log::error << "GetDIBits failed";

				ReleaseDC(hTargetWindow, hScreen);

				MyBMInfo.bmiHeader.biCompression = BI_RGB;
				MyBMInfo.bmiHeader.biHeight = -grabSize.y;
				MyBMInfo.bmiHeader.biWidth = grabSize.x;
			}

			if (captured_bitmap.size() != grabSize)
				captured_bitmap = bitmap32(grabSize.x, grabSize.y);

			hScreen = GetDC(hTargetWindow);

			if (0 == BitBlt(hCaptureDC, 0, 0, grabSize.x, grabSize.y, hScreen, 0, 0, SRCCOPY ))
				log::error << "BitBlt failed";

			ReleaseDC(hTargetWindow, hScreen);

			if (0 == GetDIBits(hCaptureDC, hCaptureBitmap, 0, MyBMInfo.bmiHeader.biHeight, (LPVOID)captured_bitmap.raw_data(), &MyBMInfo, DIB_RGB_COLORS))
				log::error << "GetDIBits failed";

		}

	};
}

/////////////////////////////
struct BuckGrowndWorker
{
	BuckGrowndWorker()
	{

	}


	void launch()
	{
		active = true;
 		active = false;
	}

protected:
	atomic_bool active = false;
	//atomic_bool finished = false;
};

namespace imgenc
{
	bitmap32 normalize(const bitmap32& arg)
	{
		bitmap32 out = arg.get_copy();

		const auto norc = [](int c)
		{
			return c - c % 2;
		};

		ForArray(i, out.pixel_count())
		{
			out.raw_data(i)->r = norc(out.raw_data(i)->r);
			out.raw_data(i)->g = norc(out.raw_data(i)->g);
			out.raw_data(i)->b = norc(out.raw_data(i)->b);
			out.raw_data(i)->a = 255;
		}

		return std::move(out);
	}
}
//////////////////////////
class MyRender : public BasicRender
{
public:
	MyRender() : UCLASS("AppRender"), BasicRender()
	{
	}

protected:
	//--------- for debug
	int  mGLerrCount     = 0;
	int  mWin32errCount  = 0;
	bool mCheck_GLErrors    = true;
	bool mChech_Win32Errors = false;
	int  mPostAct_WindowAlphaState = 0; // mb dell
	//---------

	struct basic_hot_keys
	{
		void Add(vector<KKey> /*keys_seq*/, bool ctrl, bool shift, bool alt) { }
	};

public:
	void Paint()
	{
		// Meta-helpers
		//------------------------------------
		// load fonts between frames
		struct ImFontLoader
		{
			std::vector<std::string> IM_FONTS;

			ImGuiProvider* prov = nullptr;
			bool  need_update = false;
			int   font_id = 0;
			float font_size = 16.f;

		public:
			void if_update(ImGuiProvider* pProvider)
			{
				if (!pProvider)
					return;

				prov = pProvider;

				MakeOnce
				{
					need_update = true;

					const std::string curr = fs::current_path().string();
					const std::string path = ASSET_DIR + "fonts";

					for (const auto& entry : fs::directory_iterator(path))
					{
						if (fs::is_regular_file(entry) && entry.path().extension() == ".ttf")
						{
							IM_FONTS.emplace_back(entry.path().string());
						}
					}
				}

				if (need_update)
				{
					need_update = false;

					ImGuiIO& io = ImGui::GetIO();
					io.Fonts->Clear();
					io.Fonts->AddFontDefault();

					const auto rus_gly = io.Fonts->GetGlyphRangesCyrillic();

					for (const auto& entry : IM_FONTS)
					{
						std::string path = entry;
						io.Fonts->AddFontFromFileTTF(path.c_str(), font_size, NULL, rus_gly);
					}

					prov->ImplUpdateFontTex();

					ImFont* font = io.Fonts->Fonts[font_id];
					io.FontDefault = font;
				}
			}

			void if_draw()
			{
				need_update |= ImGui::InputFloat("font size", &font_size, 0.5, 2);

				ImGuiIO& io = ImGui::GetIO();
				ImFont* font_current = ImGui::GetFont();

				for (int n = 0; n < io.Fonts->Fonts.Size; n++)
				{
					ImFont* font = io.Fonts->Fonts[n];
					ImGui::PushID((void*)font);
					if (ImGui::Selectable(font->GetDebugName(), font == font_current))
					{
						io.FontDefault = font;
						font_id = n;
					}
					ImGui::PopID();
				}
			}
		};

		// fonts can`t be reloaded during frame
		static ImFontLoader im_font_loader;
		im_font_loader.if_update(ImguiState.api_impl);
		//------------------------------------

		BeginPaint();

		static bool show_debug = true;
		static bool show_demo  = false;
		static bool show_stat  = false;

		if (ImGuiFrame(&show_debug, &show_demo, &show_stat))
		{
			//---------------------------// LAUNCHER
			struct im_window
			{
				bool active        = false;
				bool is_first_show = true;
				bool collapsed     = true;// collapsed windows call func too
				const char* name   = nullptr;
				const char* group  = nullptr;
				SuperTimer timer;
			};

			// TODO mb change key to string
			static std::unordered_map<const char*, std::map<const char*, im_window>> im_windows;

			const auto IM_LAUNCHER = []()
			{
				const std::string hover_wnd_name = "_hover_debug_window";

				if (ImGui::Begin(hover_wnd_name.c_str(), nullptr))
				{
					ImGui::NewLine();

					if (ImGui::BeginTabBar("##tabs", ImGuiTabBarFlags_None))
					{
						if (ImGui::BeginTabItem("Launcher"))
						{
							const FLOAT2 lb_button_size(200, 0);

							int btn_id = 0;

							for (auto& gr_pair : im_windows)
							{
								ImGui::Text(*gr_pair.first == '\0' ? "[no tag]" : gr_pair.first);
								ImGui::Separator();

								for (auto& win_pair : gr_pair.second)
								{
									std::string btn_str = str::format::insert("{}##{}", win_pair.first, btn_id++);

									if (win_pair.second.active)
									{
										if (win_pair.second.collapsed)
										{
											ImGui::PushStyleColor(ImGuiCol_Button,        IM_COL32(220, 220,   0, 255));
											ImGui::PushStyleColor(ImGuiCol_ButtonHovered, IM_COL32(255, 255,  80, 255));
											ImGui::PushStyleColor(ImGuiCol_ButtonActive,  IM_COL32( 99,  99,  99, 255));
										}
										else
										{
											ImGui::PushStyleColor(ImGuiCol_Button,        IM_COL32(  0,  220,   0, 255));
											ImGui::PushStyleColor(ImGuiCol_ButtonHovered, IM_COL32( 80,  255,  80, 255));
											ImGui::PushStyleColor(ImGuiCol_ButtonActive,  IM_COL32( 99,   99,  99, 255));
										}

										win_pair.second.active =
											ImGui::Button(btn_str.c_str(), lb_button_size) ? false : true;

										ImGui::SameLine();
										ImItem::Text("{} us", win_pair.second.timer.GetTimeMicro());

										ImGui::PopStyleColor();
										ImGui::PopStyleColor();
										ImGui::PopStyleColor();
									}
									else
 										win_pair.second.active =
											ImGui::Button(btn_str.c_str(), lb_button_size) ? true : false;

								}
								ImGui::NewLine();
							}

							ImGui::EndTabItem();
						}

 						ImGui::EndTabBar();
					}

				}

				ImGui::End();
			};
			const auto IM_VIEW = [](const char* gtag, const char* title, std::function<void()> func, bool show = false)
			{
				auto& win = im_windows[gtag][title];

				bool first_call = !win.name;

				if (first_call)
				{
					win.name = title;
					win.group = gtag;
					win.is_first_show = true;

					if (show)
						win.active = true;
				}

				if (win.active)
				{
					if (win.is_first_show)
					{
						win.is_first_show = false;

						const FLOAT2 pos  = FLOAT2(math::Random(200., 600.), math::Random(200., 600.));
						const FLOAT2 size = FLOAT2(math::Random(200., 600.), math::Random(200., 600.));

						ImGui::SetNextWindowPos(pos, ImGuiCond_Always);
						ImGui::SetNextWindowSize(size, ImGuiCond_Always);
					}

					const std::string str = str::format::insert("[{}] - {}", win.group, win.name);

					win.collapsed = !ImGui::Begin(str.c_str(), &win.active);


					if (Keyboard.Switchers.KeyCtrl == true &&
						Keyboard.WasPressed(os::KKey::KEY_W) &&
						ImGui::IsWindowFocused())
					{
						win.active = false;
					}

					win.timer.SetStart();
					func();
					win.timer.SetEnd();

					ImGui::End();
				}

				return;
			};
			const auto IM_WND = [IM_VIEW](const char* title, std::function<void()> func)
			{
				return IM_VIEW("", title, func, true);
			};

			IM_LAUNCHER();

			//-------------------------------------------------------// GRAPHICS
			IM_VIEW("GRAPHICS", "BITMAP32 RASTR",    []() { imview::graphics_drawers(); });
			IM_VIEW("GRAPHICS", "BITMAP32 BENCH",    []() { imview::graphics_bench();   });
			IM_VIEW("GRAPHICS", "BITMAP32 SCALE",    []()
				{
					static std::vector<bitmap32> imageSet;
					static std::vector<std::string> files =
					{
						ASSET_DIR + "A.png",
						ASSET_DIR + "B.png",
						ASSET_DIR + "C.png",
						ASSET_DIR + "D.png",
						ASSET_DIR + "E.png",
						ASSET_DIR + "F.png",
					};

					MakeOnce
					{
						ForArray(i, files.size())
							imageSet.emplace_back(png_read_image_file(files[i]));
					}

					const std::vector<int> scale_options =
					{
						(int)art::scalefilter::nearest,
						(int)art::scalefilter::bilinear,
						//(int)art::scalefilter::stb_bilinear,
						//(int)art::scalefilter::stb_mitchell_netravali,
						//(int)art::scalefilter::stb_auto,
					};

					struct scalledKit
					{
						std::vector<bitmap32> items;
					};

					static std::vector<scalledKit> scalled_kits;
					static bool need_update = true;

					static INT2 scale_size = { 20, 20 };
					ImGui::InputInt2("Scale size", &scale_size.x);
					ImGui::SameLine();

					need_update |= ImGui::Button("UPDATE [SPACE]");
					need_update |= ImGui::IsWindowFocused() && Keyboard.WasPressed(KKey::KEY_SPACE);

					static std::map<const void*, GpuTexture> gpu_mapped;

					//------------------------ update
					if (need_update)
					{
						need_update = false;

						scalled_kits.clear();
						scalled_kits.resize(imageSet.size());

						gpu_mapped.clear();

						log::info << "______ bitmap scale time ______";

						ForArray(i, imageSet.size())
						{
							const auto& srcImage = imageSet[i];

							gpu_mapped[srcImage.raw_data()].Update(srcImage);

							ForArray(j, scale_options.size())
							{
								const art::scalefilter filter = static_cast<art::scalefilter>(scale_options[j]);

								scalled_kits[i].items.emplace_back(srcImage.get_copy_scaled(scale_size, filter));

								gpu_mapped[scalled_kits[i].items[j].raw_data()].Update(scalled_kits[i].items[j]);
							}

							//SuperTimer ST;
							//ST.SetStart();
							//ST.SetEnd();
							//log::info << str::format::insert("scale time : [{} {}] : {}",
							//	item.size().x,
							//	item.size().y,
							//	ST.GetTimeNano());
							//INT2 size = item.size() / 2;
							//INT2 posA = {0,0};
							//INT2 posB = item.size() - size;
							//
							//bitmap32_view viewA = bitmap32_view(item, posA, size);
							//bitmap32_view viewB = bitmap32_view(item, posB, size);
							//
							//bitmap32_view* viewArr[] = { &viewA, &viewB };
							//
							//ForArray(i, ARRAY_SIZE(viewArr))
							//ForArray(y, viewArr[i]->size().y)
							//ForArray(x, viewArr[i]->size().x)
							//{
							//	*viewArr[i]->view_data(x, y) = lerp(*viewArr[i]->view_data(x, y), color32{ 255,0,0,255 }, 0.5);
							//}

						}
					}

					//------------------------ draw
					ForArray(i, imageSet.size())
					{
						//const bitmap32& imgA = vecA[i];
						//bitmap32& imgB = vecB[i];
						//auto& resA = gpu_map[imgA.raw_data()];
						//const auto& resB = gpu_map[imgB.raw_data()];


						const FLOAT2 VIEW_SIZE(256., 256.);

						const auto& srcImg     = imageSet[i];
						      auto& srcImgView = gpu_mapped[srcImg.raw_data()];

						ImGui::Text(str::format::insert("image: {} [{} {}]", files[i].c_str(), srcImg.size().x, srcImg.size().y).c_str());


						ImItem::ShowImage(&srcImgView, true, VIEW_SIZE);
						ImGui::SameLine();

						ForArray(j, scale_options.size())
						{
							const auto& scalledItem     = scalled_kits[i].items[j];
							      auto& scalledItemView = gpu_mapped[scalledItem.raw_data()];

							ImItem::ShowImage(&scalledItemView, true, VIEW_SIZE);

							if (j + 1 != scale_options.size())
								ImGui::SameLine();
						}

						ImGui::Separator();
					}
				});
			IM_VIEW("GRAPHICS", "BITMAP32 SAND BOX", []() { imview::graphics_sandbox(); });

			IM_VIEW("GRAPHICS", "BITMAP32 triangle fill",  []()
				{
					enum STATE
					{
						FOR_ARRAY,
						FOR_ARRAY_IN_LAMDA,
						FOR_EACH_PIX_LAMDA,
						FOR_EACH_REG_LAMDA,
						THREAD_POOL,
						THREAD_POOL_ALIVE,
						OPEN_MP_2,
						OPEN_MP_4,
						OPEN_MP_6,
						OPEN_MP_8,
					};

					static int fill_state = FOR_ARRAY;

					static bool is_near_sampler = true;
					static int  thread_c = 4;

					enum { SIZE_WH = 2048, };

					static bitmap32 ram_bitmap;
					static GpuTexture gpu_bitmap;

					//static graphics::gpu_sampler sampler_point;
					//static graphics::gpu_sampler sampler_lenear;

					MakeOnce
					{
						ram_bitmap = bitmap32(SIZE_WH, SIZE_WH,{0,0,0,255});

						//sampler_point .Create(graphics::gpu_sampler::GetPointPreset());
						//sampler_lenear.Create(graphics::gpu_sampler::GetLinearPreset());
					}

						struct smart_point
					{
						smart_point()
						{
							pos = FLOAT2(math::Random(0., SIZE_WH), math::Random(0., SIZE_WH));
							speed = FLOAT2(math::Random(2., 5.), math::Random(2., 5.));

							speed = speed / 99 + FLOAT2{ (SIZE_WH / 200), (SIZE_WH / 200) };
						}

						FLOAT2 pos;
						FLOAT2 speed;

						bool hor_left = true;
						bool vert_top = true;

						void update()
						{
							if (pos.x < 0)		 hor_left = false;
							if (pos.x > SIZE_WH) hor_left = true;
							if (pos.y < 0)		 vert_top = false;
							if (pos.y > SIZE_WH) vert_top = true;

							pos.x += (hor_left == true) ? -speed.x : speed.x;
							pos.y += (vert_top == true) ? -speed.y : speed.y;
						}
					};

					static smart_point sp[4];

					//ForArray(i, 4)
					//	sp[i].update();

					MakeOnce
					{
					}

					SuperTimer ST;
					//=====================================================================================================//
					ram_bitmap.fill_by_color({ 0,0,0,255 });

					ST.SetStart();

					if ("fill")
					{
						sp[0].pos = { 0, 0 };
						sp[2].pos = { 0, SIZE_WH };
						sp[3].pos = { SIZE_WH, SIZE_WH };

						switch (fill_state)
						{
						case FOR_ARRAY://---------------------------------------------//


							TRG(ram_bitmap, false, sp[0].pos, sp[2].pos, sp[3].pos);
							//for (int y = 0; y < ram_bitmap.size().y; y++)
							//for (int x = 0; x < ram_bitmap.size().x; x++)
							//{
							//	if (math::is_point_in_triangle2D(FLOAT2(x, y), sp[0].pos, sp[2].pos, sp[3].pos))
							//		ram_bitmap.raw_data(x, y)->r = 255;
							//}

							break;
						case FOR_ARRAY_IN_LAMDA://---------------------------------------------//

							[]() {

								for (int y = 0; y < ram_bitmap.size().y; y++)
									for (int x = 0; x < ram_bitmap.size().x; x++)
									{
										if (math::is_point_in_triangle2D(FLOAT2(x, y), sp[0].pos, sp[2].pos, sp[3].pos))
											ram_bitmap.raw_data(x, y)->r = 255;
									}

							}();

							break;
						case FOR_EACH_PIX_LAMDA://---------------------------------------------//

							ram_bitmap.for_each([](color32& p, const INT2& pos)
								{
									if (math::is_point_in_triangle2D(FLOAT2(pos.x, pos.y), sp[0].pos, sp[2].pos, sp[3].pos))
										p.r = 255;

								}, thread_c);

							break;
						case FOR_EACH_REG_LAMDA://---------------------------------------------//

							ram_bitmap.sub_region([](const INT2& pos, const INT2& end)
								{
									int r = std::hash<std::thread::id>()(std::this_thread::get_id()) / 40 % 255;
									int g = std::hash<std::thread::id>()(std::this_thread::get_id()) / 60 % 255;
									int b = std::hash<std::thread::id>()(std::this_thread::get_id()) / 20 % 255;

									ForArrayFrom(y, pos.y, end.y)
										ForArrayFrom(x, pos.x, end.x)
									{
										ram_bitmap.raw_data(x, y)->r = r;
										ram_bitmap.raw_data(x, y)->g = g;
										ram_bitmap.raw_data(x, y)->b = b;
									}

								}, thread_c);

							break;
						case THREAD_POOL:
							break;
						case THREAD_POOL_ALIVE:

							struct thread_pool
							{
								enum
								{
									COUNT = 4,
								};

								std::thread           pool[COUNT];
								std::function<void()> jobs[COUNT];

								thread_pool()
								{
									ForArray(i, COUNT)
										pool[i] = std::thread(&thread_pool::worker, this);
								}

								atomic<int> busy_workers = 0;
								atomic<int> jobs_count = 0;
								std::mutex mut;

								void worker()
								{
									while (true)
									{
										if (jobs_count > 0)
										{
											int n = jobs_count;

											if (mut.try_lock())
											{
												jobs_count--;
												busy_workers++;

												auto call = jobs[n - 1];
												mut.unlock();

												call();

												jobs[n - 1] = nullptr;
												busy_workers--;
											}
										}
										else
										{
											std::this_thread::sleep_for(std::chrono::milliseconds(2));
										}
									}
								}

								void push_job(std::function<void()> func)
								{
									func();

									//jobs[jobs_count] = func;
									//jobs_count++;
								}

								void join()
								{
									while (busy_workers > 0)
										std::this_thread::sleep_for(std::chrono::milliseconds(2));
								}
							};

							{
								static thread_pool pool;

								pool.push_job([]()
									{
										for (int y = 0; y < ram_bitmap.size().y; y++)
											for (int x = 0; x < ram_bitmap.size().x; x++)
											{
												if (math::is_point_in_triangle2D(FLOAT2(x, y), sp[0].pos, sp[2].pos, sp[3].pos))
													ram_bitmap.raw_data(x, y)->r = 255;
											}
									});
								pool.join();
							}

							break;
						case OPEN_MP_2://---------------------------------------------//

#pragma omp parallel for num_threads(2)
							for (int y = 0; y < ram_bitmap.size().y; y++)
								for (int x = 0; x < ram_bitmap.size().x; x++)
								{
									if (math::is_point_in_triangle2D(FLOAT2(x, y), sp[0].pos, sp[2].pos, sp[3].pos))
										ram_bitmap.raw_data(x, y)->r = 255;
								}

							break;
						case OPEN_MP_4://---------------------------------------------//

#pragma omp parallel for num_threads(4)
							for (int y = 0; y < ram_bitmap.size().y; y++)
								for (int x = 0; x < ram_bitmap.size().x; x++)
								{
									if (math::is_point_in_triangle2D(FLOAT2(x, y), sp[0].pos, sp[2].pos, sp[3].pos))
										ram_bitmap.raw_data(x, y)->r = 255;
								}

							break;
						case OPEN_MP_6://---------------------------------------------//

#pragma omp parallel for num_threads(6)
							for (int y = 0; y < ram_bitmap.size().y; y++)
								for (int x = 0; x < ram_bitmap.size().x; x++)
								{
									if (math::is_point_in_triangle2D(FLOAT2(x, y), sp[0].pos, sp[2].pos, sp[3].pos))
										ram_bitmap.raw_data(x, y)->r = 255;
								}

							break;
						case OPEN_MP_8://---------------------------------------------//

#pragma omp parallel for num_threads(8)
							for (int y = 0; y < ram_bitmap.size().y; y++)
								for (int x = 0; x < ram_bitmap.size().x; x++)
								{
									if (math::is_point_in_triangle2D(FLOAT2(x, y), sp[0].pos, sp[2].pos, sp[3].pos))
										ram_bitmap.raw_data(x, y)->r = 255;
								}

							break;
						default:
							break;
						}

					}

					ST.SetEnd();
					//=====================================================================================================//
					gpu_bitmap.Update(ram_bitmap);

					if (is_near_sampler)
						gpu_bitmap.SetSampler(SamplerState::GetPresetPoint());
					else
						gpu_bitmap.SetSampler(SamplerState::GetPresetLinear());

					enum FOP_CNT
					{
						FCNT = 24,
					};


					float time = ST.GetTimeReal();
					static vector<float> times(20);

					if ("counter")
					{
						memcpy(&times[0], &times[1], sizeof(float)*times.size() - 1);
						times[times.size() - 1] = time;
						time = 0;
						ForArray(i, times.size())
							time += times[i];
						time = time / (times.size() - 1);
					}



					float cv_time = time;
					float cv_gflops = (999. / time) * (4 * SIZE_WH * SIZE_WH) / 999'999'999;

					//=====================================================================================================//

					ImGui::RadioButton("FOR ARRAY         ", &fill_state, FOR_ARRAY);
					ImGui::RadioButton("FOR ARRAY (LAMDA) ", &fill_state, FOR_ARRAY_IN_LAMDA);
					ImGui::RadioButton("FOR EACH  (PIXEL) ", &fill_state, FOR_EACH_PIX_LAMDA);
					ImGui::RadioButton("FOR EACH  (REGION)", &fill_state, FOR_EACH_REG_LAMDA);
					ImGui::RadioButton("THREADS           ", &fill_state, THREAD_POOL);
					ImGui::RadioButton("THREADS (ALIVE)   ", &fill_state, THREAD_POOL_ALIVE);
					ImGui::RadioButton("OPEN MP (x2)      ", &fill_state, OPEN_MP_2);
					ImGui::RadioButton("OPEN MP (x4)      ", &fill_state, OPEN_MP_4);
					ImGui::RadioButton("OPEN MP (x6)      ", &fill_state, OPEN_MP_6);
					ImGui::RadioButton("OPEN MP (x8)      ", &fill_state, OPEN_MP_8);

					ImGui::Checkbox("TxFilter [Near/Lenear]", &is_near_sampler);
					ImGui::InputInt("ThCount", &thread_c);

					ImGui::Text(str::format::insert("Fill time  : {}", cv_time).c_str());
					ImGui::Text(str::format::insert("Perfomance : {}", cv_gflops).c_str());

					ImGui::Separator();
					ImGui::Image((ImTextureID)gpu_bitmap.GetImpl()->uid, ImGui::GetContentRegionAvail());

				});
			IM_VIEW("GRAPHICS", "BITMAP32 buckgrnd task",  []()
				{
					static bitmap32 bitmap(600, 600, { 255,0,0,255 });

					static bool buckgrownd_active = false;
					ImGui::Checkbox("buckgrownd task", &buckgrownd_active);

					static std::atomic<bool> ready = false;

					if (!buckgrownd_active)
					{
						for (size_t i = 0; i < bitmap.pixel_count(); i++)
							bitmap.raw_data(i, 0)->r = math::xorshiftf32() % 256;
					}
					else
					{
						static std::thread loop([&]()
							{
								while (true)
								{
									if (!ready)
									{
										for (size_t i = 0; i < bitmap.pixel_count(); i++)
											bitmap.raw_data(i, 0)->r = math::xorshiftf32() % 256;
										ready = true;
									}
									else
										std::this_thread::sleep_for(std::chrono::microseconds(20));
								}
							});
					}

					static GpuTexture gpu_t;
					gpu_t.Update(bitmap);
					ready = false;
					ImGui::Image((void*)gpu_t.GetImpl()->uid, { 600,600 });
				});
			IM_VIEW("GRAPHICS", "BITMAP32 BitmapTest <-> String", []()
			{
				static bitmap32 image(8, 8);
				static bool read_only = false;
				static char text[2048 * 2048] = "";

				static std::string str_buff;

				if (ImGui::Button("Randomize"))
				{
					ForArray(i, image.data_size())
						((uint8_t*)image.raw_data(0, 0))[i] = math::Random(0, 255);
				}

				if (ImGui::Button("to str"))
				{
					str_buff.clear();
					str_buff = bitmap_converter::bitmap_to_string(image);
					std::memcpy(&text[0], &str_buff[0], str_buff.size());
				}

				if (ImGui::Button("from str"))
				{
					std::memcpy(&str_buff[0], &text[0], image.pixel_count() * 8 + image.size().y);
					image = bitmap_converter::string_to_bitmap({8,8}, str_buff);
				}

				if (ImGui::Button("FromClipboard"))
				{
					auto result = os_api::clipboard::GetClipboardImage();

					if (result.IsSucceed())
						image = result.GetValue();
				}

				ImGui::Checkbox("Read-only", &read_only);
				ImGuiInputTextFlags flags = ImGuiInputTextFlags_AllowTabInput | (read_only ? ImGuiInputTextFlags_ReadOnly : 0);
				ImGui::InputTextMultiline("##source", text, IM_ARRAYSIZE(text), ImVec2(-1.0f, ImGui::GetTextLineHeight() * 16), flags);

				static GpuTexture gpu_img;
				gpu_img.Update(image);
				ImGui::Image((ImTextureID)gpu_img.GetImpl()->uid, { 512,512 });
			});
			IM_VIEW("GRAPHICS", "BITMAP32 DRAW TEXT", []()
				{
					const unsigned LINE_ABC = 0;
					const unsigned LINE_abc = 1;
					const unsigned LINE_num = 2;
					const unsigned LINE_sum = 3;
					const unsigned LINE_alt = 4;

					const  INT2 LIT_SIZE = {7, 13};
					const  INT2 LIT_UNCNOWN_CHAR_POS = INT2(0, LIT_SIZE.y * LINE_alt);
					static INT2 LIT_MAP[256];

					MakeOnce
					{
						const std::string c_ABC = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
						const std::string c_abc = "abcdefghijklmnopqrstuvwxyz";
						const std::string c_NUM = "1234567890";
						const std::string c_SUM = "{}[]()<>$*-+=/#_%^@\\&|~?`'\" !,.;:";
						const std::string c_ALT = "[?]"; // not used

						{ ForArray(i, 256)          LIT_MAP[(uint8_t)i]        = LIT_UNCNOWN_CHAR_POS; }
						{ ForArray(i, c_ABC.size()) LIT_MAP[(uint8_t)c_ABC[i]] = { LIT_SIZE.x * i, LIT_SIZE.y * LINE_ABC }; }
						{ ForArray(i, c_abc.size()) LIT_MAP[(uint8_t)c_abc[i]] = { LIT_SIZE.x * i, LIT_SIZE.y * LINE_abc }; }
						{ ForArray(i, c_NUM.size()) LIT_MAP[(uint8_t)c_NUM[i]] = { LIT_SIZE.x * i, LIT_SIZE.y * LINE_num }; }
						{ ForArray(i, c_SUM.size()) LIT_MAP[(uint8_t)c_SUM[i]] = { LIT_SIZE.x * i, LIT_SIZE.y * LINE_sum }; }
					}

					static GpuTexture gpu_atlas;
					static GpuTexture gpu_paper;

					static bitmap32 atlas = art::draw::chess_board(INT2{ 256,64 }, 8);
					static bitmap32 paper(2048, 16384, color32{ 255, 255, 255, 255 });

					MakeOnce
					{
						gpu_atlas.Update(atlas);
						gpu_paper.Update(paper);
					}

					//----------------// info

					ImGui::Text("FAQ");
					ImGui::Text(" - draws text in bitmap using literal atlas");
					ImGui::Text(" - supported only ASCII encode");

					//----------------// gpu atlas

					ImGui::Separator();
					ImGui::Text("Insert atlas here:");
					bool atlas_changed = ImItem::ShowImage(&gpu_atlas);

					ImGui::SameLine();
					if (ImGui::Button("Load default font"))
					{
						const INT2 size  = { IMG_BITMAP_FONT_SIZE_W, IMG_BITMAP_FONT_SIZE_H };
						const char* pStr = IMG_BITMAP_FONT_DATA;

						art::bitmap32 img(size);

						ForArray(y, size.y)
						ForArray(x, size.x)
							*img.raw_data(x, y) = pStr[x + y * size.x] == ' '
								? color32(255, 255, 255, 255) : color32(0, 0, 0, 255);

						atlas_changed = true;
						gpu_atlas.Update(img);
					}

					//----------------// rich text box

					ImGui::Separator();
					ImGui::Text("Add text here:");
					static char buffer[256 * 8000];
					bool text_changed = ImGui::InputTextMultiline("Rich text box", buffer, ARRAY_SIZE(buffer), { -1, 200 }, ImGuiInputTextFlags_AllowTabInput);

					//----------------// redraw paper

					static color::color32 ColorPaper = { 255, 255, 255, 255 };
					static color::color32 ColorText  = { 0,   0,   0,   255 };

					bool color_changed = false;

					{
						static FLOAT4 buck_col = { 0.999f, 0, 0, 0.999f };
						static FLOAT4 text_col = { 0,      0, 0, 0.999f };

						color_changed |= ImGui::ColorEdit4("Paper color", &buck_col.x, ImGuiColorEditFlags_Uint8);
						color_changed |= ImGui::ColorEdit4("Text  color", &text_col.x, ImGuiColorEditFlags_Uint8);

						if (color_changed)
						{
							ColorPaper = color::to_color32(buck_col);
							ColorText  = color::to_color32(text_col);
						}
					}

					static std::vector<INT2> map_points;
					static std::vector<INT2> map_offset;

					if (atlas_changed)
					{
						map_points.clear();
						map_offset = std::vector<INT2>(256, { 0,0 });

						gpu_atlas.GetData(atlas);

						size_t cur = 0;

						ForArray(i, 256)
						{
							map_offset[i].x = cur;

							const uint8_t ch_id  = uint8_t(i);
							const INT2    ch_pos = LIT_MAP[ch_id];

							ForArray(y, LIT_SIZE.y)
							ForArray(x, LIT_SIZE.x)
							{
								const INT2    offset = INT2(x, y);
								const INT2    point  = ch_pos + offset;
								const color32 pixel  = *atlas.raw_data(point.x, point.y);

								if (pixel == color32{ 0,0,0,255 })
								{
									map_points.push_back(offset);
									cur++;
								}
							}

							map_offset[i].y = cur;
						}
					}

					if (atlas_changed || text_changed || color_changed)
					{
						const std::string str = std::string(buffer);
						const INT2 CURSOR_POS_START = { 20,20 };

						INT2 CURSOR_POS = CURSOR_POS_START;

						paper.fill_by_color(ColorPaper);

						//------------------------------ draw rect
						{
							const FLOAT2 A = { 8.5f, 8.5f };
							const FLOAT2 B = { paper.size().x - 8.5f, 8.5f };
							const FLOAT2 C = { paper.size().x - 8.5f, paper.size().y - 8.5f };
							const FLOAT2 D = { 8.5f, paper.size().y - 8.5f };
							const color32 color = ColorText;

							art::draw::drawline2D(paper, A, B, color, color);
							art::draw::drawline2D(paper, A, D, color, color);
							art::draw::drawline2D(paper, C, B, color, color);
							art::draw::drawline2D(paper, C, D, color, color);
						}

						//------------------------------ draw each char
						ForArray(i, str.size())
						{
							char c = str[i];

							if (c == '\n')
							{
								CURSOR_POS.x  = CURSOR_POS_START.x;
								CURSOR_POS.y += LIT_SIZE.y;
								continue;
							}
							if (c == '\t')
							{
								const int off = CURSOR_POS_START.x;

								CURSOR_POS.x += off + 20 - ((CURSOR_POS.x - off) % 20);
								continue;
							}

							//-------------------------------------------------------------  memcpy
							if (false)
							{
								INT2 lit_pos = LIT_MAP[(uint8_t)c];

								ForArray(y, LIT_SIZE.y)
								{
									if (paper.is_rect_in(CURSOR_POS, LIT_SIZE))
									{
										auto src = atlas.raw_data(lit_pos.x,    lit_pos.y    + y);
										auto out = paper.raw_data(CURSOR_POS.x, CURSOR_POS.y + y);

										std::memcpy(out, src, LIT_SIZE.x * sizeof(color32));
									}
								}
							}
							//-------------------------------------------------------------  for each
							if (true)
							{
								//INT2 lit_pos = LIT_MAP[(uint8_t)c];
								const size_t ch_id = uint8_t(c);
								const size_t i_beg = map_offset[ch_id].x;
								const size_t i_end = map_offset[ch_id].y;

								if (paper.is_rect_in(CURSOR_POS, LIT_SIZE))
								{
									for (size_t i = i_beg; i < i_end; i++)
									{
										const INT2 target = CURSOR_POS + map_points[i];

										*paper.raw_data(target.x, target.y) = ColorText;

										//auto src = atlas.raw_data(lit_pos.x, lit_pos.y + y);
										//auto out = paper.raw_data(CURSOR_POS.x, CURSOR_POS.y + y);
										//std::memcpy(out, src, LIT_SIZE.x * sizeof(color32));
									}
								}
							}
							//-------------------------------------------------------------

							CURSOR_POS.x += LIT_SIZE.x;
						}

						gpu_paper.Update(paper);
					}

					//----------------// show paper

					ImGui::Separator();
					ImGui::Text("Output:");
					ImItem::ShowImage(&gpu_paper);
				});
			IM_VIEW("GRAPHICS", "BITMAP32 SoftCam", []()
			{
				const auto imCanvasPos = ImGui::GetCursorPos() + ImGui::GetWindowPos();
				const auto imDrawList  = ImGui::GetWindowDrawList();
				const FLOAT2 canvasPos = ImGui::GetCurrentWindow()->Pos + ImGui::GetCursorPos();
				const FLOAT2 canvasSize = { 360, 280 };

				//-----------------------------------------------------------//
 				struct CamState
				{
					FLOAT3 pos_point  = { 6.0, 6.0, 6.0 };
					FLOAT3 look_point = { 0.0, 0.0, 0.0 };
				};
				struct Object
				{
					FLOAT3 pos  = { 0, 0, 0 };
					FLOAT3 root = { 0, 0, 0 };
					FLOAT3 size = { 1, 1, 1 };
				};

				//-----------------------------------------------------------//
				static CamState cam;
				static Object obj;

				static bool auto_rotate = false;
				static bool color_tex = false;

				if (auto_rotate)
					obj.root += {0, 0.2, 0.2};

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


				static bool opt_draw = false;
				ImGui::Checkbox("opt_draw", &opt_draw);

				//obj.Root.x += 360;
				//obj.Root.y += 360;
				//obj.Root.z += 360;

				glm::mat4 Proj  = matProjection(aspect, canvasSize.x / canvasSize.y, 0.09, 99.);
				glm::mat4 View  = matView(cam.pos_point, cam.look_point);
				glm::mat4 Model = matModel(obj.pos, obj.root, obj.size);

				glm::mat4 WVP = Proj * View * Model;

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

					if(aa.w < 0 && bb.w <0 && cc.w < 0)
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

					if (!opt_draw)
					{
						if (color_tex) draw::drawTriangle3D(ram_bitmap, &depth_buff, a, b, c, va.col, vb.col, vc.col);
						else           draw::drawTriangle3D(ram_bitmap, &depth_buff, a, b, c, va.tex, vb.tex, vc.tex);
					}
					else
					{
						if (color_tex) draw::drawTriangle3D_opt(ram_bitmap, &depth_buff, a, b, c, va.col, vb.col, vc.col);
						else           draw::drawTriangle3D_opt(ram_bitmap, &depth_buff, a, b, c, va.tex, vb.tex, vc.tex);
					}
				}
			});

			//-------------------------------------------------------// OVERLAY
			IM_VIEW("OVERLAY", "CurSlime", [&]()
				{
					ImGui::Text("MouseSlime");
					ImGui::Separator();
					{
						static bool  slime_show = false;
						static float slime_cc_dim = 0.45;
						static float slime_move_dim = 0.45;
						static float slime_rounding = 5.0;
						static int   slime_upd_ps = 200;
						static int   segment_count = 30;
						static int   segment_max_rad = 30;
						static int   segment_min_rad = 9;
						static bool  segment_rebuild = true;

						ImGui::Checkbox   ("Show",            &slime_show);
						ImGui::SliderFloat("Interpolate dim", &slime_cc_dim,    0, 0.999);
						ImGui::SliderFloat("Rad translation", &slime_move_dim,  0, 0.999);
						ImGui::SliderFloat("Rounding splash", &slime_rounding,  0, 20);
						ImGui::SliderInt  ("Updates per/s.",  &slime_upd_ps,    20, 200);
						ImGui::SliderInt  ("Seg count",       &segment_count,   2, 50);
						ImGui::SliderInt  ("Seg max",         &segment_max_rad, 0, 90);
						ImGui::SliderInt  ("Seg min",         &segment_min_rad, 0, 90);

						struct circ
						{
							FLOAT2 pos;
							float rad;
						};
						static std::vector<circ> path;

						if (segment_rebuild)
						{
							path.clear();
							ForArray(i, segment_count)
								path.push_back({ {0,0}, lerp(segment_max_rad, segment_min_rad, float(i) / (segment_count - 0.999)) });
						}

						segment_rebuild =
							ImGui::Button("Rebuild segments");

						if (slime_show && Keyboard.IsPressed(KKey::KEY_ALT))
						{
							static SuperTimer st;
							static float time_out = 0;
							time_out += st.GetRoundTime();
							if (time_out >= 0.999 / slime_upd_ps)
							{
								time_out = 0;

								FLOAT2 mpos = { float(Mouse.GetLocalPos().x), float(Mouse.GetLocalPos().y) };
								ForArray(i, path.size())
								{
									auto* a = &path[i].pos;
									auto* b = &mpos;

									if (i > 0)
										b = &path[i - 2 / 2].pos;

									auto new_a = lerp(*a, *b, slime_cc_dim);

									const float CC_DIM = 0.66;

									if (math::distance_2d(new_a, *b) > path[i].rad * slime_move_dim)
										new_a = math::move_on_angle(*b, math::anlage_2d(*b, *a), path[i].rad * slime_move_dim);

									*a = new_a;

								}

							}

							ForArray(i, path.size())
								ImGui::GetForegroundDrawList()->AddCircleFilled(path[i].pos, path[i].rad + slime_rounding, 0xFFFFFFFF, 20);

							ForArray(i, path.size())
								ImGui::GetForegroundDrawList()->AddCircleFilled(path[i].pos, path[i].rad, 0xFF000000, 20);
						}
					}
					//////////////////////////////////////////////////////////////////////////
					ImGui::Text("Curve");
					ImGui::Separator();
					imview::bezierCurve();
				});
			IM_VIEW("OVERLAY", "DESKROOT", [&]()
				{
					static bitmap32 ram_img;
					static GpuTexture gpu_img;

					static bool in_enabled = false;

					MakeOnce
					{
						gpu_img.SetSampler(SamplerState::GetPresetLinear());
					}

					//////////////////////////////////////////////////////////////////////////

					ImGui::Checkbox("Enable [HOTKEY: P]", &in_enabled);

					if (Keyboard.WasPressed(KKey::KEY_P))
						in_enabled = !in_enabled;

					if (ImGui::Button("UpdateGrab") || ram_img.size() == INT2(0,0))
					{
						static ScreenGrabber sg;

						sg.grubScreen();
						ram_img = sg.captured_bitmap.get_copy();

						for (size_t i = 0; i < ram_img.pixel_count(); i++)
							std::swap(ram_img.raw_data(i)->r, ram_img.raw_data(i)->b);

						gpu_img.Update(ram_img);
					}

					if (in_enabled)
					{
						const float w = (float)myRS.RenderCanvasW; // os_api::get_system_metric(os_system_metric::MAIN_MONITOR_W).GetValue();
						const float h = (float)myRS.RenderCanvasH;//os_api::get_system_metric(os_system_metric::MAIN_MONITOR_H).GetValue();

						const FLOAT2 scr_size = FLOAT2{ w, h }; // -h to flip horisontaly
						const FLOAT2 scr_mid  = scr_size / 2.;

						const float aspect_angle = math::anlage_2d(scr_mid, { scr_size.x, 0 });
						const float aspect_dist  = math::distance_2d(scr_mid, scr_size);

						static SuperTimer st;
						static double time_exp = 0;
						time_exp += st.GetRoundTime();
 						const float cps = 0.99 / 200; // 200 per sec

						static float rootation = 0;

						if (time_exp > cps)
						{
							time_exp = 0;
							rootation += 0.06;
						}

						FLOAT2 vert[] =
						{
							math::move_on_angle(scr_mid,      0. - aspect_angle + rootation, aspect_dist),
							math::move_on_angle(scr_mid,      0. + aspect_angle + rootation, aspect_dist),
							math::move_on_angle(scr_mid, 2 * 90. + aspect_angle + rootation, aspect_dist),
							math::move_on_angle(scr_mid, 2 * 90. - aspect_angle + rootation, aspect_dist),
						};

						ImGui::GetForegroundDrawList()->AddRectFilled({ 0, 0 }, scr_size, IM_COL32(0,0,0,255));
						ImGui::GetForegroundDrawList()->AddImageQuad(
							(ImTextureID)gpu_img.GetImpl()->uid,
							vert[0/2],
							vert[2/2],
							vert[3],
							vert[2]
						);
					}
				});
			IM_VIEW("OVERLAY", "GRADIENT", [&]()
				{
					const float screen_w = (float)myRS.RenderCanvasW;
					const float screen_h = (float)myRS.RenderCanvasH;

					static FLOAT4 CTOP = { 200.f/255.f, 0,  60.f/255.f, 255.f/255.f };
					static FLOAT4 CBOT = {  60.f/255.f, 0, 200.f/255.f, 255.f/255.f };

					struct fl_color
					{
						FLOAT4 val;
						FLOAT4 trg;
					};

					static fl_color A{CTOP, CTOP};
					static fl_color B{CTOP, CTOP};
					static fl_color C{CBOT, CBOT};
					static fl_color D{CBOT, CBOT};

					static const auto get_random_target = [](FLOAT4 col)
					{
						return FLOAT4
						{
							col.x + (float)math::RandomStdReal(-0.090f, 0.090f),
							col.y + (float)math::RandomStdReal( 0.000f, 0.200f),
							col.z + (float)math::RandomStdReal(-0.090f, 0.090f),
							col.w = (float)math::RandomStdReal( 0.000f, 0.999f),
						};
					};

					static SuperTimer ST;
					static float time_exp = 0;

					time_exp += ST.GetRoundTime();

					if (time_exp > 0.999 / 200)
					{
						time_exp = 0;

						if (math::equal(A.val, A.trg, 0.009f)) A.trg = get_random_target(CTOP);
						if (math::equal(B.val, B.trg, 0.009f)) B.trg = get_random_target(CTOP);
						if (math::equal(C.val, C.trg, 0.009f)) C.trg = get_random_target(CBOT);
						if (math::equal(D.val, D.trg, 0.009f)) D.trg = get_random_target(CBOT);

						A.val = lerp(A.val, A.trg, 0.003f);
						B.val = lerp(B.val, B.trg, 0.003f);
						C.val = lerp(C.val, C.trg, 0.003f);
						D.val = lerp(D.val, D.trg, 0.003f);
					}

					ImGui::ColorEdit4("valA", &A.val.x);
					ImGui::ColorEdit4("valB", &B.val.x);
					ImGui::ColorEdit4("valC", &C.val.x);
					ImGui::ColorEdit4("valD", &D.val.x);

					ImGui::ColorEdit4("trgA", &A.trg.x);
					ImGui::ColorEdit4("trgB", &B.trg.x);
					ImGui::ColorEdit4("trgC", &C.trg.x);
					ImGui::ColorEdit4("trgD", &D.trg.x);

					auto TopL = IM_COL32(A.val.x * 255.f, A.val.y * 255.f, A.val.z * 255.f, A.val.w * 255.f);
					auto TopR = IM_COL32(B.val.x * 255.f, B.val.y * 255.f, B.val.z * 255.f, B.val.w * 255.f);
					auto BotL = IM_COL32(C.val.x * 255.f, C.val.y * 255.f, C.val.z * 255.f, C.val.w * 255.f);
					auto BotR = IM_COL32(D.val.x * 255.f, D.val.y * 255.f, D.val.z * 255.f, D.val.w * 255.f);

					// auto topC = ClearColor * 255.f;
					// auto botC = FLOAT4{255.f, 255.f, 255.f, 255.f} - ClearColor * 255.f;
					// auto topCol = IM_COL32(topC.x, topC.y, topC.z, 255);
					// auto botCol = IM_COL32(botC.x, botC.y, botC.z, 255);
					// topCol = IM_COL32(200, 0,  60, 255);
					// botCol = IM_COL32( 60, 0, 255, 255);

					ImGui::GetBackgroundDrawList()->AddRectFilledMultiColor(
						{0, 0},
						{screen_w, screen_h},
						TopL,
						TopR,
						BotR,
						BotL
					);
				});
			//-------------------------------------------------------// INPUT
			IM_VIEW("INPUT", "key stick test", []()
				{
					ImItem::Text("LEFT    Is Pressed  | {}", Mouse.IsPressed(MKey::MOUSE_LEFT));
					ImItem::Text("RIGHT   Is Pressed  | {}", Mouse.IsPressed(MKey::MOUSE_RIGHT));
					ImItem::Text("MIDDLE  Is Pressed  | {}", Mouse.IsPressed(MKey::MOUSE_MIDDLE));
					ImItem::Text("FORWARD Is Pressed  | {}", Mouse.IsPressed(MKey::MOUSE_FORWARD));
					ImItem::Text("BUCK    Is Pressed  | {}", Mouse.IsPressed(MKey::MOUSE_BUCK));
					ImItem::Text("LEFT    Was Pressed | {}", Mouse.WasPressed(MKey::MOUSE_LEFT));
					ImItem::Text("RIGHT   Was Pressed | {}", Mouse.WasPressed(MKey::MOUSE_RIGHT));
					ImItem::Text("MIDDLE  Was Pressed | {}", Mouse.WasPressed(MKey::MOUSE_MIDDLE));
					ImItem::Text("FORWARD Was Pressed | {}", Mouse.WasPressed(MKey::MOUSE_FORWARD));
					ImItem::Text("BUCK    Was Pressed | {}", Mouse.WasPressed(MKey::MOUSE_BUCK));

					ImGui::Checkbox("Alt   ", &Keyboard.Switchers.KeyAlt);
					ImGui::Checkbox("Crtl  ", &Keyboard.Switchers.KeyCtrl);
					ImGui::Checkbox("Shift ", &Keyboard.Switchers.KeyShift);
					ImGui::Checkbox("Win   ", &Keyboard.Switchers.KeySyper);

					static std::string buffer = "";
					static std::size_t m_len = 512 * 512;

					ForArrayFrom(i, 1, 6)
					{
						if (Mouse.WasPressed (MKey(i))) buffer += "was pressed  " + std::to_string(i) + "\n";
						if (Mouse.WasReleased(MKey(i))) buffer += "was released " + std::to_string(i) + "\n";
					}

					if (Mouse.WasWheel())
						buffer += "was weel " + std::to_string(Mouse.GetWeel().x) + "\n";
					//-------------------------------------------------------------------------------------------------------------------//

					if (Keyboard.HasEvent())
					{
						const auto eventsList = Keyboard.GetEvents();

						for (const auto& event : eventsList)
						{
							if (Keyboard.Switchers.KeyAlt)	break;
							if (Keyboard.Switchers.KeyCtrl) break;

							if (event.is_press)
							{
								const std::string is_press = event.is_press ? "press" : "release";
								const std::string is_stick = event.is_stick ? "stick" : "";
								const std::string key_id   = std::to_string((int)event.key_vk);
								      std::string unicode  = str::convert::UnicodeToUTF8(event.key_wk);

								if (unicode[0] == '\0')
									unicode = "\\0";

								 buffer += str::format::insert("id({}) key({}) - {} {}\n", key_id, unicode, is_press, is_stick);
							}
						}
					}


					//-------------------------------------------------------------------------------------------------------------------//
					if (ImGui::Button("Clear"))
					{
						buffer.clear();
					}

					ImGuiInputTextFlags flags = ImGuiInputTextFlags_AllowTabInput | ImGuiInputTextFlags_ReadOnly;
					ImGui::InputTextMultiline("##source", (char*)buffer.c_str(), m_len, ImVec2(-1.0f, ImGui::GetContentRegionAvail().y), flags);

				});
			IM_VIEW("INPUT", "keyboard keys",  []()
				{
					ImGuiIO&		io = ImGui::GetIO();
					ImGuiContext*	context = ImGui::GetCurrentContext();
					ImGuiWindow*	window = context->CurrentWindow;
					ImDrawList*		drawer = window->DrawList;

					FLOAT2 c_pos = ImGui::GetCursorPos();
					FLOAT2 c_avl = ImGui::GetContentRegionAvail();

					static ImItem::Grid grid;

					FLOAT2 c_size;
					//c_size.x = 20 * 80;
					//c_size.y = 6  * 80;
					//c_pos = (c_avl - c_size) / 2;
					c_size = c_avl;

					grid.InitRegion(FLOAT4{ c_pos.x, c_pos.y, c_size.x, c_size.y }, 21, 6);

					struct v_key
					{
						v_key()
						{}

						v_key(const char* _str, INT2 _size = { 1,1 })
						{
							*this = v_key(std::string(_str), _size);
						}

						v_key(std::string _str, INT2 _size = { 1,1 })
						{
							if (_str == "")
								disabled = true;

							str = _str;
							size = _size;
						}

						bool disabled = false;
						std::string str = "";
						INT2 size = { 1, 1 };



					};

					static std::vector<v_key> keys;

					MakeOnce
					{
						auto l1 = vector<v_key>{ "ESC", "F1", "F2", "F3", "F4", "F5", "F6", "F7", "F8", "F9", "F10", "F11", "F12", "","PRT\nSCR", "SCR\nLCK", "PUS\nBRK", "","", "", "" };
						auto l2 = vector<v_key>{ "~",  "1", "2", "3", "4", "5", "6", "7", "8", "9", "0", "-", "+", "<-","INS", "HOME", "UP", "NUM\nLOCK", "/", "*", "-" };
						auto l3 = vector<v_key>{ "TAB", "Q", "W", "E", "R", "T", "Y", "U", "I", "O", "P", "{", "}", {"ENTER", {1,2}}, "DEL", "END", "DOWN", "7", "8", "9", {"+",{1,2}} };
						auto l4 = vector<v_key>{ "CAPS", "A", "S", "D", "F", "G", "H", "J", "K", "L", ";", "\"", "\\", "", "", "","", "4", "5", "6", ""};
						auto l5 = vector<v_key>{ {"SHIFT",{2,1}}, "", "Z", "X", "C", "V", "B", "N", "M", "<", ">", "?", {"SHIFT",{2,1}}, "","", "↑", "", "1", "2", "3", {"ENT",{1,2}}};
						auto l6 = vector<v_key>{ "CTR", "WIN", "ALT", {" ",{8,1}},"","","","","","","", "ALT", "FN", "CTRL", "←", "↓", "→", {"0",{2,1}},"", ".", "" };

						keys.insert(keys.end(), l1.begin(), l1.end());
						keys.insert(keys.end(), l2.begin(), l2.end());
						keys.insert(keys.end(), l3.begin(), l3.end());
						keys.insert(keys.end(), l4.begin(), l4.end());
						keys.insert(keys.end(), l5.begin(), l5.end());
						keys.insert(keys.end(), l6.begin(), l6.end());
					}

					static std::unordered_map<std::string, bool> map;
					const auto eventsList = Keyboard.GetEvents();

					for (const auto e : eventsList)
					{
						map[str::convert::UnicodeToUTF8(e.key_wk)] = e.is_press;
					}


					ForArray(ix, grid.hor_colums_num)
					ForArray(iy, grid.ver_colums_num)
					{

						v_key key = keys[iy * 21 + ix];

						if (key.str != "")
						{
							FLOAT2 pos;
							FLOAT2 size;

							if (key.size == INT2{ 1,1 })
							{
								pos = grid.GetRegionPos(ix, iy, { 4, 4 });
								size = grid.GetRegionSize(ix, iy, { 4, 4 });
							}
							else
							{
								pos = grid.GetRegionPos(ix, iy, { 4, 4 });
								size = grid.GetRegionSize(ix, iy, { 4, 4 });

								size.x = (size.x + 8) * key.size.x - 8;
								size.y = (size.y + 8) * key.size.y - 8;
							}

							ImGui::SetCursorPos(pos);


							if (map[str::convert::cp1251_to_utf8(key.str)] == true)
							{
								ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_Button, FLOAT4(0.99, 0.2, .2, 0.99));

								ImGui::Button(key.str.c_str(), size);

								ImGui::PopStyleColor();
							}
							else
								ImGui::Button(key.str.c_str(), size);
						}
					}

				});
			//-------------------------------------------------------// win32
			IM_VIEW("win32", "win32-lawered", [&]()
				{
					ImGui::InputInt("Win32 errors", &mWin32errCount);

					static bool lawered = false;
					if (ImGui::Button("Tranc(222,222,222)"))
					{
						lawered = !lawered;

						const auto w_hwnd  = my_window->GetWindowId().hwnd;
						      auto w_stile = GetWindowLong(w_hwnd, GWL_EXSTYLE);

						if (lawered) w_stile = w_stile | ( WS_EX_LAYERED);
						else         w_stile = w_stile & (~WS_EX_LAYERED);

						SetWindowLong(w_hwnd, GWL_EXSTYLE, w_stile);
						SetLayeredWindowAttributes(w_hwnd, RGB(222, 222, 222), 0, LWA_COLORKEY);
					}

					static bool desctop_drawer = false;
					ImGui::Checkbox("desctop_drawer", &desctop_drawer);
					if (desctop_drawer)
					{
						static bitmap32 bitmap = png_read_image_file("assets/png.png");

						for (size_t i = 0; i < bitmap.pixel_count(); i++)
							std::swap(bitmap.raw_data(i, 0)->r, bitmap.raw_data(i, 0)->b);

						HWND wnd = NULL;// my_window->GetWindowId().hwnd;
						RECT rect = { 0,0,bitmap.size().x,bitmap.size().y };
						HDC hdc = GetDC(wnd);
						static HBITMAP bits;
						MakeOnce
						{
							BITMAPINFOHEADER   bmih;
							memset(&bmih, 0, sizeof(BITMAPINFOHEADER));

							bmih.biWidth = bitmap.size().x;
							bmih.biHeight = -bitmap.size().y;
							bmih.biBitCount = 32;
							bmih.biCompression = BI_RGB;
							bmih.biSize = sizeof(BITMAPINFOHEADER);
							bmih.biPlanes = 1;

							BITMAPINFO* bmi = (BITMAPINFO*)&bmih;

							bits = CreateDIBitmap(hdc, &bmih, CBM_INIT, bitmap.raw_data(), bmi, DIB_RGB_COLORS);
						}

						static HBRUSH brush = CreatePatternBrush(bits);
						//GetWindowRect(wnd, &rect);
						FillRect(hdc, &rect, brush);
						//DeleteObject(brush);
						ReleaseDC(wnd, hdc);
					}
				});
			IM_VIEW("win32", "magnification", [&]()
				{
					static std::string rs_status;

					//-----------------------------------------//
					ImGui::Text("Status: ");
					ImGui::Text(rs_status.c_str());

					static bool CURSOR_GRAB = false;

					ImGui::Checkbox("CURSOR_GRAB", &CURSOR_GRAB);

					//-----------------------------------------//
					constexpr int SIZE_WH = 512;

					static ScreenGrabber grabber;

					static bitmap32 desc_copy(SIZE_WH, SIZE_WH);
					static bitmap32 desc_view(SIZE_WH, SIZE_WH);

					static GpuTexture gpu_bitmap;

					//////////////////////////////////////////////////////////////////////////
					struct col
					{
						color32 p[256][256][256];
					};

					static col* color_set = new col;
					static col* color_dec = new col;

					MakeOnce
					{

						auto gen_seed = math::xorshift_64x64();

						//static math::RandomGenerator rg;
						//rg.SetSeed((int64_t)555);

						for (int r = 0; r < 256; r++)
						for (int g = 0; g < 256; g++)
						for (int b = 0; b < 256; b++)
						{
							color_set->p[r][g][b] =
							{
								(uint8_t)r,
								(uint8_t)g,
								(uint8_t)b,
								255,
							};
						}
						for (int r = 0; r < 256; r++)
						for (int g = 0; g < 256; g++)
						for (int b = 0; b < 256; b++)
						{
							uint8_t rs = (uint8_t)math::xorshift_64x64(gen_seed);//rg.GenerateInteger(0, 255);
							uint8_t gs = (uint8_t)math::xorshift_64x64(gen_seed);//rg.GenerateInteger(0, 255);
							uint8_t bs = (uint8_t)math::xorshift_64x64(gen_seed);//rg.GenerateInteger(0, 255);

							std::swap(color_set->p[r][g][b], color_set->p[rs][gs][bs]);

							const auto ca = color_set->p[r][g][b];
							const auto cb = color_set->p[rs][gs][bs];

							color_dec->p[ca.r][ca.g][ca.b] =
							{
								static_cast<uint8_t>(r),
								static_cast<uint8_t>(g),
								static_cast<uint8_t>(b),
								255,
							};
							color_dec->p[cb.r][cb.g][cb.b] =
							{
								static_cast<uint8_t>(rs),
								static_cast<uint8_t>(gs),
								static_cast<uint8_t>(bs),
								255,
							};
						}

					}
						//////////////////////////////////////////////////////////////////////////


						if (CURSOR_GRAB)
						{
							grabber.grubFromPoint(os_api::get_cursor_pos(), { SIZE_WH ,SIZE_WH }, CURSOR_GRAB);



							[&]()
							{
								auto from = grabber.captured_bitmap.raw_data();

								for (int i = 0; i < SIZE_WH * SIZE_WH; i++)
								{
									desc_copy.raw_data()[i] =
										color_set->p[from[i].b][from[i].g][from[i].r];
								}
							}();

							gpu_bitmap.Update(desc_copy);
							ImGui::Image((ImTextureID)gpu_bitmap.GetImpl()->uid, { (float)SIZE_WH, (float)SIZE_WH });
						}

					ImGui::Separator();
					ImGui::Text("FullMag (files deleted)");

					static int rect_algo = 5;

					ImGui::Text("Algo:");
					ImGui::RadioButton("Mask", &rect_algo, 5);
					ImGui::RadioButton("Enc",  &rect_algo, 6);
					ImGui::RadioButton("Dec",  &rect_algo, 7);

					static bool swap_action = false;
					static bool is_fullscr = false;

					bool set_fullscr = false;
					bool set_windowed = false;

					swap_action = ImGui::Button("Swap FullMode") || (Keyboard.WasPressed(KKey::KEY_SPACE));

					if (swap_action)
					{
						is_fullscr = !is_fullscr;

						set_fullscr  = swap_action &&  is_fullscr;
						set_windowed = swap_action && !is_fullscr;

						if (set_fullscr)
						{
							grabber.grubScreen();
							desc_copy = grabber.captured_bitmap;

							ForArray(i, desc_copy.pixel_count())
								std::swap(desc_copy.raw_data(i)->r, desc_copy.raw_data(i)->b);

							//TODO MB ADD TO WINDOW CLASS
							SetWindowLong(my_window->GetWindowId().hwnd, GWL_EXSTYLE, GetWindowLong(my_window->GetWindowId().hwnd, GWL_EXSTYLE) | WS_EX_LAYERED);
							SetLayeredWindowAttributes(my_window->GetWindowId().hwnd, 0, 2, LWA_ALPHA);
							//Win32_SetWindowFrame(my_window, set_fullscr_borderless);
							my_window->SetWindowFrame(WindowBorderStile::BorderlessFullScr);
							my_window->SetTopMost(true);

							mPostAct_WindowAlphaState = 2;
						}
						if (set_windowed)
						{
							my_window->SetWindowFrame(WindowBorderStile::DefaultBorders);
							//Win32_SetWindowFrame(my_window, set_normal);
							//my_window->SetTopMost(false);
						}
					}

					if (is_fullscr)
					{

						MakeOnce
						{
							bitmap32 surc = png_read_image_file("C:/temp/in.png");
							bitmap32 mask = png_read_image_file("C:/temp/mask.png");

							surc = imgenc::normalize(surc);
							png_write_image_file(surc, "C:/temp/norm.png");

							ForArray(i, surc.pixel_count())
							{
								if (mask.raw_data(i)->a > 0)
								{
									surc.raw_data(i)->r != 255 ? surc.raw_data(i)->r++ : surc.raw_data(i)->r;
									surc.raw_data(i)->g != 255 ? surc.raw_data(i)->g++ : surc.raw_data(i)->g;
									surc.raw_data(i)->b != 255 ? surc.raw_data(i)->b++ : surc.raw_data(i)->b;
								}
							}

							png_write_image_file(surc, "C:/temp/out.png");
						}


						int pos_x = Mouse.GetLocalPos().x - SIZE_WH / 2;
						int pos_y = Mouse.GetLocalPos().y - SIZE_WH / 2;
						int end_x = desc_copy.size().x - SIZE_WH;
						int end_y = desc_copy.size().y - SIZE_WH;

						if (pos_x < 0) pos_x = 0;
						if (pos_y < 0) pos_y = 0;
						if (pos_x >= end_x) pos_x = end_x;
						if (pos_y >= end_y) pos_y = end_y;

						FLOAT2 pos = {
							(float)pos_x,
							(float)pos_y,
						};

						FLOAT2 pos2 = pos + FLOAT2{
							(float)SIZE_WH,
							(float)SIZE_WH,
						};

						desc_view = desc_copy;

						switch (rect_algo)
						{
						break; case 5: //////////////////////////////////////////////////////////////////////////

							for (int x = pos.x; x < pos2.x; x++)
								for (int y = pos.y; y < pos2.y; y++)
								{
									auto col = desc_view.raw_data(x, y);
									if ((col->r % 2 != 0) && (col->g % 2 != 0) && (col->b % 2 != 0))
									{
										col->r = 255; col->g = 0; col->b = 0;
									}
								}

						break; case 6://////////////////////////////////////////////////////////////////////////

							for (int x = pos.x; x < pos2.x; x++)
								for (int y = pos.y; y < pos2.y; y++)
								{
									auto col = desc_view.raw_data(x, y);
									*col = color_set->p[col->r][col->g][col->b];
								}

						break; case 7://////////////////////////////////////////////////////////////////////////

							for (int x = pos.x; x < pos2.x; x++)
								for (int y = pos.y; y < pos2.y; y++)
								{
									auto col = desc_view.raw_data(x, y);
									*col = color_dec->p[col->r][col->g][col->b];
								}
						}

						gpu_bitmap.Update(desc_view);

						auto dr = ImGui::GetForegroundDrawList();

						ImGui::PushClipRect({ 0,0 }, { 6000,6000 }, false);
						ImGui::GetCurrentWindow()->DC.CursorPos = { 0,0 };
						ImGui::GetCurrentWindow()->DC.CursorMaxPos = { 6000,6000 };
						ImGui::Image((ImTextureID)gpu_bitmap.GetImpl()->uid,
							{ (float)os_api::get_system_metric(os_sys_metric::MAIN_MONITOR_W).GetValue(),
							  (float)os_api::get_system_metric(os_sys_metric::MAIN_MONITOR_H).GetValue() });

						dr->AddRect(pos, pos2, IM_COL32(255, 0, 0, 255), 0, 0, 2);

						ImGui::PopClipRect();
					}
				});
			IM_VIEW("win32", "DesktopGraber", []()
			{
				static bitmap32 ram_capture;
				static GpuTexture gpu_res;

				static SuperTimer st;
				static float view_fraps_fps = 0;
				static float view_compression = 0;

				static bool sw_compress_enabled = false;
				static bool sw_graber_active = false;

 				static int rad_compressor = 0;

				ImGui::Checkbox("Enable desktop graber", &sw_graber_active);

				ImGui::Separator();

				ImGui::Checkbox("Enable compress", &sw_compress_enabled);
				ImGui::RadioButton("Zlib", &rad_compressor, 0);
				ImGui::RadioButton("Png", &rad_compressor, 1);
				ImGui::RadioButton("Jpeg", &rad_compressor, 2);

				ImGui::Separator();

				ImGui::InputFloat("view fps", &view_fraps_fps);
				ImGui::InputFloat("view cmp", &view_compression);
				ImGui::Image((void*)gpu_res.GetImpl()->uid, { 2560, 1440 });

				if (sw_graber_active)
				{
					static ScreenGrabber sc;
					static void* d3d9;
					static atomic_bool captured = false;
 					static std::thread graber = std::thread([&]()
						{
							while (true)
							{
								if (!captured)
								{
									sc.grubScreen();

									captured = true;
								}
								else
									;//std::this_thread::sleep_for(chrono::nanoseconds(20));
							}
						});

					static float frames = 0;

					if (captured)
					{
						frames++;

						//-------------------------------------- brga -> rgba

						ram_capture = sc.captured_bitmap;

						captured = false;

						ForArray(i, ram_capture.pixel_count())
						{
							auto pic = ram_capture.raw_data(i);
							std:swap(pic->r, pic->b);
						}

						//cap.scale(1600,900);
						//--------------------------------------compress

						if (sw_compress_enabled)
						{
							if (rad_compressor == 0)
							{
								//auto zbuff = str::compress::compress_data(ram_capture.raw_data(), ram_capture.data_size(), str::compress::CompessType::BestSpeed);
								//str::compress::decompress_data(zbuff, ram_capture.raw_data());
								//view_compression = zbuff.comp_size / float(ram_capture.data_size());
								//zbuff.release();
 							}

							if (rad_compressor == 1)
							{
								static std::vector<uint8_t> data;
								static std::vector<uint8_t> encoded;
								static std::vector<uint8_t> decoded;

								data.resize(ram_capture.data_size());
								std::memcpy(&data[0], ram_capture.raw_data(), ram_capture.data_size());
								encoded.clear();
								decoded.clear();

								unsigned int w = ram_capture.size().x;
								unsigned int h = ram_capture.size().y;

								//lodepng::encode(encoded, data, w, h);
								//lodepng::decode(decoded, w, h, encoded, LodePNGColorType::LCT_RGBA, 8U);

								view_compression = encoded.size() / float(ram_capture.data_size());

								std::memcpy(ram_capture.raw_data(), &decoded[0], ram_capture.data_size());
							}

							if (rad_compressor == 2)
							{
								//static bitmap24 data;
								//
								//auto size = ram_capture.size();
								//
								//if (data.size() != size)
								//	data = bitmap24(size.x, size.y);
								//
								//ForArray(i, ram_capture.pixel_count())
								//{
								//	data.raw_data(i)->r = ram_capture.raw_data(i)->r;
								//	data.raw_data(i)->g	= ram_capture.raw_data(i)->g;
								//	data.raw_data(i)->b	= ram_capture.raw_data(i)->b;
								//}
								//
								//jbuff buff = pack_jbuff(data.raw_data(), size.x, size.y);
								//
								//unpack_jbuff_to(buff, (unsigned char*)data.raw_data());
								//
								//ForArray(i, ram_capture.pixel_count())
								//{
								//	ram_capture.raw_data(i)->r = data.raw_data(i)->r;
								//	ram_capture.raw_data(i)->g = data.raw_data(i)->g;
								//	ram_capture.raw_data(i)->b = data.raw_data(i)->b;
								//}
							}
						}

						gpu_res.Update(ram_capture);
 					}

					static float esc_time = 0;
					esc_time += st.GetRoundTime();

					if (esc_time >= 0.999)
					{
						esc_time = 0;
						view_fraps_fps = frames / 0.999;
						frames = 0;
					}
				}


			});

			//-------------------------------------------------------// test
			IM_VIEW("test", "show openCL",     []() { imview::openCL();     });
			IM_VIEW("test", "show openCV",     []() { imview::openCV();   });
			IM_VIEW("test", "show wifi map",   []() { imview::wifiMap();   });
			IM_VIEW("test", "show sort gui",   []() { imview::sortView();   });
			IM_VIEW("test", "show resize_c",   []() { imview::resize_map(); });
			IM_VIEW("test", "show zlib test",  []() { imview::zlib_test();  });
			IM_VIEW("test", "show Live",       []() { imview::view_live();  });
			IM_VIEW("test", "show Live2",      []() { imview::view_live2(); });
			IM_VIEW("test", "show playground", []() { imview::playground(); });
			//-------------------------------------------------------// gui
			IM_VIEW("gui", "Panel & Grid", []()
				{
					ImGuiIO& io = ImGui::GetIO();
					ImGuiContext* context = ImGui::GetCurrentContext();
					ImGuiWindow* window = context->CurrentWindow;
					ImDrawList* drawer = window->DrawList;

					static ImItem::Region region_canvas;
					static ImItem::Region region_left_side;
					static ImItem::Grid   grid_right_side;

					FLOAT2 canvas_pos = ImGui::GetCursorPos();
					FLOAT2 canvas_pos_g = ImGui::GetCursorPos() + ImGui::GetWindowPos();
					FLOAT2 canvas_size = ImGui::GetContentRegionAvail();


					region_canvas.InitRegion_Ratio(canvas_pos, canvas_size, 0.50, true);
					region_left_side.InitRegion_Ratio(region_canvas.GetRegionA(), 0.50, false);
					grid_right_side.InitRegion(region_canvas.GetRegionB(), 6, 6);

					bool press = false;

					ImGui::SetCursorPos(region_left_side.GetPosA({ 4,4 }));	press |= ImGui::Button("PRESS##0", region_left_side.GetSizeA({ 4,4 }));	// 0.
					ImGui::SetCursorPos(region_left_side.GetPosB({ 4,4 }));	press |= ImGui::Button("PRESS##1", region_left_side.GetSizeB({ 4,4 }));	// 0.

					ForArray(y, 6)
						ForArray(x, 6)
					{
						ImGui::SetCursorPos(grid_right_side.GetRegionPos(x, y, { 4,4 }));
						press |= ImGui::Button(fmt::format("PRESS##{}{}", x, y).c_str(), grid_right_side.GetRegionSize(x, y, { 4,4 }));
					}

					static FLOAT2 xy;
					static int x = -1;

					if (press)
					{
						xy = io.MousePos;
						x = 0;
					}
					if (x > -1)
					{
						drawer->AddCircleFilled(xy, x++, IM_COL32(255, 0, 0, 255), x);
						drawer->AddCircle(xy, x++, IM_COL32(255, 255, 255, 255), x, 1.0f);

						if (x > 100)
						{
							x = -1;
						}
					}

					ImGui::PushClipRect(FLOAT2(0, 0), FLOAT2(10000, 10000), false);
					{




					}
					ImGui::PopClipRect();

				});
			IM_VIEW("gui", "Fonts loader", []() { im_font_loader.if_draw(); });
			//-------------------------------------------------------//
			IM_VIEW("bech", "hash distribution", []() { imview::sunc_hash_distribution(); });
			IM_VIEW("bech", "mem distribution",  []() { imview::sunc_mem_distribution();  });

			IM_VIEW("sunc", "packets",   []() { imview::sunc_packets();  });
            IM_VIEW("bech", "umanager",  []() { imview::umanager_view(); });
            IM_VIEW("bech", "suncBench", []() { imview::suncBench();     });
            IM_VIEW("bech", "corbaLog",  []() { imview::corbaLog();      });
		}

		ImGuiEnd();
		EndPaint();

		//-------------------------------------//
		if (mPostAct_WindowAlphaState > 0)
		{
			if (mPostAct_WindowAlphaState == 3)
			{
				::SetLayeredWindowAttributes(my_window->GetWindowId().hwnd, 0, 255, LWA_ALPHA);
				mPostAct_WindowAlphaState = 0;
			}
			else
			{
				mPostAct_WindowAlphaState = 3;
			}
		}
	}

	void Frame()
	{
		Paint();

		Mouse.ClearState();
		Keyboard.ClearEvents();

		if (mCheck_GLErrors)
		{
			auto glerr = glGetError();

			while (glerr != 0)
			{
				switch (glerr)
				{
				break; case GL_NO_ERROR:
				break; case GL_INVALID_ENUM:                  log::error << "GL_INVALID_ENUM: An unacceptable value is specified for an enumerated argument.The offending command is ignored and has no other side effect than to set the error flag.";
				break; case GL_INVALID_VALUE:                 log::error << "GL_INVALID_VALUE: A numeric argument is out of range.The offending command is ignored and has no other side effect than to set the error flag.";
				break; case GL_INVALID_OPERATION:             log::error << "GL_INVALID_OPERATION: The specified operation is not allowed in the current state.The offending command is ignored and has no other side effect than to set the error flag.";
				break; case GL_INVALID_FRAMEBUFFER_OPERATION: log::error << "GL_INVALID_FRAMEBUFFER_OPERATION: The framebuffer object is not complete.The offending command is ignored and has no other side effect than to set the error flag.";
				break; case GL_OUT_OF_MEMORY:                 log::error << "GL_OUT_OF_MEMORY: There is not enough memory left to execute the command.The state of the GL is undefined, except for the state of the error flags, after this error is recorded.";
				break; case GL_STACK_UNDERFLOW:               log::error << "GL_STACK_UNDERFLOW: An attempt has been made to perform an operation that would cause an internal stack to underflow.";
				break; case GL_STACK_OVERFLOW:                log::error << "GL_STACK_OVERFLOW: An attempt has been made to perform an operation that would cause an internal stack to overflow.";
				default:
					log::error << "GL_ERROR: unexpected error code"; break;
				}

				glerr = glGetError();
			}
		}

		if (mChech_Win32Errors)
		{
			DWORD win32err = ::GetLastError();

			while (win32err != 0)
			{
				mWin32errCount++;

				std::string err = Win32errorToString(win32err);

				if (!err.empty())
					log::error << "win32 error: " + str::convert::cp1251_to_utf8(err);

				win32err = ::GetLastError();
				::SetLastError(0);
			}
		}

 	}
};