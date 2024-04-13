int jas_image_fmtfromname(char *name)
{
	int i;
	char *ext;
	jas_image_fmtinfo_t *fmtinfo;
	/* Get the file name extension. */
	if (!(ext = strrchr(name, '.'))) {
		return -1;
	}
	++ext;
	/* Try to find a format that uses this extension. */	
	for (i = 0, fmtinfo = jas_image_fmtinfos; i < jas_image_numfmts; ++i,
	  ++fmtinfo) {
		/* Do we have a match? */
		if (!strcmp(ext, fmtinfo->ext)) {
			return fmtinfo->id;
		}
	}
	return -1;
}