Decompress::~Decompress()
{
	try
	{
		EOk -= Poco::Delegate<Decompress, std::pair<const ZipLocalFileHeader, const Poco::Path> >(this, &Decompress::onOk);
	}
	catch (...)
	{
		poco_unexpected();
	}
}