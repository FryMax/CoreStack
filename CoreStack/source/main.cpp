#include "pch.h"

#include "MyInput.h"
#include "MyRender.h"

#include <future>
#include <shared_mutex>
#include <map>
template <class T>
inline void std_hash(int& seed, const T& v)
{
	std::hash<T> hasher;
	seed ^= hasher(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
}
class TaskMan
{

	//template <class _Fty, class... _ArgTypes>
	//_NODISCARD future<_Invoke_result_t<decay_t<_Fty>, decay_t<_ArgTypes>...>> async(
	//	launch _Policy, _Fty&& _Fnarg, _ArgTypes&&... _Args) {
	//	// manages a callable object launched with supplied policy
	//	using _Ret = _Invoke_result_t<decay_t<_Fty>, decay_t<_ArgTypes>...>;
	//	using _Ptype = typename _P_arg_type<_Ret>::type;
	//	_Promise<_Ptype> _Pr(
	//		_Get_associated_state<_Ret>(_Policy, _Fake_no_copy_callable_adapter<_Fty, _ArgTypes...>(
	//			_STD forward<_Fty>(_Fnarg), _STD forward<_ArgTypes>(_Args)...)));
	//
	//	return future<_Ret>(_Pr._Get_state_for_future(), _Nil());
	//}
	//
	//template<class fn, class... args>
	//auto addTask(fn&& func, args&&... _args)
	//{
	//	using retType = std::result_of(decltype(&C::Func)(C, char, int&))
	//
	//}
	//
	//void aaa()
	//{
	//	TaskMan tasks;
	//
	//	const auto futere_result = tasks.addTask([]()
	//		{
	//			std::this_thread::sleep_for(std::chrono::seconds(5));
	//			return 369;
	//		});
	//}
};
using Collection_t = std::string;
using PGroupsSnapshot = std::shared_ptr<Collection_t>;
namespace
{
    class GroupsProvider
    {
    public:

        struct Snapshot
        {
        public:
            friend class GroupsProvider;

        public:
            std::string get(const std::string& id) const { return *snapshot; }


        private:
            Snapshot(PGroupsSnapshot collection) : snapshot(collection)
            {
            }

            PGroupsSnapshot snapshot;
        };

        Snapshot getGroupsSnapshot()
        {
            return m_snapshot;
        }

	public:
        PGroupsSnapshot m_snapshot;
    };
}



void aaa()
{
	MakeOnceWait(
		log::info << "aaa";
	)

}

void test()
{
	return;


	GroupsProvider prov;
    prov.m_snapshot = std::make_shared<Collection_t>();
	*prov.m_snapshot = "ass\n";

	auto ta = std::async(std::launch::async, [&]()
		{
			const auto it = prov.getGroupsSnapshot();
            ForArray(i, 10)
			{
				std::cout << it.get("");
				std::this_thread::sleep_for(std::chrono::seconds(1));
			}
		});
    auto tb = std::async(std::launch::async, [&]()
        {
            const auto it = prov.getGroupsSnapshot();
            ForArray(i, 10)
            {
                std::cout << it.get("");
				std::this_thread::sleep_for(std::chrono::seconds(1));
            }
        });

    std::this_thread::sleep_for(std::chrono::seconds(5));
	std::cout << "released\n";
	prov.m_snapshot = std::make_shared<Collection_t>();
    *prov.m_snapshot = "ASS\n";

    auto ta2 = std::async(std::launch::async, [&]()
        {
            const auto it = prov.getGroupsSnapshot();
            ForArray(i, 10)
            {
                std::cout << it.get("");
                std::this_thread::sleep_for(std::chrono::seconds(1));
            }
        });
    auto tb2 = std::async(std::launch::async, [&]()
        {
            const auto it = prov.getGroupsSnapshot();
            ForArray(i, 10)
            {
                std::cout << it.get("");
                std::this_thread::sleep_for(std::chrono::seconds(1));
            }
        });

	//auto map = std::map<std::string, std::string>
	//{
	//	{"a","aaa"},
	//	{"b","bbb"},
	//	{"c","ccc"},
	//};
	//
	//
	//for (auto& it : map)
	//{
	//	auto a = std::move(it);
	//	log::info << str::format::insert("{} - {}", it.first, it.second);
	//}
	//for (auto& it : map)
	//{
	//	auto a = std::move(it);
	//	log::info << str::format::insert("{} - {}", it.first, it.second);
	//
	//}
	//SuperTimer st;
	//
	//st.SetStart();
	//ForArray(i, 1'000'000)
	//{
	//	//auto s = snappy::Compress(&str[0], str.size() , &output);
	//	Compress((byte_t*)&str[0], str.size(), &compressed);
	//}
	//st.SetEnd();
	//
	//std::string restored;
	////auto out = snappy::Uncompress(&output[0], output.size() , &restored);
	//Uncompress((byte_t*)&compressed[0], &restored);
	//BREAK_IF(str != restored);

}

//int main(int argc, char **argv)
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow)
{
	std::setlocale(LC_ALL, "C");

	os_api::Win32DBG::OnUeCreateMiniDump();

	os::Console console;
	os::Window window;

	MyInputReader input;
	MyRender render;

	//---------------------------------------------// Console
	console.Init(ConsoleState());
	console.Show();
	console.RedirectIO();
	console.SetUTF8Encoding();

	log::update_output_links(); // u can use log now

	//log::info << GetBuildInfo();

	for (const auto& it : GetBuildInfo())
		log::info << str::format::insert("{:<10} : {}", it.first, it.second);
	//---------------------------------------------//

	test();

	//---------------------------------------------// Window
	window.Init(WindowState());
	window.Show();
	window.AttachInputHandler(&input);
	//---------------------------------------------// Render
	auto RO = graphics::RenderOptions();

	RO.MyWindow    = (os::Window*)&window;
	//RO.MyConsole = (os::Console*)&console;

	RO.WindowId       = window.GetWindowId();
	RO.RenderCanvasH  = window.GetHeight();
	RO.RenderCanvasW  = window.GetWidth();

	RO.AAMode             = graphics::AntiAliasingMethod::NoAA;
	RO.MSAA_SamplerCount  = 1;
	RO.MSAA_QualityLevel  = 0;
	RO.AnisotropicQuality = 8;

	//RO.single_thread_gpu_api_use_promise = true;
	//RO.disable_gpu_task_timeout = false;

	//------------------------------// init
	log::info << "Init render...";
	render.Init(RO);
	//------------------------------// dummy frame test
	log::info << "First frame...";
	render.Frame();

	while (window.IsExit() == false)
	{
		window.HandleMessages();

		if (window.WasResized())
			render.NotifyResize(window.GetWidth(), window.GetHeight());

		render.NotifyFocus(window.IsActive());
		render.Frame();
	}

	render.Release();
	window.Close();

	std::this_thread::sleep_for(std::chrono::seconds(3));
	return 0;
}
