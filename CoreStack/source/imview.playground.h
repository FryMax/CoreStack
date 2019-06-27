#pragma once
#include "pch.h"

namespace imview
{
	void playground();
}

//////////////////////////////////////////////////////////////////////////

void imview::playground()
{
	static const size_t CanvasSize = 800;
	static const FLOAT2 CanvasMid = FLOAT2{ CanvasSize, CanvasSize } / 2;
	static const float  DotRad = 8;
	static const float  DotSpeed = 0.999;

	struct Circle
	{
		static Circle genCircle()
		{
			return Circle
			{
				CanvasMid + FLOAT2(math::Random(-20.f,20.f), math::Random(-20.f,20.f)),
				(float)math::RandomFastReal(0.,360.),
				DotRad,
				DotSpeed
			};
		}

		FLOAT2 pos;
		float angle;
		const float radius;
		const float speed;
	};

	static std::vector<Circle> vec;

	static bitmap32 img(CanvasSize, CanvasSize);
	static GpuTexture gpuImg;
	static SuperTimer ST;

	static auto GetReflect = [](FLOAT2 vec, FLOAT2 normal) -> FLOAT2
	{
		float dotprod = vec.x * normal.x + vec.y * normal.y;
		return vec - (normal * dotprod * 2.f);
	};
	static auto GetAngle = [](Circle c, FLOAT2 normal) -> float
	{
		auto vec = math::move_on_angle(c.pos, c.angle, c.speed) - c.pos;
		auto ref = GetReflect(vec, normal);
		return math::anlage_2d(FLOAT2{0,0}, ref);
	};

	while ( ST.InEach(1./ 200.) )
	{
		//auto buf = vec;
		for (size_t i = 0; i < vec.size(); i++)
		{
			auto& it  = vec[i];

			auto put_in_range = [](float val, float min, float max)
			{
				if (val <= min) return min;
				if (val >= max) return max;
				return val;
			};

			//auto& out = buf[i];
			//---------------
			bool collision_left   = it.pos.x - it.radius <= 0;
			bool collision_top    = it.pos.y - it.radius <= 0;
			bool collision_right  = it.pos.x + it.radius >= CanvasSize;
			bool collision_bottom = it.pos.y + it.radius >= CanvasSize;

			if (collision_bottom) { it.angle = GetAngle(it, FLOAT2{  0.0,  1.0 }); }
			if (collision_left  ) { it.angle = GetAngle(it, FLOAT2{  1.0,  0.0 }); }
			if (collision_top   ) { it.angle = GetAngle(it, FLOAT2{  0.0, -1.0 }); }
			if (collision_right ) { it.angle = GetAngle(it, FLOAT2{ -1.0,  0.0 }); }

			it.pos.x = put_in_range(it.pos.x, it.radius, CanvasSize - it.radius);
			it.pos.y = put_in_range(it.pos.y, it.radius, CanvasSize - it.radius);

			//---------------
			for (auto& cl : vec)
			{
				if(&it == &cl)
					continue;

				const float overlap = it.radius + cl.radius - math::distance_2d(it.pos, cl.pos);

				if (overlap > 0.)
				{
					const auto delta = it.pos - cl.pos;
					const auto normalA = delta / math::distance_2d({ 0,0 }, delta);
					const auto normalB = normalA * -1.0;

					it.angle = GetAngle(it, normalA);
					cl.angle = GetAngle(cl, normalB);
					it.pos += normalA * overlap / 2.;
					cl.pos += normalB * overlap / 2.;
				}
			}
		}

		//vec = std::move(buf);
		for (auto& it : vec)
		{
			it.pos = math::move_on_angle(it.pos, it.angle, it.speed);
		}
	}

	//ImGuiIO& io = ImGui::GetIO();
	ImGuiContext* context = ImGui::GetCurrentContext();
	ImGuiWindow* window = context->CurrentWindow;
	ImDrawList* drawer = window->DrawList;

	////////////////////////////////////////////////////////////////////////// draw ui
	const FLOAT2 POS = ImGui::GetCursorPos();
	const FLOAT2 OFFSET = window->Pos + POS;
	const FLOAT2 RECT = FLOAT2{ CanvasSize,CanvasSize };

	drawer->AddRect(OFFSET, OFFSET + RECT, IM_COL32(255, 0, 0, 255));
	for (/*const*/ auto& it : vec)
	{
		drawer->AddCircleFilled(OFFSET + it.pos, it.radius, IM_COL32(255, 0, 0, 99));
		drawer->AddLine(OFFSET + it.pos, OFFSET + math::move_on_angle(it.pos, it.angle, it.radius * 1.5), IM_COL32(255, 0, 0, 99));
	}

	if (ImGui::Button("[add one]"))
	{
		vec.emplace_back(Circle::genCircle());
	}
	ImGui::SameLine();
	if (ImGui::Button("[add 20]"))
	{
		for (size_t i = 0; i < 20; i++)
			vec.emplace_back(Circle::genCircle());
	}
}