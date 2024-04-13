int jas_image_writecmpt2(jas_image_t *image, int cmptno, jas_image_coord_t x,
  jas_image_coord_t y, jas_image_coord_t width, jas_image_coord_t height,
  long *buf)
{
	jas_image_cmpt_t *cmpt;
	jas_image_coord_t i;
	jas_image_coord_t j;
	long v;
	long *bufptr;

	if (cmptno < 0 || cmptno >= image->numcmpts_)
		goto error;
	cmpt = image->cmpts_[cmptno];
	if (x < 0 || x >= cmpt->width_ || y < 0 || y >= cmpt->height_ ||
	  width < 0 || height < 0 || x + width > cmpt->width_ ||
	  y + height > cmpt->height_)
		goto error;

	bufptr = buf;
	for (i = 0; i < height; ++i) {
		if (jas_stream_seek(cmpt->stream_, (cmpt->width_ * (y + i) + x)
		  * cmpt->cps_, SEEK_SET) < 0)
			goto error;
		for (j = 0; j < width; ++j) {
			v = *bufptr++;
			if (putint(cmpt->stream_, cmpt->sgnd_, cmpt->prec_, v))
				goto error;
		}
	}

	return 0;
error:
	return -1;
}