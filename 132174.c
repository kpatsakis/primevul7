ZipStreamBuf::~ZipStreamBuf()
{
	// make sure destruction of streams happens in correct order
	_ptrOBuf = 0;
	_ptrOHelper = 0;
	_ptrBuf = 0;
	_ptrHelper = 0;
}