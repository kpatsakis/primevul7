void ZipOutputStream::close(Poco::UInt64& extraDataSize)
{
	flush();
	_buf.close(extraDataSize);
}