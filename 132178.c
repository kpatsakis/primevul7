ZipOutputStream::ZipOutputStream(std::ostream& ostr, ZipLocalFileHeader& fileEntry, bool seekableOutput): ZipIOS(ostr, fileEntry, seekableOutput), std::ostream(&_buf)
{
}