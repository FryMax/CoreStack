#pragma once

#include "compress.h"
//#include <3rd-party/miniz-2.1.0/miniz.h>

////////////////////////////////////////////////////////////////////////////////////

struct cmp_meta
{
    uint64_t size_src;
    uint64_t size_cmp;
    //uint8_t padding[4];//todo
};
// compression structure
// [64-size][64-reserved][data]

LIB_NAMESPACE_START

void Compress(const byte_t* input, uint64_t len, std::string* output)
{
    //mz_ulong OutLen = compressBound(len);
    //byte_t* pOut = (byte_t*)malloc((size_t)OutLen);
    //BREAK_IF(Z_OK != compress2((unsigned char*)pOut, &OutLen, (const unsigned char*)input, (mz_ulong)len, MZ_BEST_SPEED));
    //
    /////////////////////////////////
    //cmp_meta meta;
    //meta.size_src = len;
    //meta.size_cmp = OutLen;
    //
    //std::string buffer(sizeof(cmp_meta) + OutLen, 0);
    //std::memcpy(&buffer[0], &meta, sizeof(cmp_meta));
    //std::memcpy(&buffer[sizeof(cmp_meta)], pOut, OutLen);
    //
    /////////////////////////////////
    //*output = std::move(buffer);
    //free(pOut);
}

void Uncompress(const byte_t* input, std::string* output)
{
    //const cmp_meta& meta = *reinterpret_cast<const cmp_meta*>(input);
    /////////////////////////////////
    //mz_ulong OutLen = meta.size_src;
    //byte_t* pOut = (byte_t*)malloc((size_t)OutLen);
    //BREAK_IF(Z_OK != uncompress((unsigned char*)pOut, &OutLen, (const unsigned char*)(input + sizeof(cmp_meta)), (mz_ulong)meta.size_cmp));
    //
    /////////////////////////////////
    //*output = std::string((const char*)pOut, (size_t)OutLen);
    //free(pOut);
}

LIB_NAMESPACE_END