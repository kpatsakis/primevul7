void jas_image_clearfmts()
{
	int i;
	jas_image_fmtinfo_t *fmtinfo;
	for (i = 0; i < jas_image_numfmts; ++i) {
		fmtinfo = &jas_image_fmtinfos[i];
		if (fmtinfo->name) {
			jas_free(fmtinfo->name);
			fmtinfo->name = 0;
		}
		if (fmtinfo->ext) {
			jas_free(fmtinfo->ext);
			fmtinfo->ext = 0;
		}
		if (fmtinfo->desc) {
			jas_free(fmtinfo->desc);
			fmtinfo->desc = 0;
		}
	}
	jas_image_numfmts = 0;
}