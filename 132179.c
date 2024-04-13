void CompressTest::testManipulatorReplace()
{
	{
		std::ofstream out(Poco::Path::temp() + "appinf.zip", std::ios::binary);
		Poco::Path theFile(ZipTest::getTestFile("data", "test.zip"));
		Compress c(out, true);
		c.addFile(theFile, theFile.getFileName());
		ZipArchive a(c.close());
	}
	ZipManipulator zm(Poco::Path::temp() + "appinf.zip", true);
	zm.replaceFile("test.zip", ZipTest::getTestFile("data", "doc.zip"));
	
	ZipArchive archive=zm.commit();
	assert (archive.findHeader("test.zip") != archive.headerEnd());
	assert (archive.findHeader("doc.zip") == archive.headerEnd());
}