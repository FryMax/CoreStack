#pragma once

#include "../shared_in.h"

#include "addon_gl.h"

#include <Awesomium/WebCore.h>
#include <Awesomium/BitmapSurface.h>
#include <Awesomium/STLHelpers.h>


namespace
{
	const size_t		DEFAULT_WEB_CANVAS_W = 800;
	const size_t		DEFAULT_WEB_CANVAS_H = 600;
	const std::string	DEFAULT_WEB_URL_LINK = "https://www.google.ru";

	using namespace Awesomium;
};


namespace web
{
	class MyLoadListener : public Awesomium::WebViewListener::Load
	{
		virtual void OnChangeTitle		(Awesomium::WebView* caller, const Awesomium::WebString& title) = 0;
		virtual void OnChangeAddressBar	(Awesomium::WebView* caller, const Awesomium::WebURL& url) = 0;
		virtual void OnChangeTooltip	(Awesomium::WebView* caller, const Awesomium::WebString& tooltip) = 0;
		virtual void OnChangeTargetURL	(Awesomium::WebView* caller, const Awesomium::WebURL& url) = 0;
		virtual void OnChangeCursor		(Awesomium::WebView* caller, Awesomium::Cursor cursor) = 0;
		virtual void OnChangeFocus		(Awesomium::WebView* caller, Awesomium::FocusedElementType focused_type) = 0;
		virtual void OnAddConsoleMessage(Awesomium::WebView* caller, const Awesomium::WebString& message, int line_number, const Awesomium::WebString& source) = 0;


		virtual void OnShowCreatedWebView(Awesomium::WebView* caller,
			Awesomium::WebView* new_view,
			const Awesomium::WebURL& opener_url,
			const Awesomium::WebURL& target_url,
			const Awesomium::Rect& initial_pos,
			bool is_popup) = 0;

	};

	//////////////////////////////////////////////////////////////////////////
	class ImplSurface : public Awesomium::Surface
	{
	private:
		Awesomium::WebView* mWebView;

		//bitmap32 mCanvasImg;
		GpuTexture mGpuTex;

		int mCanvasWidth = 0;
		int mCanvasHeight = 0;
		bool mUpdated = false;

		mutable bitmap32 buffer;

	public:
		virtual ~ImplSurface() { }

		ImplSurface(Awesomium::WebView* view, int width, int height)
		{
			mWebView = view;
			mCanvasWidth = width;
			mCanvasHeight = height;
			mUpdated = true;

			buffer = bitmap32(width, height, { 0,0,0,255 });

			v_texture::texture_state state;

			state.bgra_mode = true;
			state.w = width;
			state.h = height;

			mGpuTex.CreateUpdateData(buffer, &state);
		}

		virtual void Paint(unsigned char *srcBuffer, int srcRowSpan, const Awesomium::Rect &srcRect, const Awesomium::Rect &destRect)
		{
			mUpdated = true;

			char Buffer[256];

			sprintf(Buffer, "[Web] Paint copy [%i %i %i %i] to [%i %i %i %i]\n",
				srcRect.x, srcRect.y, srcRect.width, srcRect.height,
				destRect.x, destRect.y, destRect.width, destRect.height);

			std::cout << std::string(Buffer);

			const QUAD box = QUAD(
				{
					destRect.x,
					destRect.y
				},
				{
					destRect.width,
					destRect.height
				});

			mGpuTex.UpdateSubRegionData(box, srcBuffer);

			const int line_size  = srcRect.width * sizeof(color32);
			const int line_count = srcRect.height;

			for (int y = 0; y < line_count; y++)
			{
				int line  = y + srcRect.y;
				int start = srcRect.x;

				const unsigned char* pScr = srcBuffer;

				int scrOff = line * srcRowSpan + start * sizeof(color32);

				std::memcpy(
					buffer.raw_data(start, line),
					pScr + scrOff,
					line_size);

			}
		}

		virtual void Scroll(int dx, int dy, const Awesomium::Rect &clip_rect)
		{
			mUpdated = true;

			char Buffer[256];

			sprintf(Buffer, "[Web] Scroll [%i %i] - [%i %i %i %i]\n",
				dx,
				dy,
				clip_rect.x,
				clip_rect.y,
				clip_rect.width,
				clip_rect.height);

			std::cout << std::string(Buffer);

			QUAD scr = QUAD(
				{ clip_rect.x, clip_rect.y, },
				{ clip_rect.width, clip_rect.height, }
			);

			const int line_size  = scr.GetSize().x * sizeof(color32);
			const int line_count = scr.GetSize().y;


			for (int y = clip_rect.y; y < clip_rect.y + clip_rect.height; y++)
			{
				std::memmove(
					buffer.raw_data(clip_rect.x + dx, y + dy),
					buffer.raw_data(clip_rect.x, y),
					clip_rect.width * sizeof(color32));
 			}

		}

		bitmap32* getBuffer()
		{
			return &buffer;
		}

		const GpuTexture* getGpuObject(bool& hasChanges)
		{
			hasChanges = mUpdated;

			mUpdated = false;

			return &mGpuTex;
		}

	};

	class ImplSurfaceFactory : public Awesomium::SurfaceFactory
	{
	public:
		virtual Awesomium::Surface* CreateSurface(Awesomium::WebView * view, int width, int height)
		{
			return new ImplSurface(view, width, height);
		}

		virtual void DestroySurface(Awesomium::Surface * surface)
		{
			delete surface;
		}
	};

	class WebProvider
	{
		ImplSurfaceFactory *mSurFac = nullptr;

	public:

		inline bool Init(int w = DEFAULT_WEB_CANVAS_W, int h = DEFAULT_WEB_CANVAS_H)
		{
			try
			{
				WebConfig config;
				WebSession *session;
				//WebPreferences ppref;

				config.log_level = kLogLevel_Verbose;

				mWebCore = WebCore::Initialize(config);

				if (!mSurFac)
					mSurFac = new ImplSurfaceFactory;

				mWebCore->set_surface_factory(mSurFac);

				session = mWebCore->CreateWebSession(WSLit("assets//cache//"), WebPreferences());
				mWebView = mWebCore->CreateWebView(w, h, session, Awesomium::kWebViewType_Offscreen);

 				ResizeCanvas(w, h);
			}
			catch (const std::exception& e)
			{
				std::cout << "[WEB] exception:" << e.what() << "\n";
			}

			return true;
		}

		inline bool ResizeCanvas(int w, int h)
		{
			if (w <= 0) return false;
			if (h <= 0) return false;

			mCanvasWight  = w;
			mCanvasHeight = h;

			mWebView->Resize(w, h);
			mWebCore->Update();

			return true;
		}

		inline bool	LoadPage(const std::string& _site = DEFAULT_WEB_URL_LINK, bool wait_for_load = false)
		{
			mLastSetPage = _site;
			WebURL url(WSLit(_site.c_str()));

			if (!url.IsValid())
				return false;

			mWebView->LoadURL(url);
			mWebCore->Update();

			if (wait_for_load)
			{
				while (mWebView->IsLoading())
				{
					mWebCore->Update();
					std::this_thread::sleep_for(std::chrono::milliseconds(20));
				}
			}

			return true;
		}

		inline void Update()		{ mWebCore->Update(); }
		inline void	ResumeRender()	{ mWebView->ResumeRendering(); }
		inline void	PauseRender()	{ mWebView->PauseRendering(); }

		// - - - - - - - - - - - - Get

		inline std::string GetVersion() { return std::string(mWebCore->version_string()); }
		inline std::string GetTitle()
		{
			const int Len = mWebView->title().ToUTF8(NULL, 0);

			std::string output(Len, 0);

			mWebView->title().ToUTF8(&output[0], Len);

			return output;
		}
		inline std::string GetUrl()
		{
			const int Len = mWebView->url().spec().ToUTF8(NULL, 0);

			std::string output(Len, 0);

			mWebView->url().spec().ToUTF8(&output[0], Len);

			return output;
		}
		inline std::string GetHtml()
		{
			JSValue ls = mWebView->ExecuteJavascriptWithResult(WSLit("document.getElementsByTagName('html')[0].innerHTML"), WSLit(""));

			if (ls.IsString())
			{
				const int Len = ls.ToString().ToUTF8(NULL, 0);

				std::string output(Len, 0);

				ls.ToString().ToUTF8(&output[0], Len);

				return output;
			}

			return "Javascript execute failed";
		}

		inline size_t GetMemUsed  () { return mWebCore->used_memory(); }
		inline size_t GetMemAlloc () { return mWebCore->allocated_memory(); }
		inline size_t GetCanvasW  () { return mCanvasWight; }
		inline size_t GetCanvasH  () { return mCanvasHeight; }
		inline bool   IsLoading   () { return mWebView->IsLoading(); }
		inline bool   IsCrashed   () { return mWebView->IsCrashed(); }

		const GpuTexture* getGpuObject(bool& HasChanges, bitmap32** buffer)
		{
			if (!mWebView)
				return nullptr;

			ImplSurface* surfase = reinterpret_cast<ImplSurface*>(mWebView->surface());

			if (!surfase)
				return nullptr;

			*buffer = surfase->getBuffer();
			return surfase->getGpuObject(HasChanges);
		}

		inline WebView* View() { return mWebView; }

	private:

		std::string mLastSetPage = "";
		size_t	mCanvasWight = 0;
		size_t	mCanvasHeight = 0;

		WebCore* mWebCore = nullptr;
		WebView* mWebView = nullptr;

	};




};