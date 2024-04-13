int jas_image_addfmt(int id, char *name, char *ext, char *desc,
  jas_image_fmtops_t *ops)
{
	jas_image_fmtinfo_t *fmtinfo;
	assert(id >= 0 && name && ext && ops);
	if (jas_image_numfmts >= JAS_IMAGE_MAXFMTS) {
		return -1;
	}
	fmtinfo = &jas_image_fmtinfos[jas_image_numfmts];
	fmtinfo->id = id;
	if (!(fmtinfo->name = jas_strdup(name))) {
		return -1;
	}
	if (!(fmtinfo->ext = jas_strdup(ext))) {
		jas_free(fmtinfo->name);
		return -1;
	}
	if (!(fmtinfo->desc = jas_strdup(desc))) {
		jas_free(fmtinfo->name);
		jas_free(fmtinfo->ext);
		return -1;
	}
	fmtinfo->ops = *ops;
	++jas_image_numfmts;
	return 0;
}