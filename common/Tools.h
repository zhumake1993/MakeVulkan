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

#define ASSERT(x,...)			\
{								\
	if(!(x)) LOG(__VA_ARGS__);	\
	assert(x);					\
}


#define EXIT assert(false)

// �����ļ�·�������ļ�����
// ����android�������ļ������apk�У�������ѹ���ĸ�ʽ�������Ҫ�����⴦��
std::vector<char> GetBinaryFileContents(std::string const &filename);