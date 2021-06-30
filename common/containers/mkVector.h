#pragma once

#include <vector>
#include <assert.h>

//test
#include "Log.h"

// mkVector：替代std::vector，增加了对索引范围的检查
// 1. 边界
// 2. int
template<class _Ty>
class mkVector
{
public:

	mkVector() :vec() {}
	mkVector(size_t size) :vec(size) {}
	mkVector(size_t size, const _Ty& val) :vec(size, val) {}

	// std::vector也没有使用引用
	// 拷贝或赋值一个initializer_list对象不会拷贝列表中的元素，只是引用，原始列表和副本共享元素
	mkVector(std::initializer_list<_Ty> ilist) :vec(ilist) {}

	mkVector(const mkVector& other) :vec(other.vec) {}
	mkVector(mkVector&& other) :vec(other.vec) {}

	mkVector& operator=(const mkVector& other) { vec = other.vec; return (*this); }
	mkVector& operator=(const mkVector&& other) { vec = other.vec; return (*this); }

	void push_back(const _Ty& val) { vec.push_back(val); }
	void push_back(_Ty&& val) { vec.push_back(val); }

	// 直接抄的std::vector
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

	// 需要typename前缀
	typename std::vector<_Ty>::iterator begin() { return vec.begin(); }
	typename std::vector<_Ty>::const_iterator begin() const { return vec.begin(); } // vector会自动调用const版本的begin
	typename std::vector<_Ty>::iterator end() { return vec.end(); }
	typename std::vector<_Ty>::const_iterator end() const { return vec.end(); }

	// 一些外部的库需要vector
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