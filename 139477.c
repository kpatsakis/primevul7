int jas_image_getcmptbytype(jas_image_t *image, int ctype)
{
	int cmptno;

	for (cmptno = 0; cmptno < image->numcmpts_; ++cmptno) {
		if (image->cmpts_[cmptno]->type_ == ctype) {
			return cmptno;
		}
	}
	return -1;
}