#pragma once

#include <vector>

// mkVector：替代std::vector，增加了对索引范围的检查
template<class T_>
class mkVector
{
public:

	mkVector(size_t size) :vec(size) {}

	const T_ &		operator[](int index) const { assert(0 <= index && (size_t)index < vec.size()); return vec[index]; }
	T_ &			operator[](int index) { assert(0 <= index && (size_t)index < vec.size()); return vec[index]; }

private:
	std::vector<T_>	vec;
};