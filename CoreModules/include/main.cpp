#include "shared.h"

#undef main // disable fake main interceptors
int main()
{
	for (const auto& it : GetBuildInfo())
	{
		log::info << str::format::insert("{:<10} : {}", it.first, it.second);
	}

	log::warn << "--------------------------------";
	log::warn << "this is just shared-code project";

	return 0;
}