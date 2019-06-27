#include "in.h"
#include "MyAppRender.h"
#include "MySdlWindow.h"

#ifndef FAKE_EM_BUILD
	#err " -- FAKE_EM_BUILD macro was expected"
#endif

//del me
//static std::string Win32errorToString(DWORD err)
//{
//	DWORD errorMessageID = err;
//	if (errorMessageID == 0)
//		return std::string(); //No error message has been recorded
//
//	LPSTR messageBuffer = nullptr;
//	size_t size = FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
//		NULL, errorMessageID, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)& messageBuffer, 0, NULL);
//
//	std::string message(messageBuffer, size);
//
//	ForArray(i, message.size())
//	{
//		if (message[i] == '\n' || message[i] == '\r')
//			message.erase(i);
//	}
//
//	//Free the buffer.
//	LocalFree(messageBuffer);
//
//	return message;
//}

static void PopGlErrors(bool print = true)
{
	GLenum gl_error = ::glGetError();

	while (gl_error != GL_NO_ERROR)
	{
		if (print)
		{
			switch (gl_error)
			{
			break; case GL_INVALID_ENUM:                  log::error << "GL_INVALID_ENUM: An unacceptable value is specified for an enumerated argument.The offending command is ignored and has no other side effect than to set the error flag.";
			break; case GL_INVALID_VALUE:                 log::error << "GL_INVALID_VALUE: A numeric argument is out of range.The offending command is ignored and has no other side effect than to set the error flag.";
			break; case GL_INVALID_OPERATION:             log::error << "GL_INVALID_OPERATION: The specified operation is not allowed in the current state.The offending command is ignored and has no other side effect than to set the error flag.";
			break; case GL_INVALID_FRAMEBUFFER_OPERATION: log::error << "GL_INVALID_FRAMEBUFFER_OPERATION: The framebuffer object is not complete.The offending command is ignored and has no other side effect than to set the error flag.";
			break; case GL_OUT_OF_MEMORY:                 log::error << "GL_OUT_OF_MEMORY: There is not enough memory left to execute the command.The state of the GL is undefined, except for the state of the error flags, after this error is recorded.";
			break; case GL_STACK_UNDERFLOW:               log::error << "GL_STACK_UNDERFLOW: An attempt has been made to perform an operation that would cause an internal stack to underflow.";
			break; case GL_STACK_OVERFLOW:                log::error << "GL_STACK_OVERFLOW: An attempt has been made to perform an operation that would cause an internal stack to overflow.";
			break; default:                               log::error << "GL_ERROR: unexpected error code";
			}
		}
		gl_error = ::glGetError();
	}
}



extern "C"
{
	void emHelloJavaScript()
	{
		log::info << "Hello, C++!";
	}

	void emSetArticleWidth(int w, int h)
	{
		//printf("Setting SDL window size: %d x %d\n", windowSizeX, windowSizeY);
		SDL_SetWindowSize(GetSDLGLContext().windowId, w, h);
	}
}




#if (EM_BUILD)

	static os::Window*  em_window;
	static MyAppRender* em_render;

	void em_loop()
	{
		em_window->HandleMessages();

		if (em_window->WasResized())
			em_render->NotifyResize(em_window->GetWidth(), em_window->GetHeight());

		em_render->NotifyFocus(em_window->IsActive());
		em_render->PaintTemplate();
	}

	int main(int, char**)
#else
	int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow)
	//int __stdcall wWinMain(void* hInstance, void* hPrevInstance, wchar_t* lpCmdLine, int nCmdShow)
#endif
{
	//os_api::Win32DBG::OnUExepCreateMiniDump();

	os::Console console;

	os::Window*    window = new MyWindowSDL;
	MyAppRender*   render = new MyAppRender;
	MyInputReader* input  = new MyInputReader;

	//---------------------------------------------// console init
	console.Init(ConsoleState());
	console.Show();
	console.RedirectIO();
	console.SetUTF8Encoding();

	log::update_output_links();

	for (const auto& it : GetBuildInfo())
		log::info << str::format::insert("{:<20} : {}", it.first, it.second);
	//---------------------------------------------// window init
	WindowState WS = WindowState();
	WS.Wight  = 1600;
	WS.Height = 900;

	window->Init(WS);
	window->Show();
	window->AttachInputHandler(input);
	//---------------------------------------------// render init
	auto RO = graphics::RenderOptions();

	RO.MyWindow      = (os::Window*)window;
	RO.WindowId      = window->GetWindowId();
	RO.RenderCanvasH = window->GetHeight();
	RO.RenderCanvasW = window->GetWidth();

	RO.AAMode             = graphics::AntiAliasingMethod::NoAA;
	RO.MSAA_SamplerCount  = 1;
	RO.MSAA_QualityLevel  = 0;
	RO.AnisotropicQuality = 8;

	//--------------------------------  init
	log::info << "Init render...";
	render->Init(RO);

	//--------------------------------  dummy frame test
	log::info << "First frame...";
	render->PaintTemplate();

#if(EM_BUILD)
		em_window = window;
		em_render = render;
		emscripten_set_main_loop(em_loop, 0, 0);
#else
		while (window->IsExit() == false)
		{
			window->HandleMessages();

			if (window->WasResized())
				render->NotifyResize(window->GetWidth(), window->GetHeight());

			render->NotifyFocus(window->IsActive());
			render->PaintTemplate();

			{
				::PopGlErrors();
				int  mGLerrCount = 0;
				int  mWin32errCount = 0;
				bool mGetGLErrors = true;
				bool mGetWin32Errors = true;

				if (mGetGLErrors)
				{

				}

				// 			if (mGetWin32Errors)
				// 			{
				// 				DWORD win32err = ::GetLastError();
				//
				// 				while (win32err != 0)
				// 				{
				// 					mWin32errCount++;
				// 					std::string err = Win32errorToString(win32err);
				// 					if (err != "")
				// 						log::error << "win32 error: " + str::convert::cp1251_to_utf8(err);
				// 					win32err = ::GetLastError();
				// 					::SetLastError(0);
				// 				}
				// 			}
			}
		}

		render->Release();
		window->Close();
#endif

	log::info << "main - exit";
	return 0;
}
