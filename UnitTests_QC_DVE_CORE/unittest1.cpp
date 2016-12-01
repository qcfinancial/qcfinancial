#include "stdafx.h"
#include "CppUnitTest.h"
#include "QCDate.h"
#include "QCTest.h"
#include "QCInterestRate.h"
#include "QCWealthFactor.h"
#include "QCAct360.h"
#include "QCLinearWf.h"

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

	TEST_CLASS(UnitTestQCInterestRate)
	{
	public:

		TEST_METHOD(TestLinAct360)
		{
			QCAct360 act360;
			QCLinearWf lin;
			double rate = .01;
			QCInterestRate ir { rate, act360, lin };
			QCDate stDt { 1, 1, 2016 };
			QCDate endDt { 13, 2, 2016 };
			Assert::AreEqual(.00119444, ir.wf(stDt, endDt), 0.00000005, L"Test Failed", LINE_INFO());
		}

	};
}