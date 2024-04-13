ZipArchive Decompress::decompressAllFiles()
{
	poco_assert (_mapping.empty());
	ZipArchive arch(_in, *this);
	return arch;
}