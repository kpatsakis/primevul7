CppUnit::Test* CompressTest::suite()
{
	CppUnit::TestSuite* pSuite = new CppUnit::TestSuite("CompressTest");

	CppUnit_addTest(pSuite, CompressTest, testSingleFile);
	CppUnit_addTest(pSuite, CompressTest, testDirectory);
	CppUnit_addTest(pSuite, CompressTest, testManipulator);
	CppUnit_addTest(pSuite, CompressTest, testManipulatorDel);
	CppUnit_addTest(pSuite, CompressTest, testManipulatorReplace);
	CppUnit_addTest(pSuite, CompressTest, testSetZipComment);
	CppUnit_addTest(pSuite, CompressTest, testZip64);

	return pSuite;
}