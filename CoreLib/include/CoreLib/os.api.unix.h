#pragma once

#include "globals.h"
#include "os.api.h"

LIB_NAMESPACE_START

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
namespace os_api
{
	//==================================//
	namespace alt
	{

	}
	//=====================================================// System
	inline result< int32_t> get_system_metric(os_sys_metric sm)
	{
		switch (sm)
		{
		case os_sys_metric::VISIBLE_MONITOR_COUNT: return 0;
		case os_sys_metric::MAIN_MONITOR_W:        return 0;
		case os_sys_metric::MAIN_MONITOR_H:        return 0;

		default: break;
		}

		DEBUG_BREAK();
		return error_result(0, "Failed to get selected system metric");
	}

	inline result<uint64_t> get_ram_metric(os_ram_metric sm)
	{
		switch (sm)
		{
		case os_ram_metric::MEM_MemoryLoadProcent:			return 0;
		case os_ram_metric::MEM_PageFileSize:				return 0;
		case os_ram_metric::MEM_PageFileLoad:				return 0;
		case os_ram_metric::MEM_PageFileAvail:				return 0;
		case os_ram_metric::MEM_VirtualMemoryTotal:			return 0;
		case os_ram_metric::MEM_VirtualMemoryLoad:			return 0;
		case os_ram_metric::MEM_VirtualMemoryAvail:			return 0;
		case os_ram_metric::MEM_RealMemoryTotal:			return 0;
		case os_ram_metric::MEM_RealMemoryLoad:				return 0;
		case os_ram_metric::MEM_RealMemoryAvail:			return 0;
		case os_ram_metric::MEM_CurentProcVirtualMemUsage:	return 0;
		case os_ram_metric::MEM_CurentProcRealMemUsage:		return 0;
		default:
			break;
		}

		DEBUG_BREAK();
		return error_result(0ULL, "Failed to get selected ram metric");
	}

	inline result< int32_t> get_cpu_metric(os_cpu_metric sm)
	{
		switch (sm)
		{
		case os_cpu_metric::CPU_CpuLoadProcent:	(void)0;
		case os_cpu_metric::CPU_CpuNodeCount:	(void)0;
		case os_cpu_metric::CPU_CpuCoreCount:	(void)0;
		case os_cpu_metric::CPU_CpuThreadCount:	return 0; //std::thread::hardware_concurrency();

		default:
			break;
		}

		DEBUG_BREAK();
		return error_result(0, "Failed to get selected cpu metric");
	}
	
};

LIB_NAMESPACE_END