int jas_image_readcmptsample(jas_image_t *image, int cmptno, int x, int y)
{
	jas_image_cmpt_t *cmpt;
	uint_fast32_t v;
	int k;
	int c;

	cmpt = image->cmpts_[cmptno];

	if (jas_stream_seek(cmpt->stream_, (cmpt->width_ * y + x) * cmpt->cps_,
	  SEEK_SET) < 0) {
		return -1;
	}
	v = 0;
	for (k = cmpt->cps_; k > 0; --k) {
		if ((c = jas_stream_getc(cmpt->stream_)) == EOF) {
			return -1;
		}
		v = (v << 8) | (c & 0xff);
	}
	return bitstoint(v, cmpt->prec_, cmpt->sgnd_);
}