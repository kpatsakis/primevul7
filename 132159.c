ZipStreamBuf::ZipStreamBuf(std::ostream& ostr, ZipLocalFileHeader& fileEntry, bool reposition):
	Poco::BufferedStreamBuf(STREAM_BUFFER_SIZE, std::ios::out),
	_pIstr(0),
	_pOstr(&ostr),
	_ptrBuf(),
	_ptrOBuf(),
	_ptrHelper(),
	_ptrOHelper(),
	_crc32(Poco::Checksum::TYPE_CRC32),
	_expectedCrc32(0),
	_checkCRC(false),
	_bytesWritten(0),
	_pHeader(&fileEntry)
{
	if (fileEntry.isEncrypted())
		throw Poco::NotImplementedException("Encryption not supported");

	if (fileEntry.isDirectory())
	{
		// only header, no payload, zero crc
		fileEntry.setSearchCRCAndSizesAfterData(false);
		fileEntry.setCompressedSize(0);
		fileEntry.setUncompressedSize(0);
		fileEntry.setCRC(0);
		std::string header = fileEntry.createHeader();
		ostr.write(header.c_str(), static_cast<std::streamsize>(header.size()));
	}
	else
	{
		fileEntry.setSearchCRCAndSizesAfterData(!reposition);
		if (fileEntry.getCompressionMethod() == ZipCommon::CM_DEFLATE)
		{
			int level = Z_DEFAULT_COMPRESSION;
			if (fileEntry.getCompressionLevel() == ZipCommon::CL_FAST || fileEntry.getCompressionLevel() == ZipCommon::CL_SUPERFAST)
				level = Z_BEST_SPEED;
			else if (fileEntry.getCompressionLevel() == ZipCommon::CL_MAXIMUM)
				level = Z_BEST_COMPRESSION;
			// ignore the zlib init string which is of size 2 and also ignore the 4 byte adler32 value at the end of the stream!
			_ptrOHelper = new PartialOutputStream(*_pOstr, 2, 4, false);
			_ptrOBuf = new Poco::DeflatingOutputStream(*_ptrOHelper, DeflatingStreamBuf::STREAM_ZLIB, level);
		}
		else if (fileEntry.getCompressionMethod() == ZipCommon::CM_STORE)
		{
			_ptrOHelper = new PartialOutputStream(*_pOstr, 0, 0, false);
			_ptrOBuf = new PartialOutputStream(*_ptrOHelper, 0, 0, false);
		}
		else throw Poco::NotImplementedException("Unsupported compression method");

		// now write the header to the ostr!
        if (fileEntry.needsZip64())
            fileEntry.setZip64Data();
		std::string header = fileEntry.createHeader();
		ostr.write(header.c_str(), static_cast<std::streamsize>(header.size()));
	}
}