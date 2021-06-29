#pragma once

#include <vector>

// mkVector�����std::vector�������˶�������Χ�ļ��
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