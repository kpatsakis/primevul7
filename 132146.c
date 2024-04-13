void CompressTest::testZip64()
{
	typedef std::map<std::string, Poco::UInt64> FileMap;
	std::cout << std::endl;
	FileMap files;
	files["data1.bin"] = static_cast<Poco::UInt64>(KB)*4096+1;
	files["data2.bin"] = static_cast<Poco::UInt64>(KB)*16;
	files["data3.bin"] = static_cast<Poco::UInt64>(KB)*4096-1;
	
	for(FileMap::const_iterator it = files.begin(); it != files.end(); it++)
	{
		std::cout << '\t' << "createDataFile(" << it->first << ", " << it->second << ");" << std::endl;
		createDataFile(it->first, it->second);
	}
	std::ofstream out(Poco::Path::temp() + "zip64.zip", std::ios::binary | std::ios::trunc);
	Compress c(out, true, true);
	for(FileMap::const_iterator it = files.begin(); it != files.end(); it++)
	{
		const std::string& path = it->first;
		std::cout << '\t' << "addFile(" << path <<  ");" << std::endl;
		c.addFile(path, path, ZipCommon::CM_STORE);
	}
	ZipArchive a(c.close());
	for(FileMap::const_iterator it = files.begin(); it != files.end(); it++)
	{
		const std::string& path = it->first;
		Poco::UInt64 size = it->second;
		ZipArchive::FileHeaders::const_iterator it2 = a.findHeader(path);
		assert (it2 != a.headerEnd());
		const Poco::Zip::ZipLocalFileHeader& file = it2->second;
		assert(file.getUncompressedSize() == size);
		assert(file.getCompressedSize() == size);
	}
	for (FileMap::const_iterator it = files.begin(); it != files.end(); it++)
	{
		Poco::File(it->first).remove();
	}
}