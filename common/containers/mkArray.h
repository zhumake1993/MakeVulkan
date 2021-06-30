#pragma once

#include <assert.h>

// mkArray：替代C数组，增加了对索引范围的检查
// 参考自vkDOOM3
template<class T_, int numElements>
class mkArray
{
public:

	// 将ptr里的数据初始化为默认值
	mkArray() :ptr() { assert(numElements >= 0); }

	// returns number of elements in list
	int				Num() const { return numElements; }

	// returns the number of bytes the array takes up
	int				ByteSize() const { return sizeof(ptr); }

	// memset the entire array to zero
	void			Zero() { memset(ptr, 0, sizeof(ptr)); }

	// memset the entire array to a specific value
	void			Memset(const char fill) { memset(ptr, fill, numElements * sizeof(*ptr)); }

	// array operators
	const T_ &		operator[](int index) const { assert(0 <= index && index < numElements); return ptr[index]; }
	T_ &			operator[](int index) { assert(0 <= index && index < numElements); return ptr[index]; }

	// returns a pointer to the list
	const T_ *		Ptr() const { return ptr; }
	T_ *			Ptr() { return ptr; }

private:
	T_				ptr[numElements];
};