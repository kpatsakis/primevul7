void *gdImageGd2Ptr (gdImagePtr im, int cs, int fmt, int *size)
{
	void *rv;
	gdIOCtx *out = gdNewDynamicCtx(2048, NULL);

	_gdImageGd2(im, out, cs, fmt);
	rv = gdDPExtractData(out, size);
	out->gd_free(out);

	return rv;
}