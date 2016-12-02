#include "stdafx.h"
#include "CppUnitTest.h"
#include "QCDefinitions.h"
#include "QCDate.h"
#include "QCTest.h"
#include "QCInterestRate.h"
#include "QCWealthFactor.h"
#include "QCAct360.h"
#include "QCLinearWf.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace std;

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
			QCWlthFctrShrdPtr linPtr = make_shared<QCWealthFactor>(lin);
			double rate = .01;
			QCInterestRate ir { rate, act360, linPtr };
			QCDate stDt { 1, 1, 2016 };
			QCDate endDt { 26, 12, 2016 };
			cout << ir.wf(stDt, endDt) << endl;
			Assert::AreEqual(1.01, ir.wf(stDt, endDt), 0.00000005, L"Test Failed", LINE_INFO());
		}

	};
}