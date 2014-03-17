#ifndef MOON_TEST_APPLICATION_H
#define MOON_TEST_APPLICATION_H

#include "API\MoonEngine.h"

namespace moon {

	class CTestApplication
	{
		CMoonEngine m_moonEngine;
	public:
		CTestApplication();
		~CTestApplication();
	};

}

#endif