char *jas_image_fmttostr(int fmt)
{
	jas_image_fmtinfo_t *fmtinfo;
	if (!(fmtinfo = jas_image_lookupfmtbyid(fmt))) {
		return 0;
	}
	return fmtinfo->name;
}