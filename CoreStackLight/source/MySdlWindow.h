#pragma once

#include "in.h"
#include "MyInput.h"

namespace
{
	//////////////////////////////////////////////////////////////////////////
	// SDL
	//////////////////////////////////////////////////////////////////////////
}

class MyWindowSDL : public os::Window
{
public:
	bool api_create_window()
	{
		if (SDL_Init(SDL_INIT_VIDEO) < 0)
		{
			printf("SDL init error: %s\n", SDL_GetError());
			assert(false);
		}

		SDL_WindowFlags window_flags = (SDL_WindowFlags)(SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);

		GetSDLGLContext().windowId = SDL_CreateWindow(
			mState.Title.c_str(),
			mState.X,//SDL_WINDOWPOS_CENTERED,
			mState.Y,//SDL_WINDOWPOS_CENTERED,
			mState.Wight,
			mState.Height,
			window_flags);

		SDL_SetWindowSize(GetSDLGLContext().windowId, mState.Wight, mState.Height);

		return (nullptr != GetSDLGLContext().windowId);
	}

	void api_destroy_window()
	{
		SDL_DestroyWindow(GetSDLGLContext().windowId);
		SDL_Quit();
	}

	void api_handle_messages()
	{
		SDL_Event event;
		auto my_window_id = SDL_GetWindowID(GetSDLGLContext().windowId);

		while (SDL_PollEvent(&event))
		{
			//sent to input handler
			{
				static_assert(sizeof(SDL_Event) <= sizeof(os_window_event_msg), "type size mismatch");
				os_window_event_msg buffer;
				std::memcpy(&buffer, &event, sizeof(event));
				bool replicate = mInput.EventMSG(buffer);
				//sdl does not allow to not replicate messages(mb not)
			}

			switch (event.type)
			{
			case SDL_QUIT:
				isExitSms = true;
				break;
			//------------------------------ window messages
			case SDL_WINDOWEVENT:
				switch (event.window.event)
				{
				case SDL_WINDOWEVENT_CLOSE:
					if (my_window_id == event.window.windowID)
						isExitSms = true;
					break;

				case SDL_WINDOWEVENT_SHOWN:
					mState.Visable = true;
					break;

				case SDL_WINDOWEVENT_HIDDEN:
					mState.Visable = false;
					break;

				case SDL_WINDOWEVENT_MOVED:
					mState.X = event.window.data1;
					mState.Y = event.window.data1;
					updateInputRect();
					break;

				case SDL_WINDOWEVENT_RESIZED:
				case SDL_WINDOWEVENT_SIZE_CHANGED:
					isResized     = true;
					mState.Wight  = event.window.data1;
					mState.Height = event.window.data2;
					updateInputRect();
					break;

				case SDL_WINDOWEVENT_MINIMIZED: isMinimized = true;  isMaximized = false; isActive = false;
					break;
				case SDL_WINDOWEVENT_MAXIMIZED: isMinimized = false; isMaximized = true;  isActive = true;
					break;
				case SDL_WINDOWEVENT_RESTORED:  isMinimized = false; isMaximized = false; isActive = true;
					break;

				case SDL_WINDOWEVENT_ENTER:        // Mouse entered window			  (event.window.windowID)
				case SDL_WINDOWEVENT_LEAVE:        // Mouse left window				  (event.window.windowID)
				case SDL_WINDOWEVENT_FOCUS_GAINED: // Window gained keyboard focus	  (event.window.windowID)
				case SDL_WINDOWEVENT_FOCUS_LOST:   // Window lost keyboard focus	  (event.window.windowID)
				//case SDL_WINDOWEVENT_TAKE_FOCUS:   // Window is offered a focus		  (event.window.windowID)
				case SDL_WINDOWEVENT_EXPOSED:      // Window need redraw			  (event.window.windowID)
				//case SDL_WINDOWEVENT_HIT_TEST:     // Window has a special hit test	  (event.window.windowID)
				default:
					break;
				}
				break;

			//------------------------------ input messages
			// keyboard
			case SDL_KEYDOWN:
			case SDL_KEYUP:
			// mice
			case SDL_MOUSEMOTION:
			case SDL_MOUSEBUTTONDOWN:
			case SDL_MOUSEBUTTONUP:
			case SDL_MOUSEWHEEL:
				//processed in exInputHanler
				break;

			//  drag-drop
			case SDL_DROPFILE:
			//case SDL_DROPTEXT:
			//case SDL_DROPBEGIN:
			//case SDL_DROPCOMPLETE:
				log::info << "Drag&Drop action event catched";
				break;

			// not handled events
			case SDL_FINGERMOTION:
			case SDL_FINGERDOWN:
			case SDL_FINGERUP:
			case SDL_TEXTEDITING:
			case SDL_TEXTINPUT:
			default:
				break;
			}


		}

	}


	//check sdl api
	bool api_show_hide   (bool isVisable)        { LOG_WARN_NO_IMPL; return false; }
	bool api_topmost     (bool isTopmost)        { LOG_WARN_NO_IMPL; return false; }
	bool api_window_stile(WindowBorderStile wbs) { LOG_WARN_NO_IMPL; return false; }
};