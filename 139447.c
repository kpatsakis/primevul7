int jas_image_strtofmt(char *name)
{
	jas_image_fmtinfo_t *fmtinfo;
	if (!(fmtinfo = jas_image_lookupfmtbyname(name))) {
		return -1;
	}
	return fmtinfo->id;
}