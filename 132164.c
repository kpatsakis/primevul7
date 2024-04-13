void ZipTest::testValidPath()
{
	assert (ZipCommon::isValidPath("."));
	assert (ZipCommon::isValidPath("file.txt"));
	assert (ZipCommon::isValidPath(".file.txt"));
	assert (ZipCommon::isValidPath("..file.txt"));
	assert (ZipCommon::isValidPath("file.txt.."));
	assert (ZipCommon::isValidPath(".file..txt"));
	assert (ZipCommon::isValidPath("~file..txt"));
	assert (ZipCommon::isValidPath("~file/~"));
	assert (ZipCommon::isValidPath("dir/~"));
	assert (ZipCommon::isValidPath("some"));
	assert (ZipCommon::isValidPath("some/dir"));
	assert (ZipCommon::isValidPath("some/dir/or/another"));
	assert (ZipCommon::isValidPath("some/dir/./another"));
	assert (ZipCommon::isValidPath("some/dir/or/another/file.txt"));
	assert (ZipCommon::isValidPath("s~me\\d.r\\.or..\\an..her\\file.txt"));
	assert (ZipCommon::isValidPath("some\\dir\\or\\another"));
	assert (ZipCommon::isValidPath("some\\dir\\or\\another\\file.txt"));
	assert (ZipCommon::isValidPath("s~me\\d.r/.or..\\an..her\\file.txt"));

	assert (!ZipCommon::isValidPath("/../"));
	assert (!ZipCommon::isValidPath("/"));
	assert (!ZipCommon::isValidPath("\\..\\"));
	assert (!ZipCommon::isValidPath("/..\\"));
	assert (!ZipCommon::isValidPath("\\../"));
	assert (!ZipCommon::isValidPath(".."));
	assert (!ZipCommon::isValidPath("~/"));
	assert (!ZipCommon::isValidPath("~/~"));
	assert (!ZipCommon::isValidPath("/~"));
	assert (!ZipCommon::isValidPath("/file.txt"));
	assert (!ZipCommon::isValidPath("~/file.txt"));
	assert (!ZipCommon::isValidPath("some/dir/or/../another/file.txt"));
	assert (!ZipCommon::isValidPath("C:\\Windows\\system32"));
}