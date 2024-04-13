bool ZipInputStream::crcValid() const
{
	return _buf.crcValid();
}