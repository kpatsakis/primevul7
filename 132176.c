void ZipTest::testDecompressSingleFile()
{
	std::string testFile = getTestFile("data", "test.zip");
	std::ifstream inp(testFile.c_str(), std::ios::binary);
	assert (inp.good());
	ZipArchive arch(inp);
	ZipArchive::FileHeaders::const_iterator it = arch.findHeader("testfile.txt");
	assert (it != arch.headerEnd());
	ZipInputStream zipin (inp, it->second);
	std::ostringstream out(std::ios::binary);
	Poco::StreamCopier::copyStream(zipin, out);
	assert(!out.str().empty());
}