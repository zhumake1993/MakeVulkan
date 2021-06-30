#pragma once

#include "mkString.h"

class NamedObject
{
public:

	NamedObject(const mkString& name) :m_Name(name){}
	virtual ~NamedObject() {}

	mkString GetName() { return m_Name; }

protected:

	mkString m_Name;
};