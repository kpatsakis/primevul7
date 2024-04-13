ZipIOS::ZipIOS(std::istream& istr, const ZipLocalFileHeader& fileEntry, bool reposition): _buf(istr, fileEntry, reposition)
{
	poco_ios_init(&_buf);
}