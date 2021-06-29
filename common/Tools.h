#pragma once

#include "Log.h"
#include <vector>
#include "mkString.h"

// pointer有可能是一个表达式，需要先转换成一个变量
#define RELEASE(pointer)	\
{							\
	auto ptr = pointer;		\
	if(ptr)					\
		delete ptr;			\
}

#define ASSERT(x) assert(x);

//#define EXIT assert(false)

#define ALIGN(val, alignment) (((val) + (alignment) - 1) & ~((alignment) - 1))

// 根据文件路径返回文件内容
// 由于android上数据文件存放在apk中，并且是压缩的格式，因此需要做特殊处理
std::vector<char> GetBinaryFileContents(mkString const &filename);

// Wrapper functions for aligned memory allocation
// There is currently no standard for this in C++ that works across all platforms and vendors, so we abstract this
void* AlignedAlloc(size_t size, size_t alignment);

void AlignedFree(void* data);