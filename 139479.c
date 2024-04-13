static void jas_image_calcbbox2(jas_image_t *image, jas_image_coord_t *tlx,
  jas_image_coord_t *tly, jas_image_coord_t *brx, jas_image_coord_t *bry)
{
	jas_image_cmpt_t *cmpt;
	jas_image_coord_t tmptlx;
	jas_image_coord_t tmptly;
	jas_image_coord_t tmpbrx;
	jas_image_coord_t tmpbry;
	jas_image_coord_t t;
	int i;
	if (image->numcmpts_ > 0) {
		cmpt = image->cmpts_[0];
		tmptlx = cmpt->tlx_;
		tmptly = cmpt->tly_;
		tmpbrx = cmpt->tlx_ + cmpt->hstep_ * (cmpt->width_ - 1);
		tmpbry = cmpt->tly_ + cmpt->vstep_ * (cmpt->height_ - 1);
		for (i = 0; i < image->numcmpts_; ++i) {
			cmpt = image->cmpts_[i];
			if (cmpt->tlx_ < tmptlx)
				tmptlx = cmpt->tlx_;
			if (cmpt->tly_ < tmptly)
				tmptly = cmpt->tly_;
			t = cmpt->tlx_ + cmpt->hstep_ * (cmpt->width_ - 1);
			if (t > tmpbrx)
				tmpbrx = t;
			t = cmpt->tly_ + cmpt->vstep_ * (cmpt->height_ - 1);
			if (t > tmpbry)
				tmpbry = t;
		}
	} else {
		tmptlx = 0;
		tmptly = 0;
		tmpbrx = -1;
		tmpbry = -1;
	}
	*tlx = tmptlx;
	*tly = tmptly;
	*brx = tmpbrx;
	*bry = tmpbry;
}