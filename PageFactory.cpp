#include "PageFactory.h"

SimpleList<IPage*> PageFactory::m_pages;

const SimpleList<IPage*> PageFactory::GetPages()
{
	return m_pages;
}
