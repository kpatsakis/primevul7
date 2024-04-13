ZipStreamBuf::ZipStreamBuf(std::istream& istr, const ZipLocalFileHeader& fileEntry, bool reposition):
	Poco::BufferedStreamBuf(STREAM_BUFFER_SIZE, std::ios::in),
	_pIstr(&istr),
	_pOstr(0),
	_ptrBuf(),
	_ptrOBuf(),
	_ptrHelper(),
	_ptrOHelper(),
	_crc32(Poco::Checksum::TYPE_CRC32),
	_expectedCrc32(0),
	_checkCRC(true),
	_bytesWritten(0),
	_pHeader(0)
{
	if (fileEntry.isDirectory())
		return;
	_expectedCrc32 = fileEntry.getCRC();
	std::streamoff start = fileEntry.getDataStartPos();
	std::streamoff end = fileEntry.getDataEndPos();
	_checkCRC = !fileEntry.searchCRCAndSizesAfterData();
	if (fileEntry.getCompressionMethod() == ZipCommon::CM_DEFLATE)
	{
		// Fake init bytes at beginning of stream
		std::string init = ZipUtil::fakeZLibInitString(fileEntry.getCompressionLevel());

		// Fake adler at end of stream: just some dummy value, not checked anway
		std::string crc(4, ' ');
		if (fileEntry.searchCRCAndSizesAfterData())
		{
			_ptrHelper = new AutoDetectInputStream(istr, init, crc, reposition, static_cast<Poco::UInt32>(start), fileEntry.needsZip64());
		}
		else
		{
			_ptrHelper = new PartialInputStream(istr, start, end, reposition, init, crc);
		}
		_ptrBuf = new Poco::InflatingInputStream(*_ptrHelper, Poco::InflatingStreamBuf::STREAM_ZIP);
	}
	else if (fileEntry.getCompressionMethod() == ZipCommon::CM_STORE)
	{
		if (fileEntry.searchCRCAndSizesAfterData())
		{
			_ptrBuf = new AutoDetectInputStream(istr, "", "", reposition, static_cast<Poco::UInt32>(start), fileEntry.needsZip64());
		}
		else
		{
			_ptrBuf = new PartialInputStream(istr, start, end, reposition);
		}
	}
	else throw Poco::NotImplementedException("Unsupported compression method");
}