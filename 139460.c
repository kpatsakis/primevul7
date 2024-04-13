void jas_image_writecmptsample(jas_image_t *image, int cmptno, int x, int y,
  int_fast32_t v)
{
	jas_image_cmpt_t *cmpt;
	uint_fast32_t t;
	int k;
	int c;

	cmpt = image->cmpts_[cmptno];

	if (jas_stream_seek(cmpt->stream_, (cmpt->width_ * y + x) * cmpt->cps_,
	  SEEK_SET) < 0) {
		return;
	}
	t = inttobits(v, cmpt->prec_, cmpt->sgnd_);
	for (k = cmpt->cps_; k > 0; --k) {
		c = (t >> (8 * (cmpt->cps_ - 1))) & 0xff;
		if (jas_stream_putc(cmpt->stream_, (unsigned char) c) == EOF) {
			return;
		}
		t <<= 8;
	}
}