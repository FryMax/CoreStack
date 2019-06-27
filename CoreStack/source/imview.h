#pragma once
#include "pch.h"

//todell?
#include "MyInput.h"

namespace imview
{
    struct view_context
    {

    };


    void wifiMap     (view_context* = nullptr);
    void sortView    (view_context* = nullptr);
    void corbaLog    (view_context* = nullptr);
    void suncBench   (view_context* = nullptr);
    void bezierCurve (view_context* = nullptr);
    void openCV      (view_context* = nullptr);
    void openCL      (view_context* = nullptr);

}

namespace imview
{
    static void resize_map()
    {
        static bitmap32 img;
        static bitmap32 imgNearA;
        static bitmap32 imgNearB;
        static bitmap32 imgLenearA;
        static bitmap32 imgLenearB;

        static GpuTexture gpu_img;
        static GpuTexture gpu_imgNearA;
        static GpuTexture gpu_imgNearB;
        static GpuTexture gpu_imgLenearA;
        static GpuTexture gpu_imgLenearB;


        static double timeNearA = 0;
        static double timeNearB = 0;
        static double timeLenearA = 0;
        static double timeLenearB = 0;

        if (ImGui::Button("update"))
        {
            const INT2 sizeScr = { 20,40 };
            const INT2 size = sizeScr * 8;

            img = art::draw::chess_board(sizeScr, 8);
            SuperTimer st;

            //------------------------//
            imgNearA = img;
            st.SetStart();
            {
                imgNearA.resize(size, scalefilter::nearest);
            }
            st.SetEnd();
            timeNearA = st.GetTimeReal();
            //------------------------//
            imgNearB.resize(size, scalefilter::no_scale);
            st.SetStart();
            {
                //stbir__resize_arbitrary(
                //	nullptr,
                //	reinterpret_cast<uint8_t*>(img.raw_data()),
                //	img.size().x,
                //	img.size().y,
                //	4 * img.size().x,
                //	reinterpret_cast<uint8_t*>(imgNearB.raw_data()),
                //	imgNearB.size().x,
                //	imgNearB.size().y,
                //	4 * imgNearB.size().x,
                //	0,
                //	0,
                //	1,
                //	1,
                //	NULL,
                //	4,
                //	-1,
                //	0,
                //	STBIR_TYPE_UINT8,
                //	STBIR_FILTER_BOX,
                //	STBIR_FILTER_BOX,
                //	STBIR_EDGE_CLAMP,
                //	STBIR_EDGE_CLAMP,
                //	STBIR_COLORSPACE_LINEAR);
            }
            st.SetEnd();
            timeNearB = st.GetTimeReal();
            //------------------------//
            imgLenearA = img;
            st.SetStart();
            {
                imgLenearA.resize(size, scalefilter::bilinear);
            }
            st.SetEnd();
            timeLenearA = st.GetTimeReal();
            //------------------------//
            imgLenearB.resize(size, scalefilter::no_scale);
            st.SetStart();
            {
                //stbir__resize_arbitrary(NULL,
                //	(uint8_t*)img.raw_data(), img.size().x, img.size().y, 4 * img.size().x,
                //	(uint8_t*)imgLenearB.raw_data(), imgLenearB.size().x, imgLenearB.size().y, 4 * imgLenearB.size().x,
                //	0, 0, 1, 1, NULL, 4, -1, 0,
                //	STBIR_TYPE_UINT8, STBIR_FILTER_TRIANGLE, STBIR_FILTER_TRIANGLE, STBIR_EDGE_CLAMP, STBIR_EDGE_CLAMP, STBIR_COLORSPACE_LINEAR);
            }
            st.SetEnd();
            timeLenearB = st.GetTimeReal();
            //------------------------//


            //stbir_resize_uint8(
            //	(uint8_t*)img.raw_data(),  // const unsigned char* input_pixels,
            //	img.size().x,              // int input_w,
            //	img.size().y,              // int input_h,
            //	4 * img.size().x,              // int input_stride_in_bytes,
            //	(uint8_t*)imgB.raw_data(), // unsigned char* output_pixels,
            //	imgB.size().x,             // int output_w,
            //	imgB.size().y,             // int output_h,
            //	4 * imgB.size().x,             // int output_stride_in_bytes,
            //	4                          // int num_channels
            //	);

            //timeLenearB = st.GetTimeReal();

            gpu_img.Update(img);
            gpu_imgNearA.Update(imgNearA);
            gpu_imgNearB.Update(imgNearB);
            gpu_imgLenearA.Update(imgLenearA);
            gpu_imgLenearB.Update(imgLenearB);

            //stbir_resize_uint8_generic(outB.raw_data(),
            //	outB.size().,//const unsigned char* input_pixels,
            //	0,//int input_w,
            //	0,//int input_h,
            //	0,//int input_stride_in_bytes,
            //	0,//unsigned char* output_pixels,
            //	0,//int output_w,
            //	0,//int output_h,
            //	0,//int output_stride_in_bytes,
            //	0,//int num_channels,
            //	0,//int alpha_channel,
            //	0,//int flags,
            //	0,//stbir_edge edge_wrap_mode,
            //	0,//stbir_filter filter,
            //	0,//stbir_colorspace space,
            //	0,//void* alloc_context);
            //	);

            //NULL, input_pixels, input_w, input_h, input_stride_in_bytes,
            //output_pixels, output_w, output_h, output_stride_in_bytes,
            //0, 0, 1, 1, NULL, num_channels, -1, 0, STBIR_TYPE_UINT8, STBIR_FILTER_DEFAULT, STBIR_FILTER_DEFAULT,
            //STBIR_EDGE_CLAMP, STBIR_EDGE_CLAMP, STBIR_COLORSPACE_LINEAR
        }

        //ImGui::InputFloat("billinear correction", &G_POS_CORRECTION);
        //ImGui::InputFloat("billinear correctionA", &G_POS_CORRECTION_A);
        //ImGui::InputFloat("billinear correctionB", &G_POS_CORRECTION_B);
        ImGui::Text(str::format::insert("{} | {}", timeNearA, timeNearB).c_str());
        ImGui::Text(str::format::insert("{} | {}", timeLenearA, timeLenearB).c_str());
        ImItem::ShowImage(&gpu_img, true, { 256,256 });
        ImGui::Separator();
        ImItem::ShowImage(&gpu_imgNearA, true, { 512,512 }); ImGui::SameLine();
        ImItem::ShowImage(&gpu_imgNearB, true, { 512,512 });
        ImGui::Separator();
        ImItem::ShowImage(&gpu_imgLenearA, true, { 512,512 }); ImGui::SameLine();
        ImItem::ShowImage(&gpu_imgLenearB, true, { 512,512 });
    }

    //-------------------------------------------------------//

    static void umanager_view()
    {
        if (ImGui::Button("Create100uobjects"))
        {
            class udummu : utka::uclass
            {
            public:
                udummu() : UCLASS("udummy")
                {
                    static int jojo;
                    _register_prop(&jojo, "jojo prop");
                }
            };

            ForArray(i, 100)
            {
                static std::vector<udummu*> vec;
                vec.push_back(new udummu);
            }

        }
        //////////////////////////////////////////////////////////////////////////
        const FLOAT2 wsize = ImGui::GetContentRegionAvail();

        const auto& UMC = utka::UManagerClass;
        const auto& UMA = UMC.get_meta_array();

        static const utka::uclass_pin* selected = nullptr;
        bool selected_exist = false;

        //-----------------------------------------------------------------------------------------
        // uview left side
        //-----------------------------------------------------------------------------------------
        ImGui::BeginChildFrame(ImGui::GetID("uview_left_regin"), FLOAT2{ wsize.x * 0.33f, wsize.y });
        for (const auto& uit : UMA)
        {
            const std::string label = str::format::insert("{}:{}", uit.meta.obj_name, uit.id);

            if (ImGui::Selectable(label.c_str(), selected == &uit))
            {
                selected = &uit;
            }

            if (selected == &uit)
                selected_exist = true;
        }

        // selected was removed
        if (!selected_exist)
            selected = nullptr;

        ImGui::EndChildFrame();

        //-----------------------------------------------------------------------------------------
        // right side
        //-----------------------------------------------------------------------------------------
        ImGui::SameLine();
        ImGui::BeginChildFrame(ImGui::GetID("uview_right_regin"), FLOAT2{ wsize.x * 0.66f, wsize.y });
        if (selected)
        {
            const utka::uclass_pin& uit = *selected;

            ImItem::Text("meta:");
            ImItem::Text("Name: {}", uit.meta.obj_name);
            ImItem::Text("Type: {}", uit.meta.obj_type);
            ImItem::Text("Func: {}", uit.meta.obj_func);
            ImItem::Text("Dest: {}:{}", uit.meta.obj_file, uit.meta.obj_line);

            //---------------------------------------------- props
            ImGui::Separator();
            ImItem::Text("props({}):", uit.props.size());
            if (!uit.props.empty())
            {
                ImGui::Dummy({ 6,6 });
                ImGui::Columns(4, "_prop_columns");
                ImItem::Text("NAME");   ImGui::NextColumn();
                ImItem::Text("TYPE");   ImGui::NextColumn();
                ImItem::Text("ACCESS"); ImGui::NextColumn();
                ImItem::Text("VALUE");  ImGui::NextColumn();
                ImGui::Separator();

                int prop_id = 0;
                for (const auto& propit : uit.props)
                {
                    static auto const draw_prop = [&prop_id](const utka::uprop_meta& prop)
                    {
                        static auto const type_to_name = [&prop_id](const utka::uval_type& type) -> const char*
                        {
                            switch (type)
                            {
                            case utka::uval_type::u_int8:  return "int8";
                            case utka::uval_type::u_int16: return "int16";
                            case utka::uval_type::u_int32: return "int32";
                            case utka::uval_type::u_int64: return "int64";
                            case utka::uval_type::u_float: return "float";
                            case utka::uval_type::u_bool:  return "bool";
                            case utka::uval_type::u_cstr:  return "string&";
                                //...
                                //...
                                //...
                            default:                       return "undefined";
                            }
                        };

                        BREAK_IF(prop.val_ptr == nullptr);

                        const bool write = prop.access == utka::uprop_access::read_write;
                        const char* p_name = prop.alias.c_str();
                        const char* p_accs = write ? "R" : "RW";
                        const char* p_type = type_to_name(prop.type);

                        ImItem::Text(p_name); ImGui::NextColumn();
                        ImItem::Text(p_accs); ImGui::NextColumn();
                        ImItem::Text(p_type); ImGui::NextColumn();

                        const std::string imid = str::format::insert("##{}", prop_id++);

                        //prop editor
                        ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x);
                        switch (prop.type)
                        {
                        case utka::uval_type::u_int32:
                        {
                            int& value = *(int*)prop.val_ptr;
                            int buffer = value;

                            ImGui::InputInt(imid.c_str(), &buffer); ImGui::NextColumn();
                            if (write && ImGui::IsItemDeactivatedAfterEdit())
                                value = buffer;

                            break;
                        }
                        case utka::uval_type::u_float:
                        {
                            float& value = *(float*)prop.val_ptr;
                            float buffer = value;

                            ImGui::InputFloat(imid.c_str(), &buffer); ImGui::NextColumn();
                            if (write && ImGui::IsItemDeactivatedAfterEdit())
                                value = buffer;

                            break;
                        }
                        case utka::uval_type::u_bool:
                        {
                            bool& value = *(bool*)prop.val_ptr;
                            bool buffer = value;

                            ImGui::Checkbox(imid.c_str(), &buffer); ImGui::NextColumn();
                            if (write && ImGui::IsItemDeactivatedAfterEdit())
                                value = buffer;

                            break;
                        }
                        case utka::uval_type::u_cstr:
                        {
                            std::string& value = *(std::string*)prop.val_ptr;
                            std::string buffer = value;

                            ImGui::InputText(imid.c_str(), &buffer[0], buffer.size()); ImGui::NextColumn();
                            if (write && ImGui::IsItemDeactivatedAfterEdit())
                                value = std::move(buffer);

                            break;
                        }
                        ///...
                        ///...
                        ///...
                        case utka::uval_type::u_unknown:
                        {
                            ImItem::Text("unknown");
                            break;
                        }
                        default:
                        {
                            ImItem::Text("?err?");
                            break;
                        }
                        }
                        ImGui::PopItemWidth();

                        return;
                    };
                    draw_prop(propit);
                }
                ImGui::Columns(1);
            }
            //---------------------------------------------- fns
            ImGui::Separator();
            ImItem::Text("funcs({}):", uit.funcs.size());
            if (!uit.funcs.empty())
            {
                ImGui::Dummy({ 6,6 });
                for (const auto& funcit : uit.funcs)
                {
                    ImItem::Text(" alias:  {}", funcit.alias);
                    ImItem::Text(" args:   num:{}", funcit.arg_types.size());
                    ImItem::Text(" func:   {}", funcit.func.target_type().name());
                }
            }
            ImGui::Separator();
        }
        ImGui::EndChildFrame();
        //-----------------------------------------------------------------------------------------
    }

    //-------------------------------------------------------//

    static void graphics_drawers()
    {
        enum
        {
            draw_none,
            draw_pix,
            draw_line,
            draw_circle_root,
            draw_params,

            draw_old_tv,
            draw_trg_trg,
        };

        const size_t canvasS = 500;
        const size_t canvasW = canvasS;
        const size_t canvasH = canvasS;
        const FLOAT2 canvasSize(canvasW, canvasH);

        static bitmap32 bitmap(canvasW, canvasH, { 0,0,0,255 });

        static int state = draw_none;

        ImGui::RadioButton("null", &state, draw_none);
        ImGui::RadioButton("pixels", &state, draw_pix);
        ImGui::RadioButton("circle", &state, draw_circle_root);
        ImGui::RadioButton("lines", &state, draw_line);
        ImGui::RadioButton("params", &state, draw_params);
        ImGui::RadioButton("old tv", &state, draw_old_tv);
        ImGui::RadioButton("TrgTrg", &state, draw_trg_trg);


        static GpuTexture gpu_t;
        ImItem::ShowImage(&gpu_t);
        ImGui::Separator();

        if (state != draw_none)
        {
            if (state == draw_pix)
            {
                static int id = 0;

                *bitmap.raw_data(id) = color::random_color32();

                if (++id == bitmap.pixel_count())
                    id = 0;
            }

            if (state == draw_old_tv)
            {
                auto seed = math::xorshift_64x64();
                seed += math::RandomInt64(-99999, 99999);

                ForArray(i, bitmap.pixel_count())
                {
                    *bitmap.raw_data((int)i) = color32(
                        (uint8_t)math::xorshift_64x64(seed),
                        (uint8_t)math::xorshift_64x64(seed),
                        (uint8_t)math::xorshift_64x64(seed),
                        255
                    );
                }
            }

            if (state == draw_trg_trg)
            {
                static int N = 50000;
                static FLOAT2 A = { canvasSize.x / 2, 0 };
                static FLOAT2 B = { 0, canvasSize.y };
                static FLOAT2 C = { canvasSize.x, canvasSize.y };
                static FLOAT2 P = (A + B + C) / 3;
                ImGui::DragInt("Steps", &N, 0, INT_MAX);
                ImGui::DragFloat2("A", &A.x, 1.0f, 0, canvasS);
                ImGui::DragFloat2("B", &B.x, 1.0f, 0, canvasS);
                ImGui::DragFloat2("C", &C.x, 1.0f, 0, canvasS);

                const FLOAT2 map[3] = {
                    { std::min(A.x, canvasS - 0.0001f), std::min(A.y, canvasS - 0.0001f) },
                    { std::min(B.x, canvasS - 0.0001f), std::min(B.y, canvasS - 0.0001f) },
                    { std::min(C.x, canvasS - 0.0001f), std::min(C.y, canvasS - 0.0001f) }
                };

                bitmap.fill_by_color(art::color::palette::WHITE);
                for (int i = 0; i < N; i++)
                {
                    P = (P + map[math::rand64() % 3]) / 2.0f;
                    *bitmap.raw_data(P.x, P.y) = art::color::palette::BLACK;
                }
            }

            if (state == draw_line)
            {
                static color32 COLA = { 255, 255, 0, 255 };
                static color32 COLB = { 255,   0, 0, 255 };

                static color32 ago = COLA;
                static color32 bgo = COLB;

                MakeOnce
                {
                    ago = color::random_color32();
                    bgo = color::random_color32();
                }

                if (COLA.r < ago.r) COLA.r++; else if (COLA.r > ago.r) COLA.r--;
                if (COLA.g < ago.g) COLA.g++; else if (COLA.g > ago.g) COLA.g--;
                if (COLA.b < ago.b) COLA.b++; else if (COLA.b > ago.b) COLA.b--;
                if (COLB.r < bgo.r) COLB.r++; else if (COLB.r > bgo.r) COLB.r--;
                if (COLB.g < bgo.g) COLB.g++; else if (COLB.g > bgo.g) COLB.g--;
                if (COLB.b < bgo.b) COLB.b++; else if (COLB.b > bgo.b) COLB.b--;

                struct pt
                {
                    pt(INT2 canv_size, INT2 _pos) : size(canv_size - INT2{ 1,1 }), pos(_pos)
                    {}

                    INT2 pos;
                    INT2 size;

                    void step()
                    {
                        if (pos.y == 0 && pos.x != size.x) pos.x++; else
                            if (pos.x == size.x && pos.y != size.y) pos.y++; else
                                if (pos.y == size.y && pos.x != 0) pos.x--; else
                                    if (pos.x == 0 && pos.y != 0) pos.y--;
                    }
                };

                static pt a(bitmap.size(), INT2{ 0,0 });
                static pt b(bitmap.size(), a.size);

                draw::drawline2D(bitmap,
                    FLOAT2{ a.pos.x + 0.5f, a.pos.y + 0.5f },
                    FLOAT2{ b.pos.x + 0.5f, b.pos.y + 0.5f },
                    COLA,
                    COLB);

                a.step();
                b.step();
            }

            if (state == draw_circle_root || state == draw_params)
            {
                static FLOAT3 A;
                static FLOAT3 B;
                static FLOAT3 C;
                static color4f CA;
                static color4f CB;
                static color4f CC;

                if (state == draw_circle_root)
                {
                    static const auto rand_circle_pos = [](const float angle, const FLOAT2& size)
                    {
                        return math::move_on_angle(size / 2, angle, (size.x / 2 * 0.75));
                    };

                    static const auto rand_col = []()
                    {
                        return color4f
                        {
                            (float)math::RandomStdReal(0.0f, 0.999f),
                            (float)math::RandomStdReal(0.0f, 0.999f),
                            (float)math::RandomStdReal(0.0f, 0.999f),
                            (float)0.999,
                        };
                    };

                    static float ang = 0;
                    ang += 0.99;

                    const FLOAT2 cposA = rand_circle_pos(ang + 0.0f, FLOAT2(bitmap.size().x, bitmap.size().y));
                    const FLOAT2 cposB = rand_circle_pos(ang + 120.f, FLOAT2(bitmap.size().x, bitmap.size().y));
                    const FLOAT2 cposC = rand_circle_pos(ang + 240.f, FLOAT2(bitmap.size().x, bitmap.size().y));

                    A = { cposA.x, cposA.y, 5.f };
                    B = { cposB.x, cposB.y, 5.f };
                    C = { cposC.x, cposC.y, 5.f };

                    CA = rand_col();
                    CB = rand_col();
                    CC = rand_col();
                }
                if (state == draw_params)
                {
                    ImGui::DragFloat3("pos A", &A.x, 0.999f, 0.f, 500.f);
                    ImGui::DragFloat3("pos B", &B.x, 0.999f, 0.f, 500.f);
                    ImGui::DragFloat3("pos C", &C.x, 0.999f, 0.f, 500.f);
                    ImGui::ColorEdit4("col A", &CA.x, ImGuiColorEditFlags_DisplayRGB);
                    ImGui::ColorEdit4("col B", &CB.x, ImGuiColorEditFlags_DisplayRGB);
                    ImGui::ColorEdit4("col C", &CC.x, ImGuiColorEditFlags_DisplayRGB);
                    bitmap.fill_by_color({ 0,0,0,255 });
                }

                art::draw::drawTriangle3D(bitmap,
                    nullptr,
                    A,
                    B,
                    C,
                    CA,
                    CB,
                    CC,
                    false);
            }

            gpu_t.Update(bitmap);
        }
    }
    static void graphics_bench()
    {
        struct bench
        {
            bool active = false;

            inline void step(std::string name)
            {
                if (active)
                    end();
                active = true;

                item new_item;
                items.emplace_back(new_item);
                items.back().name = name;
                items.back().timer.SetStart();
            }

            inline void end()
            {
                items.back().timer.SetEnd();
                items.back().time = items.back().timer.GetTimeMicro();
                active = false;
            }

            struct item
            {
                long time;
                SuperTimer timer;
                std::string name;
            };

            std::vector<item> items;
        };

        static bench last_bench;

        bool need_restart = false;

        need_restart |= last_bench.items.size() == 0;
        need_restart |= ImGui::Button("Restart");

        if (need_restart)
        {
            bench BNC;

            INT2    size(5000, 5000);
            color32 color{ 22,66,200,220 };
            bitmap32 objectA(size);
            bitmap32 objectB(size);

            //-----------------------------------------
            BNC.step("construct");
            bitmap32 bitmapT(size);

            //-----------------------------------------
            BNC.step("contruct(fill)");
            { bitmap32 bitmapTT(size.x, size.y, color); }

            //-----------------------------------------
            BNC.step("construct-destruct");
            { bitmap32 bitmap(size); }

            //-----------------------------------------
            BNC.step("construct-destruct(fill)");
            { bitmap32 bitmap(size.x, size.y, color); }

            //-----------------------------------------
            BNC.step("cons then fill");
            bitmap32 bitmapTTT(size);
            bitmapTTT.fill_by_color(color);

            //-----------------------------------------
            BNC.step("cons then fill another");
            bitmap32 bitmapTTTT(size);
            objectA.fill_by_color(color);

            //-----------------------------------------
            BNC.step("cons then raw_data fill");
            bitmap32 bitmapTTTTTT(size);
            ForArray(i, bitmapTTTTTT.pixel_count())
                * bitmapTTTTTT.raw_data(i) = color;

            //-----------------------------------------
            BNC.step("just fill");
            objectA.fill_by_color(color);

            //-----------------------------------------
            BNC.step("Move assign");
            bitmap32 bitmap(bitmap32(size));

            //-----------------------------------------
            BNC.end();

            last_bench = BNC;
        }

        for (int i = 0; i < last_bench.items.size(); i++)
        {
            auto item = last_bench.items[i];

            ImGui::Text(item.name.c_str());
            ImGui::SameLine(200);
            ImGui::Text(str::format::insert("| {}", item.time).c_str());
        }
    }
    static void graphics_sandbox()
    {
        static bitmap32 img(64, 64, color32{ 0,0,0,255 });
        static bitmap32 img2(64, 64, color32{ 0,0,0,255 });
        static GpuTexture gpu_img;

        gpu_img.Update(img);
        ImItem::ShowImage(&gpu_img, true, FLOAT2{ 64 * 8, 64 * 8 });

        static SuperTimer st;
        if (st.InEach(1. / 100))
        {
            const color32 block_free = color32{ 0,   0,  0, 255 };
            const color32 block_sand = color32{ 255, 255,  0, 255 };

            const auto is_type = [&](INT2 pos, const color32& type) -> bool
            {
                color32 col = *img.raw_data(pos);
                return col.r == type.r
                    && col.g == type.g
                    && col.b == type.b
                    && col.a == type.a;
            };

            const auto is_free = [&](INT2 pos) -> bool
            {
                return img.is_point_in(pos)
                    && is_type(pos, block_free);
            };

            const auto set_block = [&](INT2 pos, const color32& type)
            {
                return *img2.raw_data(pos) = type;
            };

            const auto process = [&](INT2 pos)
            {
                if (is_type(pos, block_sand))
                {
                    INT2 pos_down{ pos.x, pos.y + 1 };
                    if (is_free(pos_down))
                    {
                        set_block(pos, block_free);
                        set_block(pos_down, block_sand);
                    }
                    else
                    {
                        INT2 pos_down_l{ pos_down.x - 1, pos_down.y };
                        INT2 pos_down_r{ pos_down.x + 1, pos_down.y };

                        if (is_free(pos_down_l))
                        {
                            set_block(pos_down, block_free);
                            set_block(pos_down_l, block_sand);
                        }
                        else if (is_free(pos_down_r))
                        {
                            set_block(pos_down, block_free);
                            set_block(pos_down_r, block_sand);
                        }
                    }
                }
            };

            register INT2 pos = { 0,0 };
            register INT2 sand_spawn_pos = { 64 / 2,64 / 8 };

            if (is_free(sand_spawn_pos))
                set_block(sand_spawn_pos, block_sand);

            for (; pos.y < img.size().y; pos.y++) {
                pos.x = 0;
                for (; pos.x < img.size().x; pos.x++) {
                    process(pos);
                }
            }
            img = img2;
        }
    }

    //--------------------------------------------------- open cl


    //--------------------------------------------------- sunc

    static void sunc_hash_distribution()
    {
        static bitmap32 bitmap(64, 64, color::palette::BLACK);
        static GpuTexture gpu_img;
        ImItem::ShowImage(&gpu_img);

        //------------ input

        if (ImGui::Button("reset"))
            bitmap.fill_by_color(color::palette::BLACK);
        ImGui::Separator();

        int steps = 0;

        static bool is_active;
        ImGui::Checkbox("active (+50'000)", &is_active);

        if (is_active)
            steps = 50'000;

        //------------ process

        for (int i = 0; i < steps; i++)
        {
            const uint64_t hash = math::hash64(math::rand64());
            const uint64_t total = bitmap.pixel_count();

            const double ratio = double(hash) / std::numeric_limits<uint64_t>::max();
            const uint64_t off = ratio * total;

            bitmap.raw_data(off)->r++;
        }

        if (steps > 0)
            gpu_img.Update(bitmap);
    }

    static void sunc_mem_distribution()
    {
        static bitmap32 bitmap(512, 256, color::palette::BLACK);
        static GpuTexture gpu_img;
        ImItem::ShowImage(&gpu_img);

        //------------ input

        ImGui::Separator();

        struct chunk
        {
            void* data;
            bool selected;
            char name[32];
        };
        static std::vector<chunk> arr;

        if (ImGui::Button("Add (8 * 1MB)"))
        {
            ForArray(i, 8)
            {
                chunk ch;
                ch.data = new byte_t[1024 * 1024];
                ch.selected = true;
                std::string srt = str::format::insert("chank {}", arr.size());
                std::memcpy(ch.name, &srt[0], srt.size() + 1);
                arr.push_back(ch);
            }
        }
        ////////////////////////////////////////////////////////////////////
        bool need_redraw = false;

        static uint64_t dump_start = 0;
        static uint64_t dump_end = std::numeric_limits<size_t>::max();

        const uint64_t c_dump_start = 0;
        const uint64_t c_dump_end = std::numeric_limits<size_t>::max();

        need_redraw |= ImGui::SliderScalar("Dump begin", ImGuiDataType_U64, &dump_start, &c_dump_start, &c_dump_end);
        need_redraw |= ImGui::SliderScalar("Dump end", ImGuiDataType_U64, &dump_end, &c_dump_start, &c_dump_end);

        ForArray(i, arr.size())
            need_redraw |= ImGui::Selectable(arr[i].name, &arr[i].selected);

        //------------ process

        MakeOnce
            need_redraw = true;

        if (dump_start > dump_end)
            std::swap(dump_start, dump_end);

        if (dump_end == 0)
            need_redraw = false;

        if (need_redraw)
        {
            bitmap.fill_by_color(color::palette::BLACK);

            ForArray(i, arr.size())
            {
                if (arr[i].selected)
                {
                    uint64_t offA = reinterpret_cast<int>(arr[i].data);
                    uint64_t offB = offA + 1024 * 1024;

                    offA = std::max(uint64_t(0), offA);
                    offB = std::min(c_dump_end, offB);

                    const double ratioA = double(offA - dump_start) / (dump_end - dump_start);
                    const double ratioB = double(offB - dump_start) / (dump_end - dump_start);

                    uint64_t pt_from = bitmap.pixel_count() * ratioA;
                    uint64_t pt_to = bitmap.pixel_count() * ratioB;

                    pt_from = std::max(pt_from, uint64_t(0));
                    pt_to = std::min(pt_to, uint64_t(bitmap.pixel_count() - 1));

                    auto col = color::random_color32();
                    ForArrayFrom(pt, pt_from, pt_to)
                    {
                        *bitmap.raw_data(pt) = col;
                    }
                }
            }
            gpu_img.Update(bitmap);
        }
    }

    static void sunc_packets()
    {
        using namespace net;

        enum
        {
            maxlen_ip = 1 + 15,
            maxlen_port = 1 + 5,
            maxlen_msg = 1 + 500,
        };

        ImGui::BeginColumns("_client-server_colums", 2);
        ImGui::Text("Server");
        {
            static UdpSocket socket;
            static std::string str_port = "2000";

            ImGui::InputText("Open on", &str_port[0], maxlen_port);
            if (ImGui::Button("start")) { socket.ListenOnPort(std::stoi(str_port)); }
            if (ImGui::Button("stop")) { socket.Close(); }
        }
        ImGui::NextColumn();
        ImGui::Text("Client");
        {
            static UdpSocket socket;
            static std::string str_ip = "127.0.0.1";
            static std::string str_port = "2000";
            static std::string str_msg = "";

            ImGui::InputText("Send to", &str_ip[0], maxlen_ip);
            ImGui::InputText("Port", &str_port[0], maxlen_port);
            ImGui::InputText("Msg", &str_msg[0], maxlen_msg);

            if (ImGui::Button("Send [Enter]") || ImGui::IsWindowFocused() && Keyboard.WasPressed(KKey::KEY_RETURN))
            {
                socket.SendDatagrammTo(str_msg, str_ip, std::stoi(str_port));
            }
        }
        ImGui::EndColumns();
        //------------------------------------------

        class PingPong : public net::ISerializer
        {
        public:
            bool enabled = false;
            size_t size = 1024;

        protected:
            mutable bool ping = true;

        private:
            // return true if contains data
            bool implSerialize(Packet& sunc) const override
            {
                if (!enabled || !ping)
                    return false;

                std::vector<byte> vec(size);

                FlexBuffer fb;
                fb.write(vec.data(), size);

                sunc = std::move(Packet(std::move(fb)));

                ping = false;
                return true;
            }

            // return true sunc processed
            bool implDeserialize(Packet&& sunc) override
            {
                return ping = true;
            }

            ////////////////////////////////////////
        };


        class bitmap_sunc_impl : public net::ISerializer
        {
        public:
            bitmap_sunc_impl(bitmap32& inst)
            {
                img = &inst;
            }

        protected:
            mutable bool m_need_sunc = false;
            bitmap32* img;


        private:
            // return true if contains data
            bool implSerialize(Packet& sunc) const override
            {
                if (!m_need_sunc)
                    return false;

                m_need_sunc = false;

                //------------------------------------------

                const size_t IMG_SIZE = img->data_size();

                FlexBuffer fb;
                fb.reserve(sizeof(INT2) + IMG_SIZE);

                const INT2 size = img->size();
                fb.write((byte_t*)&size, sizeof(size));
                fb.write((byte_t*)img->raw_data(), IMG_SIZE);

                sunc = std::move(Packet(std::move(fb)));

                return true;
            }

            // return true sunc processed
            bool implDeserialize(Packet&& sunc) override
            {
                return false;
            }

            ////////////////////////////////////////
        };

        ImGui::Separator();

        static SuncManager suncManager;

        static int32_t var = 0;
        static Int32Serializer varLink(&var);
        static const UID id = UID::gen_uid("/uid/i32 var");

        static PingPong ping;
        static const UID id2 = UID::gen_uid("/global/PingPong");

        MakeOnce
        {
            suncManager.Register(id, &varLink);
            suncManager.Register(id2, &ping);
        }

        static SuncPipe data_pipe;

        // read change-lists from (server) / (all clients)
        //  + process receive approve and resend missing
        //
        suncManager.ReadSunc(data_pipe);

        //--------------------
        //main loop
        //--------------------

        {
            ImGui::SliderInt("value", &var, -500, 500);
        }

        {
            //static bitmap32 image(512, 512);
            //static bitmap_sunc_impl s_image(image);
            //static const UID id_image = UID::gen_uid("/uid-source/0003699");
            //static GpuTexture gpu_img;
            //MakeOnce{ gpu_img.Update(image); }
            //const bool data_changed = ImItem::ShowImage(&gpu_img, true, { 512,512 });
        }

        //--------------------
        //main loop end
        //--------------------

        // send change-list to (server) / (all clients)
        //  + send change-list to (server) / (all clients)
        suncManager.SendSunc(data_pipe);

        if (ImGui::Button("PingSwitch"))
        {
            ping.enabled = !ping.enabled;
        }
        if (ImGui::Button("PopBuffers"))
        {
            data_pipe.push_to_pop();
            const auto packets = data_pipe.implGetSuncCollection();

            std::string report;

            for (const auto& pack : packets)
            {
                //	report += str::format::insert(
                //		"\n"
                //		"\n    id: {}{}"
                //		"\n  size: {}"
                //		"\n  type: {}",
                //		pack.id.get_hi(), pack.id.get_lo(), //UID::uid_to_string(pack.id),
                //		pack.dat.dsize,
                //		pack.dat.dtype);
                //
                //	const byte_t* p = pack.dat.duptr.get();
                //
                //	report += "\n  data: ";
                //	// if little endian
                //	for (int off = pack.dat.dsize - 1; off >= 0; off--)
                //	{
                //		report += byte_to_hex_hi(p[off]);
                //		report += byte_to_hex_lo(p[off]);
                //	}
                //
                //	report += "\n   bin: ";
                //	// if little endian
                //	for (int off = pack.dat.dsize - 1; off >= 0; off--)
                //	{
                //		//report += str::format::insert("{:#b}'", p[off]);//byte_to_hex_hi(p[off]);
                //		report += byte_to_bits(p[off]);
                //	}
                //
                //	report += "\n  value=" + std::to_string(*reinterpret_cast<const int32_t*>(p));
                //	report += "\n--------------------";
            }
            log::info << report;
        }

        struct notag
        {
            static std::string friendly_measure_size(uint64_t bytes)
            {
                constexpr uint64_t BT = 1;
                constexpr uint64_t KB = 1024 * BT;
                constexpr uint64_t MB = 1024 * KB;
                constexpr uint64_t GB = 1024 * MB;
                constexpr uint64_t TB = 1024 * GB;

                const uint64_t rank = std::log2(bytes) / 10;
                const std::string PREC = "{0:.2f} ";

                switch (rank)
                {
                case 0:  return str::format::insert("{} B", bytes);
                case 1:  return str::format::insert(PREC + "KB", double(bytes) / KB);
                case 2:  return str::format::insert(PREC + "MB", double(bytes) / MB);
                case 3:  return str::format::insert(PREC + "GB", double(bytes) / GB);
                default: return str::format::insert(PREC + "TB", double(bytes) / TB);
                }
            }
        };

        {
            static uint64_t dsize = 1024;
            ImGui::DragScalar("bytes", ImGuiDataType_U64, &dsize, 1);
            ImItem::Text("dsize : {}", notag::friendly_measure_size(dsize));
            ping.size = dsize;
        }

        ImGui::Separator();
        ImGui::Text("statistic:");
        const auto stat = data_pipe.getStat();
        ImItem::Text("total bytes | in/out : {} / {}", notag::friendly_measure_size(stat.bytes_resived), notag::friendly_measure_size(stat.bytes_sended));
        ImItem::Text("datagrams   | in/out : {} / {}", stat.datagramms_resived, stat.datagramms_sended);
    }
}
