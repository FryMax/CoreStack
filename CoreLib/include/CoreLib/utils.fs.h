#pragma once

#include "globals.h"

#if defined(__cpp_lib_filesystem) || defined(__cpp_lib_experimental_filesystem)

	#if defined(__cpp_lib_filesystem)
		#include <filesystem>
		namespace fs = std::filesystem;
	#else
		#define _SILENCE_EXPERIMENTAL_FILESYSTEM_DEPRECATION_WARNING
		#include <experimental/filesystem>
		namespace fs = std::experimental::filesystem;
	#endif

	namespace
	{
		using path_t = fs::path;

		inline bool is_file_exists(const path_t& path) {
			return fs::exists(path) && fs::is_regular_file(path);
		}

		inline std::string path_to_string(const path_t& path) {
			return path.string();
		}

	}
#else //  Native filesystem impl
	namespace
	{
		using path_t = std::string;

		inline bool is_file_exists(const path_t& path){
			std::ifstream f(path.c_str()); return f.good();
		}

		inline std::string path_to_string(const path_t& path) {
			return path;
		}
	}
#endif

template<typename T>
struct Resourse_MB_DELL
{
public:
	enum class LoadMode
	{
		LazyLoad,		// just check existence of file and load it on first call
		Async,			// add to queueand load alternately
 		ImmediateAsync, // start load immediately, possibly in a separated thread
		Immediate,      // start load immediately, wait until load is finished

		// mb add step-by-step load

		enum_size,
	};

	enum class LoadStatus
	{
		NoTask,
		Reading,
		Writing,
	};

public:
	virtual ~Resourse_MB_DELL() {}

	bool LoadDataFromFile(const path_t& path, const LoadMode mode = LoadMode::Immediate)
	{
        //if (!is_file_exists(path))
        //    return false;
		//
        //rpath   = path.generic_string();
        //rmode   = mode;
        //
		//rloaded = res_api_fromfile();
		//
		//return rloaded;
	}

	bool	Release()			{ return res_api_release();     }
	void*	GetDataPtr()		{ return res_api_ptr();         }
	T*		GetData()			{ return res_api_data();        }
	size_t	GetUsedRam()		{ return res_api_ram_size();    }
	size_t	GetAllocatedRam()	{ return res_api_ram_alloc();   }

protected:
	std::string rpath   = "";
    LoadMode    rmode   = LoadMode::Immediate;
    bool		rloaded = false;

private:
    virtual bool   res_api_fromfile  () { return false;   }
    virtual bool   res_api_release   () { return false;   }
    virtual void*  res_api_ptr       () { return nullptr; }
    virtual T*     res_api_data      () { return nullptr; }
    virtual size_t res_api_ram_size	 () { return 0;       }
    virtual size_t res_api_ram_alloc () { return 0;       }
};

struct RFile : Resourse_MB_DELL<std::string>
{
private:
	struct line_data
	{
		size_t line_start;
		size_t line_end;
	};

public:
	size_t GetLineCount()
	{
		return line_sizes.size();
	}

	size_t GetLineSize(size_t line_id)
	{
		if (GetLineCount() < line_id && GetLineCount() == 0)
			return 0;

 		return line_sizes[line_id].line_end - line_sizes[line_id].line_start;
	}

protected:
	int rfile_api_calculate_line_count()
	{
		line_sizes.clear();

		int line_count = 0;

		size_t l_start = 0;
		size_t l_end = 0;

		ForArray(i, data.size())
		{
			if (data[i] == '\n' || data[i] == '\0')
			{
  				line_count++;
				l_end = i;
				line_sizes.push_back({ l_start, l_end });
				l_start = l_end;
			}
		}

		return line_count;
	}

	std::vector<line_data> line_sizes;

private:
	bool res_api_fromfile() override
	{
		assert(rmode == LoadMode::Immediate);

		std::ifstream file(rpath);

		if (!file)
			return false;

		//------------------------------// get file size
		file.seekg(0, std::ios::end);
		std::streamoff fsize = file.tellg();

		//------------------------------// create buffer
		data.resize((size_t)fsize);
		std::fill(data.begin(), data.end(), '\0');

		//------------------------------// Read file
		file.seekg(0);
		file.read(&data[0], fsize);

		rfile_api_calculate_line_count();

		return true;
	}

	bool         res_api_release   () override { data.clear(); data.shrink_to_fit(); return true; }
	void*        res_api_ptr       () override { return &data[0];		 }
	std::string* res_api_data      () override { return &data;			 }
	size_t       res_api_ram_size  () override { return data.size();	 }
	size_t       res_api_ram_alloc () override { return data.capacity(); }

protected:
	std::string data;

};

namespace FILE_IO_TEST
{
	constexpr char MSG_ERR_FILE_NOT_FOUND[] = "Failed to find file: "; // + {file_path}
	constexpr char MSG_ERR_FILE_OPEN_FAIL[] = "Failed to open file: "; // + {file_path}

	enum class file_write_mode
	{
		Resume,
		Rewrite,
	};

	struct file_buffer
	{
		void release()
		{
			if (data)
				delete[] data;
		}

		const char* data;
		long long size;
	};

	//----------------------------------------------------------// Text file format

	inline result<std::string> read_file(const path_t& path)
	{
		std::ifstream file(path, std::ios::in);

		if (is_file_exists(path))
			return error_result(MSG_ERR_FILE_NOT_FOUND + path_to_string(path));

		if (!file)
			return error_result(MSG_ERR_FILE_OPEN_FAIL + path_to_string(path));

		file.seekg(0, std::ios::end);
		const std::streamoff size = file.tellg();

		std::string data(size, '\0');
		file.seekg(std::ios::beg);
		file.read(&data[0], size);

		return data;
	}

	inline result<bool> write_file(const path_t& path, std::string data, file_write_mode wmode = file_write_mode::Resume)
	{
		int mode = std::ios::out;

		if (wmode == file_write_mode::Resume)  mode |= std::ios::app;
		if (wmode == file_write_mode::Rewrite) mode |= std::ios::trunc;

		std::ofstream file(path, mode);

		if (!file)
			return error_result(MSG_ERR_FILE_OPEN_FAIL + path_to_string(path));

		//------------------------------// write file
		file.write(&data[0], data.size());

		return true;
	}

	//----------------------------------------------------------// Raw file format

	inline result<file_buffer> read_file_raw(const path_t& path)
	{
		std::ifstream file(path, std::ios::in | std::ios::binary);

		if (!is_file_exists(path))
			return error_result(MSG_ERR_FILE_NOT_FOUND + path_to_string(path));

		if (!file)
			return error_result(MSG_ERR_FILE_OPEN_FAIL + path_to_string(path));

		//-------------------------------- get file size
		file.seekg(0, std::ios::end);
		std::streamoff size = file.tellg();

		//-------------------------------- create buffer
		char *data = new char[size];
 		std::fill(&data[0], &data[size], '\0');

		//-------------------------------- read file
		file.seekg(std::ios::beg);
		file.read(&data[0], size);

		return file_buffer{data, size};
	}

	inline result<bool> write_file_raw(const path_t& path, const char* data, size_t data_size, file_write_mode wmode = file_write_mode::Resume)
	{
		int mode = std::ios::out | std::ios::binary;

		if (wmode == file_write_mode::Resume)  mode |= std::ios::app;
		if (wmode == file_write_mode::Rewrite) mode |= std::ios::trunc;

		std::ofstream file(path, mode);

		if (!file)
			return error_result(MSG_ERR_FILE_OPEN_FAIL + path_to_string(path));

		file.write(data, data_size);

		return true;
	}

}

using namespace FILE_IO_TEST;