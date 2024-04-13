void CompressTest::testManipulator()
{
	{
		std::ofstream out(Poco::Path::temp() + "appinf.zip", std::ios::binary);
		Poco::Path theFile(ZipTest::getTestFile("data", "test.zip"));
		Compress c(out, true);
		c.addFile(theFile, theFile.getFileName());
		ZipArchive a(c.close());
	}
	ZipManipulator zm(Poco::Path::temp() + "appinf.zip", true);
	zm.renameFile("test.zip", "renamedtest.zip");
	zm.addFile("doc/othertest.zip", ZipTest::getTestFile("data", "test.zip"));
	ZipArchive archive=zm.commit();
	assert (archive.findHeader("doc/othertest.zip") != archive.headerEnd());
}