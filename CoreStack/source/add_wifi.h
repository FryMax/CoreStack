#pragma once

namespace wifi
{
	inline color32 powerToColor(const int power, bool white_buck = false)
	{
		size_t level = power / 256;
		uint8_t   pw = power % 256;
		uint8_t  npw = uint8_t(255 - pw);

		level = level >= 5 ? 5 : level;

		switch (level)
		{
		case 0:  if (white_buck) return color32{ 255, npw, npw, 255 };
				 else           return color32{ pw,    0,   0,  pw };
		case 1:	 return color32{ 255,  pw,   0, 255 };
		case 2:  return color32{ npw, 255,   0, 255 };
		case 3:  return color32{ 0,   255,  pw, 255 };
		case 4:  return color32{ 0,   npw, 255, 255 };
		case 5:  return color32{ 0,     0, 255, 255 };
		default: return color32{ 200, 200, 200, 255 };
		}
	}

	using Vec = FLOAT2;
	using Map = bitmap32;

	struct WifiSpot;

	struct World
	{
		Map walls;
		Map objects;
		Map power;

		bitmap_t<float> energy;
		const float power_map_scale = 0.8f;	// Power map scaling
		Vec wifi_pos;

		bool ins_rays_on = true;
		bool ref_rays_on = true;

		float total_refs = 0;
	};

	struct Ray
	{
		Vec pos;
		Vec dir;

		float energy;

		inline Ray normalize(float len)
		{
			dir /= std::sqrt(std::pow(dir.x, 2) + std::pow(dir.y, 2)) * len;
			return *this;
		}
	};

	struct RayCaster
	{
		const color32 cwall = color32{ 0,   0,   0, 255 };
		const color32 ñbord = color32{ 49, 160, 255, 255 };
		const color32 cfree = color32{ 255, 255, 255, 255 };

		void throwRay(Ray& ray, World& world)
		{
			FLOAT2& pos = ray.pos;
			FLOAT2& dir = ray.dir;
			float& power = ray.energy;
			auto& colizion = world.walls;

			while (power > 0)
			{
				power -= 0.00006f;

				if (*colizion.raw_data((int)pos.x, (int)pos.y) == ñbord)
					break;

				while (*colizion.raw_data((int)pos.x, (int)pos.y) == cwall)
				{
					if (!world.ins_rays_on)
						return;

					power *= 0.989f;
					pos += dir;
				}

				const bool clHor = *colizion.raw_data(int(pos.x + dir.x), int(pos.y)) == cwall;
				const bool clVer = *colizion.raw_data(int(pos.x), int(dir.y + pos.y)) == cwall;

				if (clHor || clVer)
				{
					const float power_abs = power * 0.44f;
					const float power_ref = power_abs * (0.8 + 0.45);
					const float power_ins = power_abs * (0.8 + 0.45);

					power = power_ins;

					Ray ref_ray;

					ref_ray.pos = pos - dir;
					ref_ray.dir = dir;
					ref_ray.energy = power_ref;

					if (clHor) ref_ray.dir.x = -dir.x;
					if (clVer) ref_ray.dir.y = -dir.y;

					ref_ray.dir =
						math::move_on_angle({ 0,0 }, math::RandomFastReal(-9., 9.) + math::anlage2D({ 0,0 }, ref_ray.dir), 0.99999f);

					if (world.ref_rays_on)
						throwRay(ref_ray, world);

					world.total_refs++;
				}

				pos += dir;

				if (world.walls.raw_data((int)pos.x, (int)pos.y)->r > 90)
				{
					INT2 p = {
						int(pos.x * world.power_map_scale),
						int(pos.y * world.power_map_scale),
					};

					int dist = (int)math::distance2D(world.wifi_pos, pos);

					*world.energy.raw_data(p.x, p.y) += power;

					if (dist % 5 < 2)
					{
						*world.power.raw_data(p.x, p.y) =
							powerToColor(*world.energy.raw_data((int)p.x, (int)p.y));
					}
				}
			}
		}
	};

	struct RayColiderBody : RayCaster
	{
		enum ClType : unsigned
		{
			ClFree = 0x0,
			ClVert = 0x2,
			ClHor = 0x4,
		};

		virtual ClType CheckCollision(const Ray& ray, const World& world)
		{
			return error_result(ClFree, "NoImpl").GetValue();
		}
	};

	struct Material
	{
		enum MatType : unsigned
		{
			MtUnknown,
			MtWood,
			MtBlock,
			MtÑoncrete,
			MtArmed,
		};

		float thickness;
		float hardness;
	};

	struct Wall : Material, RayColiderBody
	{
		ClType CheckCollision(const Ray& ray, const World& world)
		{
			error_result("NoImpl");
			return ClFree;
		}
	};

	struct Object : RayColiderBody
	{
		ClType CheckCollision(const Ray& ray, const World& world)
		{
			error_result("NoImpl");
			return ClFree;
		}
	};

	struct WifiSpot : RayCaster
	{
		enum WfRangeType
		{
			Wireless_G_2_4Ghz,	// 2.4 Ghz frequency
			Wireless_N_5_0Ghz,	// 5.0 Ghz frequency
		};

		enum WfPowerType
		{
			High,		// 200 meters
			Average,	// 50 meters
			Low,		// 20 meters
		};

		WifiSpot(Vec pos = { 0,0 }, WfRangeType rt = Wireless_G_2_4Ghz, WfPowerType pt = Average)
		{
			total_power = 500'000;
			ray_power = total_power / 5'000;
		}

		bool isCharged()
		{
			return ray_power * ray_count < total_power;
		}

		Ray generateRay()
		{
			return
			{
				this->Pos,
				math::move_on_angle({ 0,0 }, math::Random(0.0f, 360.0f), 1.0f),
				this->ray_power,
			};
		}

	public:
		Vec Pos;				// Router position
		std::vector<Ray> rays;	// Active rays

		float ray_power = 0;	// Single ray start power
		float ray_count = 0;	// Total casted rays
		float total_power = 0;	// Total output power
	};

	class WifiSimulator : public World
	{
	public:
		WifiSpot wifi_spot;

		bool is_active = false;
		bool need_update = false;

	public:
		void loadPlan(const std::string& path)
		{
			need_update = true;

			walls = png_read_image(path);

			int pw = (int)(walls.size().x * power_map_scale);
			int ph = (int)(walls.size().y * power_map_scale);

			power = bitmap32(pw, ph, { 0,0,0,0 });
			energy = bitmap_t<float>(pw, ph, 0.f);
		}

		void update()
		{
			const float max_timeout = 1.00f / 200.f;
			wifi_pos = wifi_spot.Pos;

			if (is_active)
			{
				need_update = true;

				if (wifi_spot.isCharged())
				{
#pragma omp parallel for num_threads(4)
					for (int i = 0; i < 20; i++)
					{
						wifi_spot.ray_count++;
						Ray ray = wifi_spot.generateRay();
						wifi_spot.throwRay(ray, *static_cast<World*>(this));
					}
				}
				else
				{
					is_active = false;

					bitmap32 blur(energy.size().x, energy.size().y);

					ForArray(y, blur.size().y)
						ForArray(x, blur.size().x)
					{
						constexpr int kernel = 9 + 8;
						constexpr int off = kernel / 2;

						INT2 ker_mid = { (int)x, (int)y };

						if (ker_mid.x < off) ker_mid.x = off;
						if (ker_mid.y < off) ker_mid.y = off;

						if (ker_mid.x + off >= blur.size().x) ker_mid.y = blur.size().x - off - 1;
						if (ker_mid.y + off >= blur.size().y) ker_mid.y = blur.size().y - off - 1;

						float summ = 0;
						float max = 0;

						for (int iy = ker_mid.y - off; iy < ker_mid.y + off; iy++)
							for (int ix = ker_mid.x - off; ix < ker_mid.x + off; ix++)
							{
								float val = *energy.raw_data(ix, iy);
								summ += val;
								max = val > max ? val : max;
							}

						summ /= std::powf(kernel, 2.);

						if (*walls.raw_data(
							int(x / power_map_scale),
							int(y / power_map_scale)) == color32{ 0,0,0,255 })
						{
							summ /= 20;
						}

						*blur.raw_data(x, y) = powerToColor(summ);
					}
					power = blur;
				}
			}
		}
	};

	void imgui_show_wifi_map()
	{
		auto canv_pos_glob = ImGui::GetCursorPos() + ImGui::GetWindowPos();
		auto canv_avail = ImGui::GetContentRegionAvail();
		auto canv_drawlist = ImGui::GetWindowDrawList();

		FLOAT2 mouse_pos_glob =
		{
			static_cast<float>(Mouse.GetLocalPos().x),
			static_cast<float>(Mouse.GetLocalPos().y),
		};

		const auto color_green = IM_COL32(0, 200, 0, 200);
		const auto color_red = IM_COL32(200, 0, 0, 200);
		const auto color_white = IM_COL32(255, 255, 255, 60);
		const auto color_yellow = IM_COL32(255, 255, 0, 99);
		const auto color_black = IM_COL32(0, 0, 0, 255);

		//////////////////////////////////////////////////////////////////////////
		static WifiSimulator simulator;



		//////////////////////////////////////////////////////////////////////////
		static const FLOAT2   size = { 1200, 900 };
		static const FLOAT2   size2 = { 1200, 32 };
		static const bitmap32 map_buff(size.x, size.y, color32{ 255, 0, 0, 64 });
		static const bitmap32 pow_buff(size.x, size.y, color32{ 0, 0, 0, 0 });
		static const bitmap32 color_buffer(size2.x, size2.y, color32{ 255, 255, 0, 64 });

		static GpuTexture gpu_wall;
		static GpuTexture gpu_power;
		static GpuTexture gpu_color;

		MakeOnce
		{
			QUAD quad(
				INT2{ 2,2 },
				INT2{color_buffer.size().x - 5,
					 color_buffer.size().y - 5});

			ForArray(y, color_buffer.size().y)
			ForArray(x, color_buffer.size().x)
			{
				float max_power = 6 * 255.;
				float pw_per_pt = max_power / color_buffer.size().x;

				*color_buffer.raw_data(x, y) = powerToColor(x * pw_per_pt, true);

				if (!quad.isContains(INT2{(int)x, (int)y }))
				{
					*color_buffer.raw_data(x, y) = {200,200,200,255};
				}
			}

			gpu_wall.Update(map_buff);
			gpu_power.Update(pow_buff);
			gpu_color.Update(color_buffer);
		}

		const FLOAT2 image_mid_loc = (size / 2.);
		const FLOAT2 image_pos_glob = ImGui::GetCurrentWindow()->Pos + ImGui::GetCursorPos();
		const FLOAT2 mouse_img_pos = (FLOAT2(Mouse.GetLocalPos().x, Mouse.GetLocalPos().y) - image_pos_glob);

		const bool mouse_in_img =
			mouse_img_pos.x >= 0 &&
			mouse_img_pos.y >= 0 &&
			mouse_img_pos.y < size.x &&
			mouse_img_pos.y < size.y;

		// draw maps
		auto cp = ImGui::GetCursorPos();
		ImGui::SetCursorPos(cp); ImGui::Image((void*)gpu_wall.GetImpl()->uid, size);
		ImGui::SetCursorPos(cp); ImGui::Image((void*)gpu_power.GetImpl()->uid, size);

		ImGui::SameLine();
		ImGui::BeginGroup();
		{
			ImGui::PushItemWidth(200);
			{
				ImGui::Separator(); //----------------------------------
				ImGui::Text("Wifi spot");

				ImGui::InputFloat("Rays throwed", &simulator.wifi_spot.ray_count);
				ImGui::InputFloat("Total refs", &simulator.total_refs);
				ImGui::InputFloat("Spot power", &simulator.wifi_spot.total_power);
				ImGui::RadioButton("Wireless_G_2_4Ghz", true);
				ImGui::RadioButton("Wireless_N_5_0Ghz", false);

				ImGui::Separator(); //----------------------------------
				ImGui::Text("Template loader");

				if (ImGui::Button("ast/wifi/A+", { 90, 0 })) { simulator.loadPlan("assets/wifi/walls.png"); }
				if (ImGui::Button("ast/wifi/test", { 90, 0 })) { simulator.loadPlan("assets/wifi/A.png"); }
				if (ImGui::Button("ast/wifi/null", { 90, 0 })) { simulator.loadPlan("assets/wifi/B.png"); }

				ImGui::Separator(); //----------------------------------
				ImGui::Text("Simulation launcher");

				if (ImGui::Button("Start", { 90, 0 }))
				{
					for (;;)
					{
						if (math::equal(simulator.walls.size(), INT2{ 0, 0 }))
						{
							log::warn << inUTF8("Don`t forget to set load plan");
							break;
						}

						if (math::equal(simulator.wifi_spot.Pos, FLOAT2{ 0, 0 }))
						{
							log::warn << inUTF8("Don`t forget to set wifi position");
							break;
						}

						simulator.total_refs = 0;
						simulator.wifi_spot.ray_count = 0;
						simulator.power.fill_by_color({ 0, 0, 0, 0 });
						simulator.energy.fill_by_color(0.0f);
						simulator.is_active = true;
						break;
					}
				}

				float proc =
					(simulator.wifi_spot.ray_power
						* simulator.wifi_spot.ray_count)
					/ simulator.wifi_spot.total_power;

				ImGui::ProgressBar(proc, FLOAT2(200, 0));

				ImGui::Dummy({ 0,60 });
				ImGui::Separator(); //----------------------------------
				ImGui::Text("Raytrayser opt");
				ImGui::Checkbox("Interpose Rays", &simulator.ins_rays_on);
				ImGui::Checkbox("Reflected Rays", &simulator.ref_rays_on);

				//ImGui::InputFloat("int frac", nullptr);
				//ImGui::InputFloat("ref frac", nullptr);

			}
			ImGui::PopItemWidth();
		}
		ImGui::EndGroup();

		ImGui::Image((void*)gpu_color.GetImpl()->uid, size2);


		if ("wifi position")
		{
			if (Keyboard.IsPressed(KKey::KEY_W))
			{
				simulator.wifi_spot.Pos = mouse_img_pos;
			}

			if (!math::equal(simulator.wifi_spot.Pos, { 0,0 }))
			{
				ImGui::GetForegroundDrawList()->AddRectFilled(
					canv_pos_glob + simulator.wifi_spot.Pos - FLOAT2{ 8,8 },
					canv_pos_glob + simulator.wifi_spot.Pos + FLOAT2{ 8,8 },
					IM_COL32(255, 0, 0, 200));
			}
		}
		if ("wall selection")
		{
			constexpr color32 color_wall = color32{ 0,0,0,255 };

			if (Keyboard.IsPressed(KKey::KEY_CONTROL) &&
				simulator.walls.size() != INT2{ 0,0 })
			{
				static auto _pos = FLOAT2{ 0.f,0.f };

				static FLOAT2 imgA = { 0,0 };
				static FLOAT2 imgB = { 0,0 };

				if (mouse_in_img && !math::equal(_pos, mouse_img_pos))
				{
					_pos = mouse_img_pos;

					if (simulator.walls.get_pixel(_pos.x, _pos.y) == color_wall)
					{
						auto find = [&](int pw, int ph, int w, int h, const bitmap32& bt)
						{
							while (*bt.raw_data(pw + w, ph + h) == color_wall)
							{
								pw += w;
								ph += h;
							}

							return INT2(pw + w, ph + h);
						};

						INT2 l = find(_pos.x, _pos.y, -1, 0, simulator.walls);
						INT2 r = find(_pos.x, _pos.y, +1, 0, simulator.walls);
						INT2 t = find(_pos.x, _pos.y, 0, -1, simulator.walls);
						INT2 b = find(_pos.x, _pos.y, 0, +1, simulator.walls);

						imgA = FLOAT2{ (float)l.x, (float)t.y };
						imgB = FLOAT2{ (float)r.x, (float)b.y };
					}
				}

				const FLOAT2 A = imgA + image_pos_glob - FLOAT2{ 2, 2 };
				const FLOAT2 B = imgB + image_pos_glob + FLOAT2{ 3, 3 };

				ImGui::GetForegroundDrawList()->AddRectFilled(A, B, IM_COL32(0, 220, 0, 200));
				ImGui::GetForegroundDrawList()->AddRect(A, B, IM_COL32(0, 255, 0, 255));
			}

		}

		if (simulator.is_active)
		{
			simulator.update();

			static SuperTimer st;
			static double time = 0;
			static double nps = 200000;
			static double per = 0.999 / nps;

			time += st.GetRoundTime();
			while (time > per)
				time -= per;
		}

		if (simulator.need_update)
		{
			simulator.need_update = false;

			gpu_wall.Update(simulator.walls);
			gpu_power.Update(simulator.power);
		}
	}
}