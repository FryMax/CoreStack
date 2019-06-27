#include "globals.h"

LIB_NAMESPACE_START

namespace utka
{
	umanager_class UManagerClass;
}

LIB_NAMESPACE_END


LIB_NAME::utka::uclass::uclass(const char* name, const char* type, const char* func, const char* file, int line)
{
	uclass_meta meta;

	meta.obj_name = std::string(name);
	meta.obj_name.shrink_to_fit();

	meta.obj_type = std::string(type);
	meta.obj_type = meta.obj_type.substr(0, meta.obj_type.find('*'));
	meta.obj_name.shrink_to_fit();

	meta.obj_func = std::string(func);
	meta.obj_func.shrink_to_fit();

	meta.obj_file = std::string(file);
	meta.obj_file.shrink_to_fit();

	meta.obj_line = std::to_string(line);
	meta.obj_line.shrink_to_fit();

	const auto upid = std::to_string((size_t)this);
	UManagerClass.register_uclass(upid, std::move(meta));
};

LIB_NAME::utka::uclass::~uclass()
{
	UManagerClass.release_uclass(std::to_string((size_t)this));
}

//-----------------------------------------------------


//-----------------------------------------------------


//void register_global_uobject(const uclass& inst)
//{
//	const auto& vec = getUCat()->uarray;
//
//	for (const auto& it : vec)
//	{
//		const auto& uit = *it.ptr;
//
//		BREAK_IF(inst.uobject_id == uit.uobject_id);
//		BREAK_IF(inst.uobject_name.empty());
//	}
//
//	getUCat()->uarray.push_back(uclass_meta{ &inst });
//}
//
//void release_global_uobject(const uclass& inst)
//{
//	LOG_WARN_NO_IMPL;
//}
//
//const std::vector<uclass_meta>& get_uclass_list()
//{
//	return getUCat()->uarray;
//}
//
//void LIB_NAME::utka::release_global_uobject(uclass_meta& inst)
//{
//}


