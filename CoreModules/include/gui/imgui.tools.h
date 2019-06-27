#pragma once

#include "../shared_in.h"

#include "imgui.impl.h"

namespace ImItem
{
	namespace helpers
	{
		inline void set_stile_colors_light()
		{
			ImGuiStyle& style                           = ImGui::GetStyle();
			style.Colors[ImGuiCol_Text]                 = ImVec4(0.08f, 0.08f, 0.08f, 1.00f);	// Текст
			style.Colors[ImGuiCol_TextDisabled]         = ImVec4(0.60f, 0.60f, 0.60f, 1.00f);	// Отключенный текст
			style.Colors[ImGuiCol_WindowBg]             = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);	// Окно
			style.Colors[ImGuiCol_ChildBg]              = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);	// Окно в  окне
			style.Colors[ImGuiCol_Border]               = ImVec4(0.00f, 0.00f, 0.00f, 0.15f);	// обводка
			style.Colors[ImGuiCol_BorderShadow]         = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);	// обводка двойная (тень)
			style.Colors[ImGuiCol_FrameBg]              = ImVec4(0.59f, 0.59f, 0.59f, 0.39f);	// все поля ввода
			style.Colors[ImGuiCol_FrameBgHovered]       = ImVec4(0.78f, 0.78f, 0.78f, 0.39f);	// все поля ввода (НАВЕДЕНИЕ)
			style.Colors[ImGuiCol_FrameBgActive]        = ImVec4(1.00f, 0.59f, 0.00f, 0.39f);	// все поля ввода (НАЖАТИЕ)
			style.Colors[ImGuiCol_TitleBg]              = ImVec4(1.00f, 0.71f, 0.00f, 0.59f);	// Рамка окна
			style.Colors[ImGuiCol_TitleBgCollapsed]     = ImVec4(1.00f, 0.71f, 0.00f, 0.78f);	// Рамка окна (СВЕРНУТАЯ)
			style.Colors[ImGuiCol_TitleBgActive]        = ImVec4(1.00f, 0.71f, 0.00f, 1.00f);	// Рамка окна (АКТИВНАЯ)
			style.Colors[ImGuiCol_MenuBarBg]            = ImVec4(1.00f, 1.00f, 0.00f, 0.39f);	// Меню бар
			style.Colors[ImGuiCol_ScrollbarBg]          = ImVec4(0.24f, 0.24f, 0.24f, 0.12f);	// Скролбар
			style.Colors[ImGuiCol_ScrollbarGrab]        = ImVec4(0.24f, 0.24f, 0.24f, 0.59f);	// Скролбар бегунок
			style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.24f, 0.24f, 0.24f, 0.39f);	// Скролбар бегунок (НАВЕДЕНИЕ)
			style.Colors[ImGuiCol_ScrollbarGrabActive]  = ImVec4(0.24f, 0.24f, 0.24f, 0.78f);	// Скролбар бегунок	(НАЖАТИЕ)
			//style.Colors[ImGuiCol_ComboBg]            = ImVec4(0.78f, 0.78f, 0.78f, 0.90f);	// Выборный список
			style.Colors[ImGuiCol_CheckMark]            = ImVec4(1.00f, 0.39f, 0.00f, 0.78f);	// Радио Кнопка
			style.Colors[ImGuiCol_SliderGrab]           = ImVec4(1.00f, 0.59f, 0.00f, 0.78f);	// Бегунок
			style.Colors[ImGuiCol_SliderGrabActive]     = ImVec4(1.00f, 0.59f, 0.00f, 1.00f);	// Бегунок (НАЖАТИЕ)
			style.Colors[ImGuiCol_Button]               = ImVec4(1.00f, 0.78f, 0.24f, 1.00f);	// Кнопка
			style.Colors[ImGuiCol_ButtonHovered]        = ImVec4(1.00f, 0.78f, 0.24f, 0.78f);	// Кнопка (НАВЕДЕНИЕ)
			style.Colors[ImGuiCol_ButtonActive]         = ImVec4(0.94f, 0.71f, 0.16f, 1.00f);	// Кнопка (НАЖАТИЕ)
			style.Colors[ImGuiCol_Header]               = ImVec4(1.00f, 0.78f, 0.24f, 0.86f);	// Сворачиваемая колонка
			style.Colors[ImGuiCol_HeaderHovered]        = ImVec4(1.00f, 0.71f, 0.00f, 1.00f);	// Сворачиваемая колонка (НАВЕДЕНИЕ)
			style.Colors[ImGuiCol_HeaderActive]         = ImVec4(1.00f, 0.78f, 0.24f, 1.00f);	// Сворачиваемая колонка (НАЖАТИЕ)
			style.Colors[ImGuiCol_Separator]            = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);	// Линия колонок вертикальная
			style.Colors[ImGuiCol_SeparatorHovered]     = ImVec4(0.70f, 0.60f, 0.60f, 1.00f);	// Линия колонок вертикальная (НАВЕДЕНИЕ)
			style.Colors[ImGuiCol_SeparatorActive]      = ImVec4(0.90f, 0.70f, 0.70f, 1.00f);	// Линия колонок вертикальная (НАЖАТИЕ)
			style.Colors[ImGuiCol_ResizeGrip]           = ImVec4(0.24f, 0.24f, 0.24f, 0.63f);	// Уголочек размера окна
			style.Colors[ImGuiCol_ResizeGripHovered]    = ImVec4(1.00f, 0.47f, 0.00f, 0.71f);	// Уголочек размера окна (НАВЕДЕНИЕ)
			style.Colors[ImGuiCol_ResizeGripActive]     = ImVec4(1.00f, 0.47f, 0.00f, 1.00f);	// Уголочек размера окна (НАЖАТИЕ)
			//style.Colors[ImGuiCol_CloseButton]        = ImVec4(0.50f, 0.50f, 0.90f, 0.50f);	// Закрытие окна
			//style.Colors[ImGuiCol_CloseButtonHovered] = ImVec4(0.70f, 0.70f, 0.90f, 0.60f);	// Закрытие окна → (НАВЕДЕНИЕ)
			//style.Colors[ImGuiCol_CloseButtonActive]  = ImVec4(0.70f, 0.70f, 0.70f, 1.00f);	// Закрытие окна → (НАЖАТИЕ)
			style.Colors[ImGuiCol_PlotLines]            = ImVec4(0.16f, 0.16f, 0.16f, 0.78f);	// Линия
			style.Colors[ImGuiCol_PlotLinesHovered]     = ImVec4(1.00f, 0.70f, 0.00f, 1.00f);	// Линия → (НАВЕДЕНИЕ)
			style.Colors[ImGuiCol_PlotHistogram]        = ImVec4(1.00f, 0.63f, 0.00f, 1.00f);	// Гистограмма
			style.Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.43f, 0.00f, 1.00f);	// Гистограмма → (НАВЕДЕНИЕ)
			style.Colors[ImGuiCol_TextSelectedBg]       = ImVec4(1.00f, 0.43f, 0.00f, 1.00f);	// Изменяемый текст фон
			style.Colors[ImGuiCol_PopupBg]              = ImVec4(0.78f, 0.78f, 0.78f, 0.90f);	// Подскака фон
			style.Colors[ImGuiCol_ModalWindowDimBg]     = ImVec4(0.20f, 0.20f, 0.20f, 0.31f);	// хз - Затемнение модального окна
		}
		inline void set_stile_colors_lighter()
		{
			ImGuiStyle& style                           = ImGui::GetStyle();
			style.Colors[ImGuiCol_Text]                 = ImVec4(0.08f, 0.08f, 0.08f, 1.00f);
			style.Colors[ImGuiCol_TextDisabled]         = ImVec4(0.60f, 0.60f, 0.60f, 1.00f);
			style.Colors[ImGuiCol_WindowBg]             = ImVec4(1.00f, 1.00f, 1.00f, 0.94f);
			style.Colors[ImGuiCol_PopupBg]              = ImVec4(0.78f, 0.78f, 0.78f, 1.00f);
			style.Colors[ImGuiCol_Border]               = ImVec4(0.00f, 0.00f, 0.00f, 0.47f);
			style.Colors[ImGuiCol_BorderShadow]         = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
			style.Colors[ImGuiCol_FrameBg]              = ImVec4(0.59f, 0.59f, 0.59f, 0.20f);
			style.Colors[ImGuiCol_FrameBgHovered]       = ImVec4(0.90f, 0.63f, 0.00f, 0.63f);
			style.Colors[ImGuiCol_FrameBgActive]        = ImVec4(0.90f, 0.31f, 0.00f, 0.78f);
			style.Colors[ImGuiCol_TitleBg]              = ImVec4(1.00f, 0.71f, 0.00f, 0.78f);
			style.Colors[ImGuiCol_TitleBgCollapsed]     = ImVec4(1.00f, 0.71f, 0.00f, 0.78f);
			style.Colors[ImGuiCol_TitleBgActive]        = ImVec4(1.00f, 0.71f, 0.00f, 1.00f);
			style.Colors[ImGuiCol_MenuBarBg]            = ImVec4(1.00f, 1.00f, 0.00f, 0.39f);
			style.Colors[ImGuiCol_ScrollbarBg]          = ImVec4(0.24f, 0.24f, 0.24f, 0.12f);
			style.Colors[ImGuiCol_ScrollbarGrab]        = ImVec4(0.24f, 0.24f, 0.24f, 0.47f);
			style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.24f, 0.24f, 0.24f, 0.39f);
			style.Colors[ImGuiCol_ScrollbarGrabActive]  = ImVec4(0.24f, 0.24f, 0.24f, 0.78f);
			style.Colors[ImGuiCol_CheckMark]            = ImVec4(1.00f, 0.39f, 0.00f, 0.78f);
			style.Colors[ImGuiCol_SliderGrab]           = ImVec4(1.00f, 0.59f, 0.00f, 0.78f);
			style.Colors[ImGuiCol_SliderGrabActive]     = ImVec4(1.00f, 0.59f, 0.00f, 1.00f);
			style.Colors[ImGuiCol_Button]               = ImVec4(1.00f, 0.78f, 0.24f, 1.00f);
			style.Colors[ImGuiCol_ButtonHovered]        = ImVec4(1.00f, 0.78f, 0.24f, 0.78f);
			style.Colors[ImGuiCol_ButtonActive]         = ImVec4(0.94f, 0.71f, 0.16f, 1.00f);
			style.Colors[ImGuiCol_Header]               = ImVec4(1.00f, 0.78f, 0.24f, 0.86f);
			style.Colors[ImGuiCol_HeaderHovered]        = ImVec4(1.00f, 0.71f, 0.00f, 1.00f);
			style.Colors[ImGuiCol_HeaderActive]         = ImVec4(1.00f, 0.78f, 0.24f, 1.00f);
			style.Colors[ImGuiCol_ResizeGrip]           = ImVec4(0.24f, 0.24f, 0.24f, 0.63f);
			style.Colors[ImGuiCol_ResizeGripHovered]    = ImVec4(1.00f, 0.47f, 0.00f, 0.71f);
			style.Colors[ImGuiCol_ResizeGripActive]     = ImVec4(1.00f, 0.47f, 0.00f, 1.00f);
			style.Colors[ImGuiCol_PlotLines]            = ImVec4(0.16f, 0.16f, 0.16f, 0.78f);
			style.Colors[ImGuiCol_PlotLinesHovered]     = ImVec4(1.00f, 0.70f, 0.00f, 1.00f);
			style.Colors[ImGuiCol_PlotHistogram]        = ImVec4(1.00f, 0.63f, 0.00f, 1.00f);
			style.Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.43f, 0.00f, 1.00f);
			style.Colors[ImGuiCol_TextSelectedBg]       = ImVec4(1.00f, 0.43f, 0.00f, 1.00f);
		}
		inline void set_stile_colors_orange()
		{
			ImVec4* colors                         = ImGui::GetStyle().Colors;
			colors[ImGuiCol_Text]                  = ImVec4(0.08f, 0.08f, 0.08f, 1.00f);
			colors[ImGuiCol_TextDisabled]          = ImVec4(0.60f, 0.60f, 0.60f, 1.00f);
			colors[ImGuiCol_WindowBg]              = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
			colors[ImGuiCol_PopupBg]               = ImVec4(1.00f, 1.00f, 1.00f, 0.94f);
			colors[ImGuiCol_Border]                = ImVec4(0.00f, 0.01f, 0.00f, 0.24f);
			colors[ImGuiCol_FrameBg]               = ImVec4(0.59f, 0.59f, 0.59f, 0.24f);
			colors[ImGuiCol_FrameBgHovered]        = ImVec4(0.78f, 0.78f, 0.78f, 1.00f);
			colors[ImGuiCol_FrameBgActive]         = ImVec4(0.35f, 0.35f, 0.35f, 0.35f);
			colors[ImGuiCol_TitleBg]               = ImVec4(0.78f, 0.78f, 0.78f, 1.00f);
			colors[ImGuiCol_TitleBgActive]         = ImVec4(1.00f, 0.71f, 0.00f, 1.00f);
			colors[ImGuiCol_TitleBgCollapsed]      = ImVec4(0.78f, 0.78f, 0.78f, 0.90f);
			colors[ImGuiCol_MenuBarBg]             = ImVec4(1.00f, 1.00f, 0.00f, 0.39f);
			colors[ImGuiCol_ScrollbarBg]           = ImVec4(0.24f, 0.24f, 0.24f, 0.12f);
			colors[ImGuiCol_ScrollbarGrab]         = ImVec4(0.24f, 0.24f, 0.24f, 0.59f);
			colors[ImGuiCol_ScrollbarGrabHovered]  = ImVec4(0.24f, 0.24f, 0.24f, 0.39f);
			colors[ImGuiCol_ScrollbarGrabActive]   = ImVec4(0.24f, 0.24f, 0.24f, 0.78f);
			colors[ImGuiCol_CheckMark]             = ImVec4(1.00f, 0.39f, 0.00f, 0.78f);
			colors[ImGuiCol_SliderGrab]            = ImVec4(1.00f, 0.59f, 0.00f, 0.78f);
			colors[ImGuiCol_SliderGrabActive]      = ImVec4(1.00f, 0.59f, 0.00f, 1.00f);
			colors[ImGuiCol_Button]                = ImVec4(1.00f, 0.78f, 0.24f, 1.00f);
			colors[ImGuiCol_ButtonHovered]         = ImVec4(1.00f, 0.78f, 0.24f, 0.78f);
			colors[ImGuiCol_ButtonActive]          = ImVec4(0.94f, 0.71f, 0.16f, 1.00f);
			colors[ImGuiCol_Header]                = ImVec4(1.00f, 0.78f, 0.24f, 0.86f);
			colors[ImGuiCol_HeaderHovered]         = ImVec4(1.00f, 0.71f, 0.00f, 1.00f);
			colors[ImGuiCol_HeaderActive]          = ImVec4(1.00f, 0.78f, 0.24f, 1.00f);
			colors[ImGuiCol_Separator]             = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
			colors[ImGuiCol_SeparatorHovered]      = ImVec4(0.70f, 0.60f, 0.60f, 1.00f);
			colors[ImGuiCol_SeparatorActive]       = ImVec4(0.90f, 0.70f, 0.70f, 1.00f);
			colors[ImGuiCol_ResizeGrip]            = ImVec4(0.24f, 0.24f, 0.24f, 0.63f);
			colors[ImGuiCol_ResizeGripHovered]     = ImVec4(1.00f, 0.47f, 0.00f, 0.71f);
			colors[ImGuiCol_ResizeGripActive]      = ImVec4(1.00f, 0.47f, 0.00f, 1.00f);
			colors[ImGuiCol_Tab]                   = ImVec4(0.76f, 0.80f, 0.84f, 0.93f);
			colors[ImGuiCol_TabActive]             = ImVec4(0.60f, 0.73f, 0.88f, 1.00f);
			colors[ImGuiCol_TabUnfocused]          = ImVec4(0.92f, 0.93f, 0.94f, 0.99f);
			colors[ImGuiCol_TabUnfocusedActive]    = ImVec4(0.74f, 0.82f, 0.91f, 1.00f);
			colors[ImGuiCol_PlotLines]             = ImVec4(0.16f, 0.16f, 0.16f, 0.78f);
			colors[ImGuiCol_PlotLinesHovered]      = ImVec4(1.00f, 0.70f, 0.00f, 1.00f);
			colors[ImGuiCol_PlotHistogram]         = ImVec4(1.00f, 0.63f, 0.00f, 1.00f);
			colors[ImGuiCol_PlotHistogramHovered]  = ImVec4(1.00f, 0.43f, 0.00f, 1.00f);
			colors[ImGuiCol_TextSelectedBg]        = ImVec4(1.00f, 0.43f, 0.00f, 1.00f);
			colors[ImGuiCol_DragDropTarget]        = ImVec4(0.26f, 0.59f, 0.98f, 0.95f);
			colors[ImGuiCol_NavHighlight]          = ImVec4(0.26f, 0.59f, 0.98f, 0.80f);
			colors[ImGuiCol_NavWindowingHighlight] = ImVec4(0.70f, 0.70f, 0.70f, 0.70f);
			colors[ImGuiCol_NavWindowingDimBg]     = ImVec4(0.20f, 0.20f, 0.20f, 0.20f);
			colors[ImGuiCol_ModalWindowDimBg]      = ImVec4(0.20f, 0.20f, 0.20f, 0.31f);
		}

		struct gui_2d_object
		{
			FLOAT2 Pos;
			FLOAT2 Size;
		};

		struct Region : gui_2d_object
		{
		private:
			float croped_line_len;
			float stable_line_len;
			float region_1_size;
			float region_1_aspect;
			float region_2_size;
			float region_2_aspect;

			bool horisontal;
			bool size_in_ratio;

			enum class resize_tupe
			{
				save_aspect,
				drag_1_region,
				drag_2_region,
			};

			//TODO struct SUBREGION
            // getFirst
            // getSecond
            // getRegion[0-1]

		protected:
			void SetSize(float size_in_pixels)
			{
				if (size_in_pixels > croped_line_len) { size_in_pixels = croped_line_len; }
				else if (size_in_pixels < -croped_line_len) { size_in_pixels = -croped_line_len; }

				bool swap = size_in_pixels < 0.0f;
				size_in_pixels = std::abs(size_in_pixels);

				region_1_size = size_in_pixels;
				region_2_size = croped_line_len - region_1_size;

				if (swap)
					std::swap(region_1_size, region_2_size);

				size_in_ratio = false;
			}
			void SetSizeRatio(float aspect)
			{
				if (aspect > 1.0f) { aspect = 1.0f; }
				else if (aspect < -1.0f) { aspect = -1.0f; }

				bool swap = aspect < 0.0f;
				aspect = std::abs(aspect);

				region_1_aspect = aspect;
				region_2_aspect = 1.0f - region_1_aspect;

				if (swap)
				{
					float tmp = region_1_aspect;
					region_1_aspect = region_2_aspect;
					region_2_aspect = tmp;
				}

				size_in_ratio = true;
			}
			void InitRegion(FLOAT2 x0y0, FLOAT2 wh, bool hor)
			{
				Pos = x0y0;
				Size = wh;
				horisontal = hor;
				croped_line_len = horisontal ? Size.x : Size.y;
				stable_line_len = horisontal ? Size.y : Size.x;
			}

		public:
			void InitRegion_Ratio(FLOAT2 x0y0, FLOAT2 wh, float ratio, bool hor = true) { InitRegion(x0y0, wh, hor); SetSizeRatio(ratio); }
			void InitRegion_Sized(FLOAT2 x0y0, FLOAT2 wh, float size, bool hor = true) { InitRegion(x0y0, wh, hor); SetSize(size); }

			void InitRegion_Ratio(FLOAT4 x0y0wh, float ratio, bool hor = true) { InitRegion({ x0y0wh.x, x0y0wh.y }, { x0y0wh.z, x0y0wh.w }, hor); SetSizeRatio(ratio); }
			void InitRegion_Sized(FLOAT4 x0y0wh, float size, bool hor = true) { InitRegion({ x0y0wh.x, x0y0wh.y }, { x0y0wh.z, x0y0wh.w }, hor); SetSize(size); }

		public:
			FLOAT2 GetPosA(FLOAT2 padding = { 0, 0 }) { return Pos + padding; }
			FLOAT2 GetPosB(FLOAT2 pad = { 0, 0 })
			{
				FLOAT2 out;

				if (horisontal)	out = FLOAT2(Pos.x + GetSizeA().x, Pos.y);
				else			out = FLOAT2(Pos.x, GetSizeA().y + Pos.y);

				return out + pad;
			}
            FLOAT2 GetSizeA(FLOAT2 pad = { 0, 0 }) { return getRegionSize(true)  - getPadding(pad); }
            FLOAT2 GetSizeB(FLOAT2 pad = { 0, 0 }) { return getRegionSize(false) - getPadding(pad); }

			FLOAT4 GetRegionA(FLOAT2 pad = { 0, 0 })
			{
				FLOAT2 p = GetPosA(pad);
				FLOAT2 s = GetSizeA(pad);
				return FLOAT4(p.x, p.y, s.x, s.y);
			}

			FLOAT4 GetRegionB(FLOAT2 pad = { 0, 0 })
			{
				FLOAT2 p = GetPosB(pad);
				FLOAT2 s = GetSizeB(pad);
				return FLOAT4(p.x, p.y, s.x, s.y);
			}

		private:
			FLOAT2 getPadding(FLOAT2 pad = { 0, 0 })
			{
				return FLOAT2(pad.x * 2.0f, pad.y * 2.0f);
			}


			FLOAT2 getRegionSize(bool isRegionA)
			{
				float croped = croped_line_len;
				float stable = stable_line_len;

				float _ratio;
				float _sizeo;

				if (isRegionA)
				{
					_ratio = region_1_aspect;
					_sizeo = region_2_size;
				}
				else
				{
					_ratio = region_2_aspect;
					_sizeo = region_1_size;
				}

				if (size_in_ratio) { croped = croped * _ratio; }
				else { croped = croped - _sizeo; }

				FLOAT2 out;

				if (horisontal) out = FLOAT2(croped, stable);
				else			out = FLOAT2(stable, croped);

				return out;
			}

		};
		struct Grid : gui_2d_object
		{
			void InitRegion(FLOAT4 x0y0wh, int w_colums, int h_colums)
			{
				assert(math::InRange(w_colums, 1, 100));
				assert(math::InRange(h_colums, 1, 100));

				Pos = { x0y0wh.x, x0y0wh.y };
				Size = { x0y0wh.z, x0y0wh.w };

				hor_colums_num = w_colums;
				ver_colums_num = h_colums;

				hor_colum_size = Size.x / w_colums;
				ver_colum_size = Size.y / h_colums;
			}



			size_t hor_colums_num;
			size_t ver_colums_num;

			float hor_colum_size;
			float ver_colum_size;

		public:

			FLOAT2 GetRegionPos(size_t w_id, size_t h_id, FLOAT2 padding = { 0, 0 })
			{
				assert(math::InRange(w_id, 0, hor_colums_num - 1));
				assert(math::InRange(h_id, 0, ver_colums_num - 1));

				FLOAT2 out;

				out.x = Pos.x + hor_colum_size * w_id + padding.x;
				out.y = Pos.y + ver_colum_size * h_id + padding.y;

				return out;
			}
			FLOAT2 GetRegionSize(size_t w_id, size_t h_id, FLOAT2 padding = { 0, 0 })
			{
				assert(math::InRange(w_id, 0, hor_colums_num - 1));
				assert(math::InRange(h_id, 0, ver_colums_num - 1));

				FLOAT2 out;

				out.x = hor_colum_size - padding.x * 2;
				out.y = ver_colum_size - padding.y * 2;

				return out;
			}
			FLOAT4 GetRegion(size_t w_id, size_t h_id, FLOAT2 padding = { 0, 0 })
			{
				FLOAT2 pos = GetRegionPos(w_id, h_id, padding);
				FLOAT2 size = GetRegionSize(w_id, h_id, padding);

				return FLOAT4(pos.x, pos.y, size.x, size.y);
			}
		};
	}

	using namespace helpers;

	inline void Text(const char* str) { ImGui::TextUnformatted(str); }
	inline void Text(const std::string& str) { ImGui::TextUnformatted(str.c_str()); }
	template <typename... Args> inline void Text(const char* str, Args&& ... args)        { Text(str::format::insert(str, std::forward<Args>(args)...)); }
	template <typename... Args> inline void Text(const std::string& str, Args&& ... args) { Text(str::format::insert(str, std::forward<Args>(args)...)); }

	// mb dell
	inline void RawText(const char* str)        { ImGui::TextUnformatted(str); }
	inline void RawText(const std::string& str)	{ ImGui::TextUnformatted(str.c_str()); }
	//template <typename... Args>
	//inline void TextFmt(const std::string& str, Args&& ... args)
	//{
	//	ImGui::Text(str::format::insert(str.c_str(), std::forward<Args>(args)...));
	//}

	//////////////////////////////////////////////////////////////////////////

	enum
	{
		TS_BOLD,
	};

	template <typename... Args>
	static void __todell_TextFmtStiled(const std::string& in, int32_t stile, Args&& ... args)
	{
		//if (stile == TS_BOLD)
		//{
		//	ImGuiIO& io = ImGui::GetIO();
		//	ImFont* font_current = ImGui::GetFont();
		//	ImFont* font_target = nullptr;
		//	std::string fnt_name = "RobotoMedium.ttf, 16px";
		//
		//	for (int n = 0; n < io.Fonts->Fonts.Size; n++)
		//	{
		//		ImFont* font = io.Fonts->Fonts[n];
		//		if (std::string(font->GetDebugName()) == fnt_name)
		//			font_target = font;
		//	}
		//
		//	if (font_target)
		//	{
		//		ImGui::SetCurrentFont(font_target);
		//		ImGui::TextFmt(in, std::forward<Args>(args)...);
		//		ImGui::SetCurrentFont(font_current);
		//	}
		//	else
		//	{
		//		ImGui::TextFmt("[-bold]" + in, std::forward<Args>(args)...);
		//	}
		//}
		//else
		//	ImGui::TextFmt(in, std::forward<Args>(args)...);
	}

	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////
	using namespace graphics;

	inline bool ShowImage(GpuTexture* gpu_img, bool context_popup = true, FLOAT2 size = FLOAT2{ 0,0 })
	{
		if (size.x == 0 && size.y == 0)
		{
			size.x = static_cast<float>(gpu_img->GetSize().x);
			size.y = static_cast<float>(gpu_img->GetSize().y);
		}

		bool gpu_data_changed = false;

		const FLOAT2 cpos = ImGui::GetCursorPos();
		const FLOAT2 wpos = ImGui::GetWindowPos();

		// may be change id impl

		ImGui::Image((ImTextureID)uint64_t(gpu_img->GetImpl()->uid), size);

		ImGui::PushID(gpu_img->GetImpl()->uid);
		if (context_popup && ImGui::BeginPopupContextItem("bitmap_context_menu"))
		{
			ImGui::Text("Bitmap context menu");

			ImGui::Separator();
			ImItem::Text("data size : {} x {}", gpu_img->GetSize().x, gpu_img->GetSize().y);
			ImItem::Text("view size : {} x {}", size.x, size.y);
			ImItem::Text("content   : RGBA");

			ImGui::Separator();
			if (ImGui::Button("Copy to clipboard (RGB*)"))
			{
				bitmap32 image;
				gpu_img->GetData(image);

				for (int i = 0; i < image.pixel_count(); i++)
                    std::swap(image.raw_data(i)->r, image.raw_data(i)->b);

				os_api::clipboard::SetClipboardImage(image);

				ImGui::CloseCurrentPopup();
			}
			if (ImGui::Button("Copy as string (RGBA)"))
			{
				bitmap32 image;
				gpu_img->GetData(image);

				std::string str_buff = bitmap_converter::bitmap_to_string(image);

				os_api::clipboard::SetClipboardText(str_buff);

				ImGui::CloseCurrentPopup();
			}

			ImGui::Separator();
			if (ImGui::Button("Paste image (RGB*)"))
			{
				auto result = os_api::clipboard::GetClipboardImage();

				if (result.IsSucceed())
				{
					bitmap32 image = std::move(result.GetValue());

					for (int i = 0; i < image.pixel_count(); i++)
						std::swap(image.raw_data(i)->r, image.raw_data(i)->b);

					gpu_img->Update(image);
					gpu_data_changed = true;
				}

				ImGui::CloseCurrentPopup();
			}

			ImGui::EndPopup();

			FLOAT2 A = cpos + wpos;
			FLOAT2 B = cpos + wpos + size;

			ImGui::GetWindowDrawList()->AddRectFilled(A, B, IM_COL32(255, 255, 255, 60));
			ImGui::GetWindowDrawList()->AddRect(A, B, IM_COL32(255, 255, 255, 99));

		}
		ImGui::PopID();

		return gpu_data_changed;
	}


	//////////////////////////////////////////////////////////////////////////
}