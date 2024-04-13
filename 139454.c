jas_image_fmtinfo_t *jas_image_lookupfmtbyid(int id)
{
	int i;
	jas_image_fmtinfo_t *fmtinfo;

	for (i = 0, fmtinfo = jas_image_fmtinfos; i < jas_image_numfmts; ++i, ++fmtinfo) {
		if (fmtinfo->id == id) {
			return fmtinfo;
		}
	}
	return 0;
}