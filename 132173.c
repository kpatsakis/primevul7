Decompress::Decompress(std::istream& in, const Poco::Path& outputDir, bool flattenDirs, bool keepIncompleteFiles):
	_in(in),
	_outDir(outputDir),
	_flattenDirs(flattenDirs),
	_keepIncompleteFiles(keepIncompleteFiles),
	_mapping()
{
	_outDir.makeAbsolute();
	_outDir.makeDirectory();
	if (!_in.good()) throw Poco::IOException("Bad input stream");
	Poco::File tmp(_outDir);
	if (!tmp.exists())
	{
		tmp.createDirectories();
	}
	if (!tmp.isDirectory())
		throw Poco::IOException("Failed to create/open directory: " + _outDir.toString());
	EOk += Poco::Delegate<Decompress, std::pair<const ZipLocalFileHeader, const Poco::Path> >(this, &Decompress::onOk);

}