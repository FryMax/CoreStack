#include "pch.h"
#include "imview.h"

#define CL_USE_DEPRECATED_OPENCL_2_0_APIS
#ifdef __APPLE__
#include <OpenCL/opencl.h>
#else
#include <CL/cl.h>
#endif

const char* getErrorString(cl_int error)
{
    switch (error)
    {
        // run-time and JIT compiler errors
        case 0: return "CL_SUCCESS";
        case -1: return "CL_DEVICE_NOT_FOUND";
        case -2: return "CL_DEVICE_NOT_AVAILABLE";
        case -3: return "CL_COMPILER_NOT_AVAILABLE";
        case -4: return "CL_MEM_OBJECT_ALLOCATION_FAILURE";
        case -5: return "CL_OUT_OF_RESOURCES";
        case -6: return "CL_OUT_OF_HOST_MEMORY";
        case -7: return "CL_PROFILING_INFO_NOT_AVAILABLE";
        case -8: return "CL_MEM_COPY_OVERLAP";
        case -9: return "CL_IMAGE_FORMAT_MISMATCH";
        case -10: return "CL_IMAGE_FORMAT_NOT_SUPPORTED";
        case -11: return "CL_BUILD_PROGRAM_FAILURE";
        case -12: return "CL_MAP_FAILURE";
        case -13: return "CL_MISALIGNED_SUB_BUFFER_OFFSET";
        case -14: return "CL_EXEC_STATUS_ERROR_FOR_EVENTS_IN_WAIT_LIST";
        case -15: return "CL_COMPILE_PROGRAM_FAILURE";
        case -16: return "CL_LINKER_NOT_AVAILABLE";
        case -17: return "CL_LINK_PROGRAM_FAILURE";
        case -18: return "CL_DEVICE_PARTITION_FAILED";
        case -19: return "CL_KERNEL_ARG_INFO_NOT_AVAILABLE";

        // compile-time errors
        case -30: return "CL_INVALID_VALUE";
        case -31: return "CL_INVALID_DEVICE_TYPE";
        case -32: return "CL_INVALID_PLATFORM";
        case -33: return "CL_INVALID_DEVICE";
        case -34: return "CL_INVALID_CONTEXT";
        case -35: return "CL_INVALID_QUEUE_PROPERTIES";
        case -36: return "CL_INVALID_COMMAND_QUEUE";
        case -37: return "CL_INVALID_HOST_PTR";
        case -38: return "CL_INVALID_MEM_OBJECT";
        case -39: return "CL_INVALID_IMAGE_FORMAT_DESCRIPTOR";
        case -40: return "CL_INVALID_IMAGE_SIZE";
        case -41: return "CL_INVALID_SAMPLER";
        case -42: return "CL_INVALID_BINARY";
        case -43: return "CL_INVALID_BUILD_OPTIONS";
        case -44: return "CL_INVALID_PROGRAM";
        case -45: return "CL_INVALID_PROGRAM_EXECUTABLE";
        case -46: return "CL_INVALID_KERNEL_NAME";
        case -47: return "CL_INVALID_KERNEL_DEFINITION";
        case -48: return "CL_INVALID_KERNEL";
        case -49: return "CL_INVALID_ARG_INDEX";
        case -50: return "CL_INVALID_ARG_VALUE";
        case -51: return "CL_INVALID_ARG_SIZE";
        case -52: return "CL_INVALID_KERNEL_ARGS";
        case -53: return "CL_INVALID_WORK_DIMENSION";
        case -54: return "CL_INVALID_WORK_GROUP_SIZE";
        case -55: return "CL_INVALID_WORK_ITEM_SIZE";
        case -56: return "CL_INVALID_GLOBAL_OFFSET";
        case -57: return "CL_INVALID_EVENT_WAIT_LIST";
        case -58: return "CL_INVALID_EVENT";
        case -59: return "CL_INVALID_OPERATION";
        case -60: return "CL_INVALID_GL_OBJECT";
        case -61: return "CL_INVALID_BUFFER_SIZE";
        case -62: return "CL_INVALID_MIP_LEVEL";
        case -63: return "CL_INVALID_GLOBAL_WORK_SIZE";
        case -64: return "CL_INVALID_PROPERTY";
        case -65: return "CL_INVALID_IMAGE_DESCRIPTOR";
        case -66: return "CL_INVALID_COMPILER_OPTIONS";
        case -67: return "CL_INVALID_LINKER_OPTIONS";
        case -68: return "CL_INVALID_DEVICE_PARTITION_COUNT";

        // extension errors
        case -1000: return "CL_INVALID_GL_SHAREGROUP_REFERENCE_KHR";
        case -1001: return "CL_PLATFORM_NOT_FOUND_KHR";
        case -1002: return "CL_INVALID_D3D10_DEVICE_KHR";
        case -1003: return "CL_INVALID_D3D10_RESOURCE_KHR";
        case -1004: return "CL_D3D10_RESOURCE_ALREADY_ACQUIRED_KHR";
        case -1005: return "CL_D3D10_RESOURCE_NOT_ACQUIRED_KHR";
        default: return "Unknown OpenCL error";
    }
}

struct cl_ok
{
private:
    cl_int val;
public:

    operator bool() const
    {
        return val == CL_SUCCESS;
    }

    bool operator=(cl_int err)
    {
        val = err;
        if (val != CL_SUCCESS)
        {
            log::error << std::string("CL_N_OK: ") + getErrorString(val);
            //__debugbreak();
            //*(int*)nullptr = 0;
            if (val == CL_BUILD_PROGRAM_FAILURE)
            {
                // // Determine the size of the log
                // size_t log_size;
                // clGetProgramBuildInfo(program, device_id, CL_PROGRAM_BUILD_LOG, 0, NULL, &log_size);
                // // Allocate memory for the log
                // char* log = (char*)malloc(log_size);
                // // Get the log
                // clGetProgramBuildInfo(program, device_id, CL_PROGRAM_BUILD_LOG, log_size, log, NULL);
                // // Print the log
                // printf("%s\n", log);
            }
        }

        return val == CL_SUCCESS;
    }
};

struct cl_impl
{
	cl_device_id
		device_id;

	cl_context
		context;

	cl_command_queue
		command_queue;

	cl_kernel
		kernel;

	cl_mem
		memobj;

	size_t memItems;
	size_t memLenth;

	//////////////////////////////////////////////////////////////////////////
#define __CLTEST_E(test, msg) if(!ok || test) { log::error << __FUNCTION__ ": [OPENCL]:" msg; return false; }
#define CL_TEST(msg)	      if(!ok)         { log::error << __FUNCTION__ ": [OPENCL]:" msg; return false; }

	bool init_device()
	{
		cl_ok ok;
		cl_int err_code;

		cl_platform_id platform_id;
		cl_uint ret_num_platforms;
		ok = clGetPlatformIDs(1, &platform_id, &ret_num_platforms);
		CL_TEST("Cant get platform ids")

        // получить доступные устройства
        cl_uint ret_num_devices;
		ok = clGetDeviceIDs(platform_id, CL_DEVICE_TYPE_GPU/*CL_DEVICE_TYPE_DEFAULT*/, 1, &device_id, &ret_num_devices);
		CL_TEST("Cant find gpu with cl support")

        if (ret_num_devices == 0)
            return false;

		// создать контекст
		context = clCreateContext(NULL, 1, &device_id, NULL, NULL, &err_code);
		ok = err_code;
		CL_TEST("Create context failed")

        // создаем очередь команд
        command_queue = clCreateCommandQueue(context, device_id, 0, &err_code);
		ok = err_code;
		CL_TEST("Create command queue failed")

        return ok;
	}

	bool init_code()
	{
		//////////////////////////////////////////////////////////////////////////
		const std::string kernel_code =
            "float cross_product(float2 a, float2 b, float2 c)									\n"
            "{																					\n"
            "	return (a.x - c.x) * (b.y - c.y) - (b.x - c.x) * (a.y - c.y);					\n"
            "}																					\n"
            "																					\n"
            "bool is_point_in_triangle2D(float2 p, float2 a, float2 b, float2 c)				\n"
            "{																					\n"
            "	const float fcp_ab = cross_product(p, a, b);									\n"
            "	const float fcp_bc = cross_product(p, b, c);									\n"
            "	const float fcp_ca = cross_product(p, c, a);									\n"
            "																					\n"
            "	const bool cp_ab = fcp_ab < 0.0f;												\n"
            "	const bool cp_bc = fcp_bc < 0.0f;												\n"
            "	const bool cp_ca = fcp_ca < 0.0f;												\n"
            "																					\n"
            "	return ((cp_ab == cp_bc) && (cp_bc == cp_ca));									\n"
            "}																					\n"
            "																					\n"
            "__kernel void cl_main(__global uchar4* rgba)										\n"
            "{																					\n"
            "	const float2 A = { 0,   512 };													\n"
            "	const float2 B = { 256,   0 };													\n"
            "	const float2 C = { 512, 512 };													\n"
            "																					\n"
            "	const uint  GID = get_global_id(0);												\n"
            "	const uint2 SIZE = { 512,512 };													\n"
            "																					\n"
            "	const float x = GID % SIZE.y;													\n"
            "	const float y = GID / SIZE.x;													\n"
            "																					\n"
            "	const float color = is_point_in_triangle2D((float2)(x, y), A, B, C) ? 255 : 0;	\n"
            "																					\n"
            "	uint id = GID;																	\n"
            "	id = id ^ (id << 13);															\n"
            "	id = id ^ (id >> 17);															\n"
            "	id = id ^ (id << 15);															\n"
            "																					\n"
            "																					\n"
            "	rgba[GID][0] = id;																\n"
            "	rgba[GID][1] = color;															\n"
            "	rgba[GID][2] = color;															\n"
            "	rgba[GID][3] = 255;																\n"
            "}																					\n";
		//////////////////////////////////////////////////////////////////////////
		cl_ok ok;
		cl_int err_code;

		const char* source_ptr = kernel_code.c_str();
		const size_t source_size = kernel_code.size();

		// создать бинарник из кода программы
		const cl_program program = clCreateProgramWithSource(context, 1, &source_ptr, &source_size, &err_code);
		ok = err_code;
		CL_TEST("Create program from source failed")

			// скомпилировать программу
			ok = clBuildProgram(program, 1, &device_id, NULL, NULL, NULL);
		CL_TEST("Build program failed")

			// создать кернел
			kernel = clCreateKernel(program, "cl_main", &err_code);
		ok = err_code;
		CL_TEST("Create kernel failed")

			return ok;
	}

	bool init_arg()
	{
		cl_ok ok;
		cl_int err_code;
		//////////////////////////////////////////////////////////////////////////
		memobj = NULL;
		memItems = 512 * 512;
		memLenth = memItems * sizeof(color32);

		// создать буфер
		memobj = clCreateBuffer(context, CL_MEM_READ_ONLY, memLenth, NULL, &err_code);
		ok = err_code;

		// записать данные в буфер
		//std::vector<cl_int> mem(memItems, 0);
		// ok = clEnqueueWriteBuffer(command_queue, memobj, CL_TRUE, 0, memItems * sizeof(cl_int), &mem[0], 0, NULL, NULL);

		// устанавливаем параметр
		ok = clSetKernelArg(kernel, 0, sizeof(cl_mem), (void*)&memobj);

		return ok;
	}

	bool process_core()
	{
		cl_ok ok;
		cl_int err_code;
		//////////////////////////////////////////////////////////////////////////
		const size_t global_work_size[1] = { memItems };

		// выполнить кернел
		ok = clEnqueueNDRangeKernel(command_queue, kernel, 1, NULL, global_work_size, NULL, 0, NULL, NULL);
		ok = clFinish(command_queue);

		return ok;
	}

	bool read_data(bitmap32& out)
	{
		cl_ok ok;
		cl_int err_code;
		//////////////////////////////////////////////////////////////////////////

		// считать данные из буфера
		ok = clEnqueueReadBuffer(command_queue, memobj, CL_TRUE, 0, memLenth, (void*)out.raw_data(), 0, NULL, NULL);
		ok = clFinish(command_queue);

		return ok;
	}

};

void imview::openCL(view_context*)
{
    const INT2 SIZE{ 512,512 };

    static bitmap32 img_soft(SIZE, color32{ 0,0,0,255 });
    static bitmap32 img_hard(SIZE, color32{ 0,0,0,255 });
    static GpuTexture gpu_soft;
    static GpuTexture gpu_hard;


    //----
    const FLOAT2 A = { 0,   512 };
    const FLOAT2 B = { 256,   0 };
    const FLOAT2 C = { 512, 512 };

    SuperTimer st;

    //////////////////////////////////////////////////////////////////////////
    st.SetStart();
    //#pragma omp parallel for num_threads(8)
    for (int y = 0; y < SIZE.y; y++)
        for (int x = 0; x < SIZE.x; x++)
        {
            uint32_t id = y * SIZE.x + x;
            id = id ^ (id << 13);
            id = id ^ (id >> 17);
            id = id ^ (id << 15);

            const FLOAT2 P(x, y);
            const uint8_t color = math::is_point_in_triangle2D(P, A, B, C) ? 255 : 0;
            img_soft.raw_data(x, y)->r = id;
            img_soft.raw_data(x, y)->g = color;
            img_soft.raw_data(x, y)->b = color;
            img_soft.raw_data(x, y)->a = 255;
        }
    st.SetEnd();
    log::info << str::format::insert("soft_time: {}", st.GetTimeReal());
    //////////////////////////////////////////////////////////////////////////
    static cl_impl cl;
    MakeOnce
    {
        cl.init_device();
        cl.init_code();
        cl.init_arg();
    }
        //----------------
    st.SetStart();
    cl.process_core();
    st.SetEnd();
    log::info << str::format::insert("hard_proc: {}", st.GetTimeReal());
    //----------------
    st.SetStart();
    cl.read_data(img_hard);
    st.SetEnd();
    log::info << str::format::insert("hard_read: {}", st.GetTimeReal());
    //----------------
    //////////////////////////////////////////////////////////////////////////

    gpu_soft.Update(img_soft);
    gpu_hard.Update(img_hard);
    ImItem::ShowImage(&gpu_soft, true, FLOAT2{ (float)SIZE.x, (float)SIZE.y });
    ImGui::SameLine();
    ImItem::ShowImage(&gpu_hard, true, FLOAT2{ (float)SIZE.x, (float)SIZE.y });
}
