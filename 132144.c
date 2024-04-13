ZipIOS::ZipIOS(std::ostream& ostr, ZipLocalFileHeader& fileEntry, bool reposition): _buf(ostr, fileEntry, reposition)
{
	poco_ios_init(&_buf);
}