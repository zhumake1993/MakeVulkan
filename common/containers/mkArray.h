#pragma once

#include <assert.h>

// mkArray：替代C数组
// 1.增加了对越界的检查
// 2.参考自vkDOOM3
template<class T_, int numElements>
class mkArray
{
public:

	mkArray() { assert(numElements >= 0); }

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
	T_				ptr[numElements]{}; // 数组需要显示初始化
};