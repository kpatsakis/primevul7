int jas_image_getfmt(jas_stream_t *in)
{
	jas_image_fmtinfo_t *fmtinfo;
	int found;
	int i;

	/* Check for data in each of the supported formats. */
	found = 0;
	for (i = 0, fmtinfo = jas_image_fmtinfos; i < jas_image_numfmts; ++i,
	  ++fmtinfo) {
		if (fmtinfo->ops.validate) {
			/* Is the input data valid for this format? */
			if (!(*fmtinfo->ops.validate)(in)) {
				found = 1;
				break;
			}
		}
	}
	return found ? fmtinfo->id : (-1);
}