int jas_image_encode(jas_image_t *image, jas_stream_t *out, int fmt, char *optstr)
{
	jas_image_fmtinfo_t *fmtinfo;
	if (!(fmtinfo = jas_image_lookupfmtbyid(fmt))) {
		return -1;
	}
	return (fmtinfo->ops.encode) ? (*fmtinfo->ops.encode)(image, out,
	  optstr) : (-1);
}