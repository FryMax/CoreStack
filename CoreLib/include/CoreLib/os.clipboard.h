#pragma once

#include "globals.h"
#include "art.bitmap.h"
#include "os.h"

LIB_NAMESPACE_START

namespace os_api
{
	namespace clipboard
	{
		enum class data_type
		{
			text,   // utf8 expected (multibyte enc)
			bitmap, // rgba expected (32bit)
		};

		namespace
		{
			constexpr char MSG_ERR_CL_OPEN_FAIL     [] = "Failed to open clipboard";
			constexpr char MSG_ERR_CL_GET_DATA_FAIL [] = "Failed to get clipboard data";
			constexpr char MSG_ERR_CL_NO_TEXT       [] = "Clipboard does not contain text";
			constexpr char MSG_ERR_CL_NO_IMAGES     [] = "Clipboard does not contain images";
		}

		//struct Clipboard_TODO
		//{
		//
		//	static result<std::string> GetClipboardText  ()                        { }
		//	static result<bitmap32>    GetClipboardImage ()                        { }
		//	static bool                SetClipboardImage (const bitmap32& img32)   { }
		//	static bool                SetClipboardText  (const std::string& utf8) { }
		//
		//protected:
		//	virtual bool api_open_cb();
		//	virtual bool api_clean_cb();
		//	virtual data_type api_get_data_type();
		//};


		// WINDOWS CF_BITMAP cant store alpha channel
		// mb use other format (PNG, ...)
		// https://stackoverflow.com/questions/15689541/win32-clipboard-and-alpha-channel-images

#if (PLATFORM_OS == OS_WINDOWS)

		static result<std::string> GetClipboardText ()
		{
			std::string out;

			if (!::IsClipboardFormatAvailable(CF_TEXT))
				return error_result(MSG_ERR_CL_NO_TEXT);

			if (!::OpenClipboard(NULL/*hwndMain*/))
				return error_result(MSG_ERR_CL_OPEN_FAIL);

			HANDLE hData = GetClipboardData(CF_TEXT);

			if (hData == NULL)
				return error_result(MSG_ERR_CL_GET_DATA_FAIL);

			char* pszText = static_cast<char*>(GlobalLock(hData));

			if (pszText != nullptr)
				out = std::string(pszText);

			GlobalUnlock(hData);
			CloseClipboard();

			return out;
		}

		static result<bitmap32> GetClipboardImage   ()
		{
			std::string err_msg;

			bool clOpened = false;

			bitmap32 out;

			for (;;)
			{
				if (!(bool)::IsClipboardFormatAvailable(CF_BITMAP))
				{
					err_msg = "Clipboard does not contain images";
					break;
				}

				clOpened = (bool)::OpenClipboard(NULL);
				if (!clOpened)
				{
					err_msg = "Failed to open clipboard";
					break;
				}

				HANDLE hclData = GetClipboardData(CF_BITMAP);
				if (hclData == NULL)
				{
					err_msg = "Failed to get clipboard data";
					break;
				}

				HBITMAP hBitImg = (HBITMAP)hclData;
				if (hBitImg)
				{
					BITMAPINFO bmpInfo;
					std::memset(&bmpInfo, 0, sizeof(BITMAPINFOHEADER));
					bmpInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
					::GetDIBits(::GetDC(NULL), hBitImg, 0, 0, NULL, &bmpInfo, DIB_RGB_COLORS);

					int w = bmpInfo.bmiHeader.biWidth;
					int h = bmpInfo.bmiHeader.biHeight;

					out = bitmap32(w, h);

					bmpInfo.bmiHeader.biBitCount = 32;
					bmpInfo.bmiHeader.biCompression = BI_RGB;
					bmpInfo.bmiHeader.biHeight = -bmpInfo.bmiHeader.biHeight;

					::GetDIBits(::GetDC(NULL), hBitImg, 0, bmpInfo.bmiHeader.biHeight, out.raw_data(), &bmpInfo, DIB_RGB_COLORS);
				}

				break;
			}

			if (clOpened)
				::CloseClipboard();

			if (!err_msg.empty())
				return error_result(err_msg);

			for (int i = 0; i < out.pixel_count(); i++)
				std::swap(out.raw_data(i)->r, out.raw_data(i)->b);

			return std::move(out);
		}

		static bool SetClipboardImage(const bitmap32& img)
		{
			bool clOpened = false;
			std::string err_msg;

			for (;;)
			{
				clOpened = (bool)::OpenClipboard(NULL);
				if (!clOpened)
				{
					err_msg = "Failed to open clipboard";
					break;
				}

				if (::EmptyClipboard() == FALSE)
				{
					err_msg = "Failed to clean clipboard";
					break;
				}

				HBITMAP hBitmap_copy = ::CreateBitmap(img.size().x, img.size().y, 1, 32, img.raw_data());
				::SetClipboardData(CF_BITMAP, hBitmap_copy);

				break;
			}

			if (clOpened)
				::CloseClipboard();

			if (!err_msg.empty())
			{
				log::error << err_msg;
				return false;
			}

			return true;
		}

		static bool SetClipboardText(const std::string& str)
		{
			bool clOpened = false;
			std::string err_msg;

			for (;;)
			{
				clOpened = (bool)::OpenClipboard(NULL);
				if (!clOpened)
				{
					err_msg = "Failed to open clipboard";
					break;
				}

				if (::EmptyClipboard() == FALSE)
				{
					err_msg = "Failed to clean clipboard";
					break;
				}

				HGLOBAL handle = ::GlobalAlloc(GMEM_MOVEABLE, (SIZE_T)str.size() + 1);
				if (handle == NULL)
				{
					err_msg = "Failed to GlobalAlloc";
					break;
				}

				//todo mb skip if size 0
				std::memcpy(::GlobalLock(handle), str.c_str(), str.size());
				::SetClipboardData(CF_TEXT, handle);
				::GlobalUnlock(handle);

				break;
			}

			if (clOpened)
				::CloseClipboard();

			if (!err_msg.empty())
			{
				log::error << err_msg;
				return false;
			}

			return true;
		}

#else

		static result<std::string> GetClipboardText  ()                       { LOG_WARN_NO_IMPL; return std::string("Failed to get clipboard data"); }
		static result<bitmap32>    GetClipboardImage ()                       { LOG_WARN_NO_IMPL; return bitmap32(32, 32, { 64, 200, 200, 200 });     }
		static bool                SetClipboardImage (const bitmap32& img)    { LOG_WARN_NO_IMPL; return false; }
		static bool                SetClipboardText  (const std::string& str) { LOG_WARN_NO_IMPL; return false; }

#endif

	}

}

LIB_NAMESPACE_END