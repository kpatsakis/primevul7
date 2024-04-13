int ZipStreamBuf::writeToDevice(const char* buffer, std::streamsize length)
{
	if (!_ptrOBuf) return 0; // directory entry
	if (length == 0)
		return 0;
	_bytesWritten += length;
	_ptrOBuf->write(buffer, length);
	_crc32.update(buffer, static_cast<unsigned int>(length));
	return static_cast<int>(length);
}