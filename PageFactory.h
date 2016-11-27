#ifndef _PAGEFACTORY_h
#define _PAGEFACTORY_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include "SimpleList.h"

class IPage;

class PageFactory
{
public:
	template <class T>
	static T* Create()
	{
		T* page = new T();
		m_pages.push_back(page);
		return page;
	}

	// HACK to support MainPage
	template <class T, typename U>
	static T* Create(U u)
	{
		T* page = new T(u);
		m_pages.push_back(page);
		return page;
	}

	static const SimpleList<IPage*> GetPages();

private:
	static SimpleList<IPage*> m_pages;
};

#endif

