void CompressTest::testManipulatorDel()
{
	{
		std::ofstream out(Poco::Path::temp() + "appinf.zip", std::ios::binary);
		Poco::Path theFile(ZipTest::getTestFile("data", "test.zip"));
		Compress c(out, true);
		c.addFile(theFile, theFile.getFileName());
		ZipArchive a(c.close());
	}
	ZipManipulator zm(Poco::Path::temp() + "appinf.zip", true);
	zm.deleteFile("test.zip");
	zm.addFile("doc/data.zip", ZipTest::getTestFile("data", "data.zip"));
	ZipArchive archive=zm.commit();
	assert (archive.findHeader("test.zip") == archive.headerEnd());
	assert (archive.findHeader("doc/data.zip") != archive.headerEnd());
}