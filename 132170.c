void CompressTest::createDataFile(const std::string& path, Poco::UInt64 size)
{
	std::ofstream out(path.c_str(), std::ios::binary | std::ios::trunc);
	assert( ! out.fail() );
	Poco::Buffer<char> buffer(MB);
	for(int i = 0; size != 0; i++) {
		std::memset(buffer.begin(), i, buffer.size());
		Poco::UInt64 bytesToWrite = std::min(size, static_cast<Poco::UInt64>(buffer.size()));
		out.write(buffer.begin(), bytesToWrite);
		assert( ! out.fail() );
		size -= bytesToWrite;
	}
	out.flush();
	assert( ! out.fail() );
	out.close();
	assert( ! out.fail() );
}