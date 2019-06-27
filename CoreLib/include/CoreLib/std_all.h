#pragma once

#pragma warning(push)
#pragma warning(disable : 4005)
#pragma warning(disable : 4061)
#pragma warning(disable : 4365)
#pragma warning(disable : 4371)
#pragma warning(disable : 4548)
#pragma warning(disable : 4619)
#pragma warning(disable : 4640)
#pragma warning(disable : 4668)
#pragma warning(disable : 4710)
#pragma warning(disable : 4820)
#pragma warning(disable : 4826)

//////////////////////////////////////////////////////////////////////////
//  add only std headers here
//////////////////////////////////////////////////////////////////////////

#include <cassert>       //
#include <stdexcept>     //
                         //
#include <cstring>       //
#include <iostream>      //
#include <sstream>       //
#include <fstream>       //
#include <codecvt>       // wchar convert
                         //
#include <ctime>         // c-format time
#include <climits>       //
#include <clocale>       //
#include <csetjmp>       // todo delete me
#include <cstdarg>       // va_args
#include <cstdlib>       // stdlib rand
                         //
#include <set>           //
#include <map>           //
#include <list>          //
#include <array>         //
#include <deque>         //
#include <vector>        //
#include <unordered_map> //
                         //
#include <chrono>        //
#include <random>        //
#include <atomic>        //
#include <functional>    //

//-----------------------//

#include <mutex>         //

//#include <thread>      //
//#include <future>      // std::async, std::future
//#include <optional>    //
//#include <filesystem>  //

//////////////////////////////////////////////////////////////////////////
//  std headers end
//////////////////////////////////////////////////////////////////////////

#pragma warning(pop)

/* check utils.io.h

#include <experimental/filesystem>

namespace std
{
	namespace fs = std::experimental::filesystem;
}

namespace fs = std::experimental::filesystem;

*/




