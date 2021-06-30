#pragma once

#include <vector>
#include <assert.h>

//test
#include "Log.h"

// mkVector�����std::vector�������˶�������Χ�ļ��
// 1. �߽�
// 2. int
template<class _Ty>
class mkVector
{
public:

	mkVector() :vec() {}
	mkVector(size_t size) :vec(size) {}
	mkVector(size_t size, const _Ty& val) :vec(size, val) {}

	// std::vectorҲû��ʹ������
	// ������ֵһ��initializer_list���󲻻´���б��е�Ԫ�أ�ֻ�����ã�ԭʼ�б�͸�������Ԫ��
	mkVector(std::initializer_list<_Ty> ilist) :vec(ilist) {}

	mkVector(const mkVector& other) :vec(other.vec) {}
	mkVector(mkVector&& other) :vec(other.vec) {}

	mkVector& operator=(const mkVector& other) { vec = other.vec; return (*this); }
	mkVector& operator=(const mkVector&& other) { vec = other.vec; return (*this); }

	void push_back(const _Ty& val) { vec.push_back(val); }
	void push_back(_Ty&& val) { vec.push_back(val); }

	// ֱ�ӳ���std::vector
	template<class... _Valty>
	decltype(auto) emplace_back(_Valty&&... val) { return vec.emplace_back(std::forward<_Valty>(val)...); }

	const _Ty &		operator[](int index) const { assert(0 <= index && (size_t)index < vec.size()); return vec[index]; }
	_Ty &			operator[](int index) { assert(0 <= index && (size_t)index < vec.size()); return vec[index]; }

	_Ty& back() { return vec.back(); }
	const _Ty& back() const { return vec.back(); }

	size_t size() const { return vec.size(); }

	_Ty * data() { return vec.data(); }
	const _Ty * data() const{ return vec.data(); }

	void clear() { vec.clear(); }

	void resize(size_t size) { vec.resize(size); }

	// ��Ҫtypenameǰ׺
	typename std::vector<_Ty>::iterator begin() { return vec.begin(); }
	typename std::vector<_Ty>::const_iterator begin() const { return vec.begin(); } // vector���Զ�����const�汾��begin
	typename std::vector<_Ty>::iterator end() { return vec.end(); }
	typename std::vector<_Ty>::const_iterator end() const { return vec.end(); }

	// һЩ�ⲿ�Ŀ���Ҫvector
	std::vector<_Ty>& vector() { return vec; }
	const std::vector<_Ty>& vector() const { return vec; }

private:

	std::vector<_Ty> vec;
};

template<class _Ty>
bool operator==(const mkVector<_Ty>& left, const mkVector<_Ty>& right)
{
	return left.vector() == right.vector();
}

template<class _Ty>
bool operator!=(const mkVector<_Ty>& left, const mkVector<_Ty>& right)
{
	return !(left.vector() == right.vector());
}