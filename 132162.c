bool ZipStreamBuf::crcValid() const
{
	if (!_ptrBuf) return true; // directory entry
	return _crc32.checksum() == _expectedCrc32;
}