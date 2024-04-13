uint_fast32_t jas_image_rawsize(jas_image_t *image)
{
	uint_fast32_t rawsize;
	int cmptno;
	jas_image_cmpt_t *cmpt;

	rawsize = 0;
	for (cmptno = 0; cmptno < image->numcmpts_; ++cmptno) {
		cmpt = image->cmpts_[cmptno];
		rawsize += (cmpt->width_ * cmpt->height_ * cmpt->prec_ +
		  7) / 8;
	}
	return rawsize;
}