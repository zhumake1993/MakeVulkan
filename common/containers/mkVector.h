#pragma once

#include <vector>
#include <assert.h>

// mkVector�����std::vector
// 1. ���Ӷ�Խ��ļ�顣std::vector��operator[]����û��Խ���飬һ��Խ�磬������֪��Խ��ľ���ط�
// 2. ȫ����int�������ϲ����ʡ�����ǿ������ת��
template<class _Ty>
class mkVector
{
public:

	mkVector() {}
	mkVector(int size) :vec(size) { assert(size >= 0); }
	mkVector(int size, const _Ty& val) :vec(size, val) { assert(size >= 0); }

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

	const _Ty &		operator[](int index) const { assert(0 <= index && index < static_cast<int>(vec.size())); return vec[index]; }
	_Ty &			operator[](int index) { assert(0 <= index && index < static_cast<int>(vec.size())); return vec[index]; }

	_Ty& back() { return vec.back(); }
	const _Ty& back() const { return vec.back(); }

	int size() const { return static_cast<int>(vec.size()); }

	_Ty * data() { return vec.data(); }
	const _Ty * data() const{ return vec.data(); }

	void clear() { vec.clear(); }

	void resize(int size) { vec.resize(size); }

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