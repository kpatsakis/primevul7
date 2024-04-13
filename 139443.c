static void jas_image_setbbox(jas_image_t *image)
{
	jas_image_cmpt_t *cmpt;
	int cmptno;
	int_fast32_t x;
	int_fast32_t y;

	if (image->numcmpts_ > 0) {
		/* Determine the bounding box for all of the components on the
		  reference grid (i.e., the image area) */
		cmpt = image->cmpts_[0];
		image->tlx_ = cmpt->tlx_;
		image->tly_ = cmpt->tly_;
		image->brx_ = cmpt->tlx_ + cmpt->hstep_ * (cmpt->width_ - 1) + 1;
		image->bry_ = cmpt->tly_ + cmpt->vstep_ * (cmpt->height_ - 1) + 1;
		for (cmptno = 1; cmptno < image->numcmpts_; ++cmptno) {
			cmpt = image->cmpts_[cmptno];
			if (image->tlx_ > cmpt->tlx_) {
				image->tlx_ = cmpt->tlx_;
			}
			if (image->tly_ > cmpt->tly_) {
				image->tly_ = cmpt->tly_;
			}
			x = cmpt->tlx_ + cmpt->hstep_ * (cmpt->width_ - 1) + 1;
			if (image->brx_ < x) {
				image->brx_ = x;
			}
			y = cmpt->tly_ + cmpt->vstep_ * (cmpt->height_ - 1) + 1;
			if (image->bry_ < y) {
				image->bry_ = y;
			}
		}
	} else {
		image->tlx_ = 0;
		image->tly_ = 0;
		image->brx_ = 0;
		image->bry_ = 0;
	}
}