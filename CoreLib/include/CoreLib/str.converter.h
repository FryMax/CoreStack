#pragma once

#include "globals.h"

//	Windows console encoding set command: chcp <enc>
//
//	866   - DOC кодировка
//	1251  - Windows-1251 (кириллица)
//	65001 Ц UTF-8

LIB_NAMESPACE_START

namespace str
{
	namespace convert
	{
		namespace
		{
			constexpr size_t TRUNCATE_LEN = ((size_t)-1);
		}

  		//---------------------------------------------------------------------------//	ASCI string <--> wstring (not recomended: bound to locale())

		/*

		inline const wchar_t* stdlib_char_to_wchar(const char* _cstr)
		{
			assert(_cstr);

			if (!_cstr)
				return L"";
			//===============================//
			size_t   text_len = ::strlen(_cstr) + 1;
			wchar_t* out_buffer = new wchar_t[text_len];
			size_t	 conv_count = 0;

			//mbstowcs_s(&conv_count, out_buffer, text_len, _cstr, TRUNCATE_LEN);
			std::mbstowcs(out_buffer, _cstr,);

			return out_buffer;
		}

		inline const char* stdlib_wchar_to_char(const wchar_t* _cstr)
		{
			assert(_cstr);

			if (!_cstr)
				return "";
			//===============================//
			size_t text_len = ::wcslen(_cstr) + 1;
			char* out_buffer = new char[text_len];
			size_t conv_count = 0;

			wcstombs_s(&conv_count, out_buffer, text_len, _cstr, TRUNCATE_LEN);

			return out_buffer;
		}

		*/

		static std::wstring	string_to_wstring(const std::string& str)
		{
			//std::wstring out(str.size(), '\0');
			//std::mbstowcs(&out[0], &str[0], str.size());
			//return std::move(out);
			LOG_WARN_NO_IMPL;
			return L"null";
		}

		static std::string wstring_to_string(const std::wstring& wstr)
		{
			//std::string out(wstr.size(), '\0');
			//std::wcstombs(&out[0], &wstr[0], wstr.size());
			//return std::move(out);
			LOG_WARN_NO_IMPL;
			return "null";
		}

		//---------------------------------------------------------------------------// UTF8 string <--> wstring

		static std::string UnicodeToUTF8(size_t codepoint)
		{
			if (codepoint <= 0x7f)//-------------------------------------------//  0 - 127
			{
				return { static_cast<char>(codepoint) };
			}
			else if (codepoint <= 0x7ff)//-------------------------------------//  128 - 2047
			{
				return {
					static_cast<char>(0xc0 | ((codepoint >> 6) & 0x1f)),
					static_cast<char>(0x80 | ((codepoint     ) & 0x3f)),
				};
			}
			else if (codepoint <= 0xffff)//------------------------------------//  2048 - 65635
			{
				return {
					static_cast<char>(0xe0 | ((codepoint >> 12) & 0x0f)),
					static_cast<char>(0x80 | ((codepoint >> 6 ) & 0x3f)),
					static_cast<char>(0x80 | ((codepoint      ) & 0x3f)),
				};
			}
			else//-------------------------------------------------------------//  65636 - max
			{
				return {
					static_cast<char>(0xf0 | ((codepoint >> 18) & 0x07)),
					static_cast<char>(0x80 | ((codepoint >> 12) & 0x3f)),
					static_cast<char>(0x80 | ((codepoint >> 6 ) & 0x3f)),
					static_cast<char>(0x80 | ((codepoint      ) & 0x3f)),
				};
			}
		}
		static std::wstring UTF8StringToWstring(const std::string& _text)
		{
			LOG_WARN_NO_IMPL;
			//std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
			//return converter.from_bytes(_text);
			return {};
		}
		static std::string  WstringToUTF8String(const std::wstring& _text)
		{
			LOG_WARN_NO_IMPL;
			//std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
			//return converter.to_bytes(_text);
			return {};
		}

		//====================================================================[	Encoders-converters	] (need tests)
		namespace
		{
			struct cp1251_to_unicode { int cp1251; int unicode; }
			constexpr g_letters[] = {
			   { 0x82, 0x201A }, // SINGLE LOW-9 QUOTATION MARK
			   { 0x83, 0x0453 }, // CYRILLIC SMALL LETTER GJE
			   { 0x84, 0x201E }, // DOUBLE LOW-9 QUOTATION MARK
			   { 0x85, 0x2026 }, // HORIZONTAL ELLIPSIS
			   { 0x86, 0x2020 }, // DAGGER
			   { 0x87, 0x2021 }, // DOUBLE DAGGER
			   { 0x88, 0x20AC }, // EURO SIGN
			   { 0x89, 0x2030 }, // PER MILLE SIGN
			   { 0x8A, 0x0409 }, // CYRILLIC CAPITAL LETTER LJE
			   { 0x8B, 0x2039 }, // SINGLE LEFT-POINTING ANGLE QUOTATION MARK
			   { 0x8C, 0x040A }, // CYRILLIC CAPITAL LETTER NJE
			   { 0x8D, 0x040C }, // CYRILLIC CAPITAL LETTER KJE
			   { 0x8E, 0x040B }, // CYRILLIC CAPITAL LETTER TSHE
			   { 0x8F, 0x040F }, // CYRILLIC CAPITAL LETTER DZHE
			   { 0x90, 0x0452 }, // CYRILLIC SMALL LETTER DJE
			   { 0x91, 0x2018 }, // LEFT SINGLE QUOTATION MARK
			   { 0x92, 0x2019 }, // RIGHT SINGLE QUOTATION MARK
			   { 0x93, 0x201C }, // LEFT DOUBLE QUOTATION MARK
			   { 0x94, 0x201D }, // RIGHT DOUBLE QUOTATION MARK
			   { 0x95, 0x2022 }, // BULLET
			   { 0x96, 0x2013 }, // EN DASH
			   { 0x97, 0x2014 }, // EM DASH
			   { 0x99, 0x2122 }, // TRADE MARK SIGN
			   { 0x9A, 0x0459 }, // CYRILLIC SMALL LETTER LJE
			   { 0x9B, 0x203A }, // SINGLE RIGHT-POINTING ANGLE QUOTATION MARK
			   { 0x9C, 0x045A }, // CYRILLIC SMALL LETTER NJE
			   { 0x9D, 0x045C }, // CYRILLIC SMALL LETTER KJE
			   { 0x9E, 0x045B }, // CYRILLIC SMALL LETTER TSHE
			   { 0x9F, 0x045F }, // CYRILLIC SMALL LETTER DZHE
			   { 0xA0, 0x00A0 }, // NO-BREAK SPACE
			   { 0xA1, 0x040E }, // CYRILLIC CAPITAL LETTER SHORT U
			   { 0xA2, 0x045E }, // CYRILLIC SMALL LETTER SHORT U
			   { 0xA3, 0x0408 }, // CYRILLIC CAPITAL LETTER JE
			   { 0xA4, 0x00A4 }, // CURRENCY SIGN
			   { 0xA5, 0x0490 }, // CYRILLIC CAPITAL LETTER GHE WITH UPTURN
			   { 0xA6, 0x00A6 }, // BROKEN BAR
			   { 0xA7, 0x00A7 }, // SECTION SIGN
			   { 0xA8, 0x0401 }, // CYRILLIC CAPITAL LETTER IO
			   { 0xA9, 0x00A9 }, // COPYRIGHT SIGN
			   { 0xAA, 0x0404 }, // CYRILLIC CAPITAL LETTER UKRAINIAN IE
			   { 0xAB, 0x00AB }, // LEFT-POINTING DOUBLE ANGLE QUOTATION MARK
			   { 0xAC, 0x00AC }, // NOT SIGN
			   { 0xAD, 0x00AD }, // SOFT HYPHEN
			   { 0xAE, 0x00AE }, // REGISTERED SIGN
			   { 0xAF, 0x0407 }, // CYRILLIC CAPITAL LETTER YI
			   { 0xB0, 0x00B0 }, // DEGREE SIGN
			   { 0xB1, 0x00B1 }, // PLUS-MINUS SIGN
			   { 0xB2, 0x0406 }, // CYRILLIC CAPITAL LETTER BYELORUSSIAN-UKRAINIAN I
			   { 0xB3, 0x0456 }, // CYRILLIC SMALL LETTER BYELORUSSIAN-UKRAINIAN I
			   { 0xB4, 0x0491 }, // CYRILLIC SMALL LETTER GHE WITH UPTURN
			   { 0xB5, 0x00B5 }, // MICRO SIGN
			   { 0xB6, 0x00B6 }, // PILCROW SIGN
			   { 0xB7, 0x00B7 }, // MIDDLE DOT
			   { 0xB8, 0x0451 }, // CYRILLIC SMALL LETTER IO
			   { 0xB9, 0x2116 }, // NUMERO SIGN
			   { 0xBA, 0x0454 }, // CYRILLIC SMALL LETTER UKRAINIAN IE
			   { 0xBB, 0x00BB }, // RIGHT-POINTING DOUBLE ANGLE QUOTATION MARK
			   { 0xBC, 0x0458 }, // CYRILLIC SMALL LETTER JE
			   { 0xBD, 0x0405 }, // CYRILLIC CAPITAL LETTER DZE
			   { 0xBE, 0x0455 }, // CYRILLIC SMALL LETTER DZE
			   { 0xBF, 0x0457 }  // CYRILLIC SMALL LETTER YI
			};
		}

		inline bool utf8_to_cp1251_low_level(const char *in, char *out)
		{
			int j = 0;
			for (int i = 0; in[i] != 0; ++i)
			{
				char Prefix = in[i];
				char Suffix = in[i + 1];
				if ((Prefix & 0x80) == 0)
				{
					out[j] = (char)Prefix;
					++j;
				}
				else if ((~Prefix) & 0x20)
				{
					int first5bit = Prefix & 0x1F;
					first5bit <<= 6;
					int sec6bit = Suffix & 0x3F;
					int unic_char = first5bit + sec6bit;

					if      (unic_char >= 0x080 && unic_char <= 0x0FF) { out[j] = (char)(unic_char); }
					else if (unic_char >= 0x402 && unic_char <= 0x403) { out[j] = (char)(unic_char - 0x382); }
					else if (unic_char >= 0x410 && unic_char <= 0x44F) { out[j] = (char)(unic_char - 0x350); }
					else
					{
						constexpr int count = ARRAY_SIZE(g_letters);

						ForArray(k, count)
						{
							if (unic_char == g_letters[k].unicode)
							{
								out[k] = g_letters[i].cp1251;
								goto NEXT_LETTER;
							}
						}
						// can't convert this char
						return false;
					}
				NEXT_LETTER:
					++i;
					++j;


				}
				else
				{
					// can't convert this chars
					return false;
				}
			}
			out[j] = 0;
			return true;
		}

		inline std::string cp1251_to_utf8(const std::string& in)
		{
			std::string utf8_string;

			// en-1, ru-2, ch-4 bytes per char
			utf8_string.reserve(in.size() * 2);

			for (const auto c : in)
 			{
				constexpr size_t TABLE_1251_TO_UNICODE[] = {
					0x0000, 0x0001, 0x0002, 0x0003, 0x0004, 0x0005, 0x0006, 0x0007, 0x0008, 0x0009, 0x000A, 0x000B, 0x000C, 0x000D, 0x000E, 0x000F,
					0x0010, 0x0011, 0x0012, 0x0013, 0x0014, 0x0015, 0x0016, 0x0017, 0x0018, 0x0019, 0x001A, 0x001B, 0x001C, 0x001D, 0x001E, 0x001F,
					0x0020, 0x0021, 0x0022, 0x0023, 0x0024, 0x0025, 0x0026, 0x0027, 0x0028, 0x0029, 0x002A, 0x002B, 0x002C, 0x002D, 0x002E, 0x002F,
					0x0030, 0x0031, 0x0032, 0x0033, 0x0034, 0x0035, 0x0036, 0x0037, 0x0038, 0x0039, 0x003A, 0x003B, 0x003C, 0x003D, 0x003E, 0x003F,
					0x0040, 0x0041, 0x0042, 0x0043, 0x0044, 0x0045, 0x0046, 0x0047, 0x0048, 0x0049, 0x004A, 0x004B, 0x004C, 0x004D, 0x004E, 0x004F,
					0x0050, 0x0051, 0x0052, 0x0053, 0x0054, 0x0055, 0x0056, 0x0057, 0x0058, 0x0059, 0x005A, 0x005B, 0x005C, 0x005D, 0x005E, 0x005F,
					0x0060, 0x0061, 0x0062, 0x0063, 0x0064, 0x0065, 0x0066, 0x0067, 0x0068, 0x0069, 0x006A, 0x006B, 0x006C, 0x006D, 0x006E, 0x006F,
					0x0070, 0x0071, 0x0072, 0x0073, 0x0074, 0x0075, 0x0076, 0x0077, 0x0078, 0x0079, 0x007A, 0x007B, 0x007C, 0x007D, 0x007E, 0x0020,
					0x0402, 0x0403, 0x201A, 0x0453, 0x201E, 0x2026, 0x2020, 0x2021, 0x20AC, 0x2030, 0x0409, 0x2039, 0x040A, 0x040C, 0x040B, 0x040F,
					0x0452, 0x2018, 0x2019, 0x201C, 0x201D, 0x2022, 0x2013, 0x2014, 0x0020, 0x2122, 0x0459, 0x203A, 0x045A, 0x045C, 0x045B, 0x045F,
					0x00A0, 0x040E, 0x045E, 0x0408, 0x00A4, 0x0490, 0x00A6, 0x00A7, 0x0401, 0x00A9, 0x0404, 0x00AB, 0x00AC, 0x00AD, 0x00AE, 0x0407,
					0x00B0, 0x00B1, 0x0406, 0x0456, 0x0491, 0x00B5, 0x00B6, 0x00B7, 0x0451, 0x2116, 0x0454, 0x00BB, 0x0458, 0x0405, 0x0455, 0x0457,
					0x0410, 0x0411, 0x0412, 0x0413, 0x0414, 0x0415, 0x0416, 0x0417, 0x0418, 0x0419, 0x041A, 0x041B, 0x041C, 0x041D, 0x041E, 0x041F,
					0x0420, 0x0421, 0x0422, 0x0423, 0x0424, 0x0425, 0x0426, 0x0427, 0x0428, 0x0429, 0x042A, 0x042B, 0x042C, 0x042D, 0x042E, 0x042F,
					0x0430, 0x0431, 0x0432, 0x0433, 0x0434, 0x0435, 0x0436, 0x0437, 0x0438, 0x0439, 0x043A, 0x043B, 0x043C, 0x043D, 0x043E, 0x043F,
					0x0440, 0x0441, 0x0442, 0x0443, 0x0444, 0x0445, 0x0446, 0x0447, 0x0448, 0x0449, 0x044A, 0x044B, 0x044C, 0x044D, 0x044E, 0x044F
				};

				const auto codepoint = static_cast<size_t>(TABLE_1251_TO_UNICODE[c]);
				utf8_string += UnicodeToUTF8(codepoint);
			}

			return utf8_string;
		}

		inline std::string utf8_to_cp1251	(const std::string& in)
		{
			const char *text = in.c_str();
			char *buffer = new char[in.size()];

			int result = utf8_to_cp1251_low_level(text, buffer);

			assert(result == 0);

			std::string out = std::string(buffer);

			if (buffer)
				delete[] buffer;

			return out;
		}

		inline std::string cp866_to_cp1251 (const std::string& in)
		{
 			std::string out = in;

			for (size_t i = 0; i < out.size(); i++)
			{
				unsigned char& c = *reinterpret_cast<unsigned char*>(out[i]);

				     if (c >= 128 && c < 176) c += 64;
				else if (c >= 224 && c < 240) c += 16;
			}

			return out;
		}
	}
}


LIB_NAMESPACE_END