#include "stdafx.h"
#include "CppUnitTest.h"
#include "QCTest.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTests_QC_DVE_CORE
{		
	TEST_CLASS(UnitTestQCTest)
	{
	public:
		
		TEST_METHOD(TestSumaSimple)
		{
			QCTest qct;
			Assert::AreEqual(6, qct.suma(3, 3), 0, L"Test Failed", LINE_INFO());
		}

	};
}