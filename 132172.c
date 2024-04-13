ZipInputStream::ZipInputStream(std::istream& istr, const ZipLocalFileHeader& fileEntry, bool reposition): ZipIOS(istr, fileEntry, reposition), std::istream(&_buf)
{
}