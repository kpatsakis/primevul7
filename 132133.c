void ZipTest::testSkipSingleFile()
{
	std::string testFile = getTestFile("data", "test.zip");
	std::ifstream inp(testFile.c_str(), std::ios::binary);
	assert (inp.good());
	SkipCallback skip;
	ZipLocalFileHeader hdr(inp, false, skip);
	assert (ZipCommon::HS_FAT == hdr.getHostSystem());
	int major = hdr.getMajorVersionNumber();
	int POCO_UNUSED minor = hdr.getMinorVersionNumber();
	assert (major <= 2);
	std::size_t hdrSize = hdr.getHeaderSize();
	assert (hdrSize > 30);
	ZipCommon::CompressionMethod POCO_UNUSED cm = hdr.getCompressionMethod();
	assert (!hdr.isEncrypted());
	Poco::DateTime aDate = hdr.lastModifiedAt();
	Poco::UInt64 POCO_UNUSED cS = hdr.getCompressedSize();
	Poco::UInt64 POCO_UNUSED uS = hdr.getUncompressedSize();
	const std::string& POCO_UNUSED fileName = hdr.getFileName();
}