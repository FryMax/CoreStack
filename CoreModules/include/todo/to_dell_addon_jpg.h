#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <turbojpeg.h>

struct jbuff
{
	const unsigned char* data;
	size_t size;
	size_t format;
	size_t w;
	size_t h;
};

// Note: function store buffer, equal to size of largest encoded image
//
jbuff pack_jbuff(void* data,
	const unsigned w,
	const unsigned h,
	const unsigned channels = 3,
	const unsigned quality = 95)
{
	if (!data)
		throw std::runtime_error("null output buffer");

	unsigned char* dataScr = (unsigned char*)data;

	static unsigned char* jpegBuffer = nullptr;
	static unsigned currnetBufferSize = 0;
	
	unsigned buffSize = w * h * channels * sizeof(unsigned char);

	if (!jpegBuffer || currnetBufferSize < buffSize)
	{
		jpegBuffer = static_cast<unsigned char*>(std::malloc(buffSize));
		currnetBufferSize = buffSize;
	}
	
	unsigned long jpegSize = 0;

	tjhandle jpegCompressor = tjInitCompress();

	tjCompress2(jpegCompressor, 
		dataScr, 
		w, 
		0, 
		h, 
		TJPF_RGB,
		&jpegBuffer, 
		&jpegSize,
		TJSAMP_444, 
		quality,
		TJFLAG_FASTDCT);

	tjDestroy(jpegCompressor);
	
	jbuff out;
	
	out.data = (const unsigned char*)jpegBuffer,
	out.size = jpegSize;
	out.format = TJSAMP_444;
	out.w = w;
	out.h = h;
	
	return out;
}

void unpack_jbuff_to(jbuff pack, unsigned char* outPtr)
{
	if (!outPtr)
		throw std::runtime_error("null output buffer");

	int width = pack.w;
	int height = pack.h;
	int jpegSubsamp = pack.format;
	unsigned char* pEncoded = (unsigned char*)pack.data;
	long unsigned jpegSize = pack.size;

	tjhandle jpegDecompressor = tjInitDecompress();

	tjDecompressHeader2(jpegDecompressor, pEncoded, jpegSize, &width, &height, &jpegSubsamp);
	
	tjDecompress2(jpegDecompressor, 
		(unsigned char*)pack.data,
		jpegSize, 
		outPtr,
		width, 
		0/*pitch*/, 
		height, 
		TJPF_RGB, 
		TJFLAG_FASTDCT);
	
	tjDestroy(jpegDecompressor);
}

 