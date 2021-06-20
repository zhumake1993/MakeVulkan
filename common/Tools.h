#pragma once

#include "Env.h"
#include "Log.h"

#define RELEASE(pointer)		\
{								\
	if (pointer != nullptr) {	\
		delete pointer;			\
		pointer = nullptr;		\
	}							\
}

#define ASSERT(x) assert(x);

//#define ASSERT(x)													\
//{																	\
//	if(!(x))														\
//	{																\
//		LOG("Assert false in %s at line %d", __FILE__, __LINE__);	\
//	}																\
//	assert(x);														\
//}

#define EXIT assert(false)

// 根据文件路径返回文件内容
// 由于android上数据文件存放在apk中，并且是压缩的格式，因此需要做特殊处理
std::vector<char> GetBinaryFileContents(std::string const &filename);

// Wrapper functions for aligned memory allocation
// There is currently no standard for this in C++ that works across all platforms and vendors, so we abstract this
void* AlignedAlloc(size_t size, size_t alignment);

void AlignedFree(void* data);