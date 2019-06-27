#pragma once
#include "pch.h"
#include "imview.h"

//////////////////////////////////////////////////////////////////////////

namespace
{
	struct array_adpapter
	{
		enum action_id
		{
			_get,
			_set,
			_swap,
			_compare,
		};
		struct action_data
		{
			action_id act_id;
			int a;
			int b;
		};

		std::vector<int>& data = std::vector<int>(0, 0);
		std::vector<int> data_buffer;
		std::vector<action_data> actions;

	public:
		void BindArray(std::vector<int>& dat)
		{
			data = dat;
			data_buffer = dat;
			actions.clear();
		}

		action_data PlayAction(int id)
		{
			action_data act = actions[id];

			switch (act.act_id)
			{
			case _get:		break;
			case _set:		data_buffer[act.a] = act.b; break;
			case _swap:		std::swap(data_buffer[act.a], data_buffer[act.b]); break;
			case _compare:	break;
			}

			return act;
		}

	public:
		int  get(int id)            { actions.push_back({ _get,    id,0 });     return data[id];             }
		void set(int id, int value) { actions.push_back({ _set,    id,value }); data[id] = value;            }
		void swap(int a, int b)     { actions.push_back({ _swap,   a,b });      std::swap(data[a], data[b]); }
		bool compare(int a, int b)  { actions.push_back({ _compare,a,b });      return data[a] < data[b];    }
	};

    void sub_merge(array_adpapter& data, int64_t start, int64_t end, int64_t* _metric_acc, int64_t* _metric_comp)
    {
        int64_t middle = start + (end - start) / 2;

        if (middle > start)
        {
            sub_merge(data, start, middle, _metric_acc, _metric_comp);
            sub_merge(data, middle + 1, end, _metric_acc, _metric_comp);
        }

        vector<double> sorted(end - start + 1);

        int64_t left_it = start;
        int64_t right_it = middle + 1;

        for (int i = 0; i < sorted.size(); i++)
        {
            //------------------------------------------// if one of iters reach the end
            if (left_it > middle || right_it > end)
            {
                if (left_it > middle)
                    sorted[i] = data.get(right_it++);//(data[right_it++]);
                else
                    if (right_it > end)
                        sorted[i] = data.get(left_it++); //(data[left_it++]);

                (*_metric_acc)++;
            }
            //------------------------------------------// else compare iter-values and add to buffer
            else
            {
                if (data.compare(left_it, right_it))
                    sorted[i] = data.get(left_it++);
                else
                    sorted[i] = data.get(right_it++);

                (*_metric_comp)++;   // metric
                (*_metric_acc) += 3; // metric
            }
        }

        //std::copy(sorted.begin(), sorted.end(), &data.get(start));

        ForArray(i, sorted.size())
        {
            data.set(start + i, sorted[i]);
        }

        (*_metric_acc) += sorted.size(); // metric

    }

    void buble_sort(array_adpapter& data)
    {
        size_t it = 0;
        size_t size = data.data.size() - 1;

        bool was_swap = 0;

        while (was_swap || it < size)
        {
            if (false == data.compare(it, it + 1))
            {
                data.swap(it, it + 1);
                was_swap = true;
            }

            if (++it + 1 > size && was_swap)
            {
                it = 0;
                was_swap = false;
            }

        }
    }

    void heapify(array_adpapter& data, int64_t id, int end)
    {
        int64_t left = id * 2 + 1;
        int64_t right = id * 2 + 2;
        int64_t swap_it = id;

        if (left <= end)
            if (data.compare(swap_it, left))
                swap_it = left;

        if (right <= end)
            if (data.compare(swap_it, right))
                swap_it = right;

        if (swap_it != id)
        {
            data.swap(id, swap_it);
            heapify(data, swap_it, end);
        }

    }

    void build_heap(array_adpapter& data, int64_t end)
    {
        int64_t parent = end;

        while (parent >= 0)
            heapify(data, parent--, end);
    }

    void heap_sort(array_adpapter& data)
    {
        int64_t it = 0;
        int64_t end = data.data.size() - 1;

        build_heap(data, end);

        while (end >= 0)
        {
            data.swap(0, end--);
            heapify(data, 0, end);
        }

        return;
    }
}

void imview::sortView(view_context*)
{
	//ImGui::SetNextWindowCollapsed(false, ImGuiCond_::ImGuiCond_Appearing);
	//ImGui::SetNextWindowSize(ImVec2(1280, 860), ImGuiCond_Once);
	//if (ImGui::Begin("SortView"))
	{
		enum sort_constants
		{
			sort_size = 256,
		};

		static GpuTexture gpu_tex;
		static bitmap32	  ram_tex(sort_size, sort_size);

		static std::vector<int> data(sort_size);
		static array_adpapter adp;

		MAKE_ONCE
		(
			ForArray(i, sort_size)
				data[i] = i;

			adp.BindArray(data);
		)

		if (ImGui::Button("Bad case"))
		{
			ForArray(i, sort_size)
				data[i] = sort_size - i;

			adp.BindArray(data);
		}

		ImGui::SameLine();
		if (ImGui::Button("Good case"))
		{
			ForArray(i, sort_size)
				data[i] = i;

			adp.BindArray(data);
		}

		ImGui::SameLine();
		if (ImGui::Button("Randomize"))
		{
			ForArray(i, sort_size)
				std::swap(data[i], data[math::Random(0, (int)sort_size - 1)]);

			adp.BindArray(data);
		}

		//-------------------------------------------------------------//
		static int  act_id = -1;
		static SuperTimer timer;

		struct player
		{
			int64_t writes = 0;
			int64_t reads = 0;
			int64_t swaps = 0;
			int64_t compares = 0;

			bool is_play = false;
			float frame_timeout = 0;

			void play()
			{
				writes = 0;
				reads = 0;
				swaps = 0;
				compares = 0;
				is_play = true;
			}

		};

		static player animator;

		//-------------------------------------------------------------//

		if (ImGui::Button("Buble-Sort"))
		{
			timer.SetStart();
			adp.actions.clear();
			buble_sort(adp);
			timer.SetEnd();
			log::info << std::to_string(timer.GetRoundTime());

			animator.play();
		}

		ImGui::SameLine();

		if (ImGui::Button("Merge-Sort"))
		{
			int64_t start = 0;
			int64_t end = data.size() - 1;
			int64_t acc = 0;
			int64_t comp = 0;

			timer.SetStart();
			adp.actions.clear();
			sub_merge(adp, start, end, &acc, &comp);
			timer.SetEnd();
			log::info << std::to_string(timer.GetRoundTime());

			animator.play();
		}

		ImGui::SameLine();

		if (ImGui::Button("Heap-Sort"))
		{
			int64_t start = 0;
			int64_t end = data.size() - 1;
			int64_t acc = 0;
			int64_t comp = 0;

			timer.SetStart();
			adp.actions.clear();
			heap_sort(adp);
			timer.SetEnd();
			log::info << std::to_string(timer.GetRoundTime());

			animator.play();
		}

		//-------------------------------------------------------------//

		if (animator.is_play)
		{
			if (act_id + 1 < (int)adp.actions.size())
			{
				act_id++;

				static SuperTimer timer;
				static float esc = 0;

				esc += timer.GetRoundTime();

				if (esc > animator.frame_timeout)
				{
					esc = 0;
					auto act = adp.PlayAction(act_id);

					if (act.act_id == array_adpapter::_get)     animator.reads++;
					if (act.act_id == array_adpapter::_set)     animator.writes++;
					if (act.act_id == array_adpapter::_swap)    animator.swaps++;
					if (act.act_id == array_adpapter::_compare) animator.compares++;
				}
			}
			else
			{
				animator.is_play = false;
				act_id = 0;
			}
		}


		ForArray(w, ram_tex.size().x)
			ForArray(h, ram_tex.size().y)
		{
			if ((sort_size - h) < adp.data_buffer[w])
				*ram_tex.raw_data(w, h) = { 255,255,255,255 };
			else
				*ram_tex.raw_data(w, h) = { 0, 0, 0, 0 };
		}

		if (animator.is_play)
		{
			auto act = adp.actions[act_id];

			static color32 col_read = { 0, 0, 255, 255 };
			static color32 col_write = { 255, 0, 0, 255 };
			static color32 col_swap = { 0, 0, 255, 255 }; // { 0, 255, 0, 255 };
			static color32 col_compare = { 255, 0, 0, 255 }; // { 255, 0, 255, 255 };

			switch (act.act_id)
			{
			case array_adpapter::action_id::_get:

				ForArray(i, ram_tex.size().y)
					if ((sort_size - i) < adp.data_buffer[act.a])*ram_tex.raw_data(act.a, i) = col_read;
				break;

			case array_adpapter::action_id::_set:

				ForArray(i, ram_tex.size().y)
					if ((sort_size - i) < adp.data_buffer[act.a])*ram_tex.raw_data(act.a, i) = col_write;
				break;

			case array_adpapter::action_id::_swap:

				ForArray(i, ram_tex.size().y)
				{
					if ((sort_size - i) < adp.data_buffer[act.a])*ram_tex.raw_data(act.a, i) = col_swap;
					if ((sort_size - i) < adp.data_buffer[act.b])*ram_tex.raw_data(act.b, i) = col_swap;
				}
				break;

			case array_adpapter::action_id::_compare:

				ForArray(i, ram_tex.size().y)
				{
					if ((sort_size - i) < adp.data_buffer[act.a])*ram_tex.raw_data(act.a, i) = col_compare;
					if ((sort_size - i) < adp.data_buffer[act.b])*ram_tex.raw_data(act.b, i) = col_compare;
				}
				break;
			}
		}

		////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		ImGui::Text(str::format::insert("Reads: {}", animator.reads).c_str()); ImGui::SameLine();
		ImGui::Text(str::format::insert("| Writes: {}", animator.writes).c_str()); ImGui::SameLine();
		ImGui::Text(str::format::insert("| Swaps: {}", animator.swaps).c_str()); ImGui::SameLine();
		ImGui::Text(str::format::insert("| Compares: {}", animator.compares).c_str());

		gpu_tex.Update(ram_tex);
		ImGui::Image((ImTextureID)gpu_tex.GetImpl()->uid, FLOAT2 { 4, 2 } *sort_size);
	}
}
