void ZipStreamBuf::close(Poco::UInt64& extraDataSize)
{
	extraDataSize = 0;
	if (_ptrOBuf && _pHeader)
	{
		_ptrOBuf->flush();
		DeflatingOutputStream* pDO = dynamic_cast<DeflatingOutputStream*>(_ptrOBuf.get());
		if (pDO)
			pDO->close();
		if (_ptrOHelper)
		{
			_ptrOHelper->flush();
			_ptrOHelper->close();
		}
		_ptrOBuf = 0;
		if (!*_pOstr) throw Poco::IOException("Bad output stream");

		// write an extra datablock if required
		// or fix the crc entries
		poco_check_ptr(_pHeader);
		_pHeader->setCRC(_crc32.checksum());
		_pHeader->setUncompressedSize(_bytesWritten);
		_pHeader->setCompressedSize(_ptrOHelper->bytesWritten());
		if (_bytesWritten == 0)
		{
			poco_assert (_ptrOHelper->bytesWritten() == 0);
			// Empty files must use CM_STORE, otherwise unzipping will fail
			_pHeader->setCompressionMethod(ZipCommon::CM_STORE);
			_pHeader->setCompressionLevel(ZipCommon::CL_NORMAL);
		}
		_pHeader->setStartPos(_pHeader->getStartPos()); // This resets EndPos now that compressed Size is known

		if (_pHeader->searchCRCAndSizesAfterData())
		{
            if (_pHeader->needsZip64())
            {
			    ZipDataInfo64 info;
			    info.setCRC32(_crc32.checksum());
			    info.setUncompressedSize(_bytesWritten);
			    info.setCompressedSize(_ptrOHelper->bytesWritten());
                extraDataSize = info.getFullHeaderSize();
			    _pOstr->write(info.getRawHeader(), static_cast<std::streamsize>(extraDataSize));
            }
            else
            {
 			    ZipDataInfo info;
			    info.setCRC32(_crc32.checksum());
			    info.setUncompressedSize(static_cast<Poco::UInt32>(_bytesWritten));
			    info.setCompressedSize(static_cast<Poco::UInt32>(_ptrOHelper->bytesWritten()));
                extraDataSize = info.getFullHeaderSize();
			    _pOstr->write(info.getRawHeader(), static_cast<std::streamsize>(extraDataSize));
           }
		}
		else
		{
			_pOstr->seekp(_pHeader->getStartPos(), std::ios_base::beg);
			if (!*_pOstr) throw Poco::IOException("Bad output stream");

            if (_pHeader->hasExtraField())   // Update sizes in header extension.
                _pHeader->setZip64Data();
			std::string header = _pHeader->createHeader();
			_pOstr->write(header.c_str(), static_cast<std::streamsize>(header.size()));
			_pOstr->seekp(0, std::ios_base::end);
			if (!*_pOstr) throw Poco::IOException("Bad output stream");
		}
		_pHeader = 0;
	}
}