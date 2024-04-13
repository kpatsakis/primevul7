jas_image_fmtinfo_t *jas_image_lookupfmtbyname(const char *name)
{
	int i;
	jas_image_fmtinfo_t *fmtinfo;

	for (i = 0, fmtinfo = jas_image_fmtinfos; i < jas_image_numfmts; ++i, ++fmtinfo) {
		if (!strcmp(fmtinfo->name, name)) {
			return fmtinfo;
		}
	}
	return 0;
}