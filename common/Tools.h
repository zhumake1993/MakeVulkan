#pragma once

#include "Log.h"
#include <vector>

// pointer�п�����һ�����ʽ����Ҫ��ת����һ������
#define RELEASE(pointer)	\
{							\
	auto ptr = pointer;		\
	assert(ptr);			\
	delete ptr;				\
}

#define ASSERT(x) assert(x);

//#define EXIT assert(false)

#define ALIGN(val, alignment) (((val) + (alignment) - 1) & ~((alignment) - 1))

// �����ļ�·�������ļ�����
// ����android�������ļ������apk�У�������ѹ���ĸ�ʽ�������Ҫ�����⴦��
std::vector<char> GetBinaryFileContents(std::string const &filename);

// Wrapper functions for aligned memory allocation
// There is currently no standard for this in C++ that works across all platforms and vendors, so we abstract this
void* AlignedAlloc(size_t size, size_t alignment);

void AlignedFree(void* data);