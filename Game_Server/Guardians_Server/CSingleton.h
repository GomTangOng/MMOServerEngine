#pragma once

template <class T>
class CSingleton
{
private :
	static T m_pInstance;
public : 
	static T* GetInstance()
	{
		return &m_pInstance;
	}
};