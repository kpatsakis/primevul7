void jas_image_delcmpt(jas_image_t *image, int cmptno)
{
	if (cmptno >= image->numcmpts_) {
		return;
	}
	jas_image_cmpt_destroy(image->cmpts_[cmptno]);
	if (cmptno < image->numcmpts_) {
		memmove(&image->cmpts_[cmptno], &image->cmpts_[cmptno + 1],
		  (image->numcmpts_ - 1 - cmptno) * sizeof(jas_image_cmpt_t *));
	}
	--image->numcmpts_;

	jas_image_setbbox(image);
}