int jas_image_depalettize(jas_image_t *image, int cmptno, int numlutents,
  int_fast32_t *lutents, int dtype, int newcmptno)
{
	jas_image_cmptparm_t cmptparms;
	int_fast32_t v;
	int i;
	int j;
	jas_image_cmpt_t *cmpt;

	cmpt = image->cmpts_[cmptno];
	cmptparms.tlx = cmpt->tlx_;
	cmptparms.tly = cmpt->tly_;
	cmptparms.hstep = cmpt->hstep_;
	cmptparms.vstep = cmpt->vstep_;
	cmptparms.width = cmpt->width_;
	cmptparms.height = cmpt->height_;
	cmptparms.prec = JAS_IMAGE_CDT_GETPREC(dtype);
	cmptparms.sgnd = JAS_IMAGE_CDT_GETSGND(dtype);

	if (jas_image_addcmpt(image, newcmptno, &cmptparms)) {
		return -1;
	}
	if (newcmptno <= cmptno) {
		++cmptno;
		cmpt = image->cmpts_[cmptno];
	}

	for (j = 0; j < cmpt->height_; ++j) {
		for (i = 0; i < cmpt->width_; ++i) {
			v = jas_image_readcmptsample(image, cmptno, i, j);
			if (v < 0) {
				v = 0;
			} else if (v >= numlutents) {
				v = numlutents - 1;
			}
			jas_image_writecmptsample(image, newcmptno, i, j,
			  lutents[v]);
		}
	}
	return 0;
}