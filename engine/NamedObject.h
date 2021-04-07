#pragma once

#include <string>

class NamedObject
{
public:

	NamedObject(const std::string& name) :m_Name(name){}
	virtual ~NamedObject() {}

	std::string GetName() { return m_Name; }

protected:

	std::string m_Name;
};