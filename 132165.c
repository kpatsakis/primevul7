void Decompress::onOk(const void*, std::pair<const ZipLocalFileHeader, const Poco::Path>& val)
{
	_mapping.insert(std::make_pair(val.first.getFileName(), val.second));
}