int jas_image_sampcmpt(jas_image_t *image, int cmptno, int newcmptno,
  jas_image_coord_t ho, jas_image_coord_t vo, jas_image_coord_t hs,
  jas_image_coord_t vs, int sgnd, int prec)
{
	jas_image_cmpt_t *oldcmpt;
	jas_image_cmpt_t *newcmpt;
	int width;
	int height;
	jas_image_coord_t tlx;
	jas_image_coord_t tly;
	jas_image_coord_t brx;
	jas_image_coord_t bry;
	int i;
	int j;
	jas_image_cmptparm_t cmptparm;
	jas_image_coord_t ax;
	jas_image_coord_t ay;
	jas_image_coord_t bx;
	jas_image_coord_t by;
	jas_image_coord_t d0;
	jas_image_coord_t d1;
	jas_image_coord_t d2;
	jas_image_coord_t d3;
	jas_image_coord_t oldx;
	jas_image_coord_t oldy;
	jas_image_coord_t x;
	jas_image_coord_t y;
	long v;
	jas_image_coord_t cmptbrx;
	jas_image_coord_t cmptbry;

	assert(cmptno >= 0 && cmptno < image->numcmpts_);
	oldcmpt = image->cmpts_[cmptno];
	assert(oldcmpt->tlx_ == 0 && oldcmpt->tly_ == 0);
	jas_image_calcbbox2(image, &tlx, &tly, &brx, &bry);
	width = FLOORDIV(brx - ho + hs, hs);
	height = FLOORDIV(bry - vo + vs, vs);
	cmptparm.tlx = ho;
	cmptparm.tly = vo;
	cmptparm.hstep = hs;
	cmptparm.vstep = vs;
	cmptparm.width = width;
	cmptparm.height = height;
	cmptparm.prec = prec;
	cmptparm.sgnd = sgnd;
	if (jas_image_addcmpt(image, newcmptno, &cmptparm))
		goto error;
cmptbrx = oldcmpt->tlx_ + (oldcmpt->width_ - 1) * oldcmpt->hstep_;
cmptbry = oldcmpt->tly_ + (oldcmpt->height_ - 1) * oldcmpt->vstep_;
	newcmpt = image->cmpts_[newcmptno];
	jas_stream_rewind(newcmpt->stream_);
	for (i = 0; i < height; ++i) {
		y = newcmpt->tly_ + newcmpt->vstep_ * i;
		for (j = 0; j < width; ++j) {
			x = newcmpt->tlx_ + newcmpt->hstep_ * j;
			ax = downtomult(x - oldcmpt->tlx_, oldcmpt->hstep_) + oldcmpt->tlx_;
			ay = downtomult(y - oldcmpt->tly_, oldcmpt->vstep_) + oldcmpt->tly_;
			bx = uptomult(x - oldcmpt->tlx_, oldcmpt->hstep_) + oldcmpt->tlx_;
			if (bx > cmptbrx)
				bx = cmptbrx;
			by = uptomult(y - oldcmpt->tly_, oldcmpt->vstep_) + oldcmpt->tly_;
			if (by > cmptbry)
				by = cmptbry;
			d0 = (ax - x) * (ax - x) + (ay - y) * (ay - y);
			d1 = (bx - x) * (bx - x) + (ay - y) * (ay - y);
			d2 = (bx - x) * (bx - x) + (by - y) * (by - y);
			d3 = (ax - x) * (ax - x) + (by - y) * (by - y);
			if (d0 <= d1 && d0 <= d2 && d0 <= d3) {
				oldx = (ax - oldcmpt->tlx_) / oldcmpt->hstep_;
				oldy = (ay - oldcmpt->tly_) / oldcmpt->vstep_;
			} else if (d1 <= d0 && d1 <= d2 && d1 <= d3) {
				oldx = (bx - oldcmpt->tlx_) / oldcmpt->hstep_;
				oldy = (ay - oldcmpt->tly_) / oldcmpt->vstep_;
			} else if (d2 <= d0 && d2 <= d1 && d1 <= d3) {
				oldx = (bx - oldcmpt->tlx_) / oldcmpt->hstep_;
				oldy = (by - oldcmpt->tly_) / oldcmpt->vstep_;
			} else {
				oldx = (ax - oldcmpt->tlx_) / oldcmpt->hstep_;
				oldy = (by - oldcmpt->tly_) / oldcmpt->vstep_;
			}
			assert(oldx >= 0 && oldx < oldcmpt->width_ &&
			  oldy >= 0 && oldy < oldcmpt->height_);
			if (jas_stream_seek(oldcmpt->stream_, oldcmpt->cps_ *
			  (oldy * oldcmpt->width_ + oldx), SEEK_SET) < 0)
				goto error;
			if (getint(oldcmpt->stream_, oldcmpt->sgnd_,
			  oldcmpt->prec_, &v))
				goto error;
			if (newcmpt->prec_ != oldcmpt->prec_ ||
			  newcmpt->sgnd_ != oldcmpt->sgnd_) {
				v = convert(v, oldcmpt->sgnd_, oldcmpt->prec_,
				  newcmpt->sgnd_, newcmpt->prec_);
			}
			if (putint(newcmpt->stream_, newcmpt->sgnd_,
			  newcmpt->prec_, v))
				goto error;
		}
	}
	return 0;
error:
	return -1;
}