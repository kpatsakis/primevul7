void ZipTest::testDecompressZip64()
{
	std::map<std::string, Poco::UInt64> files;
	files[Poco::Path::temp() + "data1.bin"] = static_cast<Poco::UInt64>(KB)*4096+1;
	files[Poco::Path::temp() + "data2.bin"] = static_cast<Poco::UInt64>(KB)*16;
	files[Poco::Path::temp() + "data3.bin"] = static_cast<Poco::UInt64>(KB)*4096-1;

	for(std::map<std::string, Poco::UInt64>::const_iterator it = files.begin(); it != files.end(); it++)
	{
		Poco::File file(it->first);
		if(file.exists())
			file.remove();
	}
	std::ifstream in(Poco::Path::temp() + "zip64.zip", std::ios::binary);
	Decompress c(in, Poco::Path::temp());
	c.decompressAllFiles();
	for(std::map<std::string, Poco::UInt64>::const_iterator it = files.begin(); it != files.end(); it++)
	{
		verifyDataFile(it->first, it->second);
	}
}