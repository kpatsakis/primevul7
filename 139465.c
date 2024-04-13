void jas_image_destroy(jas_image_t *image)
{
	int i;

	if (image->cmpts_) {
		for (i = 0; i < image->numcmpts_; ++i) {
			jas_image_cmpt_destroy(image->cmpts_[i]);
			image->cmpts_[i] = 0;
		}
		jas_free(image->cmpts_);
	}
	if (image->cmprof_)
		jas_cmprof_destroy(image->cmprof_);
	jas_free(image);
}