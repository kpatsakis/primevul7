void CompressTest::testSingleFile()
{
	std::ofstream out(Poco::Path::temp() + "appinf.zip", std::ios::binary);
	Poco::Path theFile(ZipTest::getTestFile("data", "test.zip"));
	Compress c(out, true);
	c.addFile(theFile, theFile.getFileName());
	ZipArchive a(c.close());
}