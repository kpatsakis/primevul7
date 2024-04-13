int jas_image_ishomosamp(jas_image_t *image)
{
	jas_image_coord_t hstep;
	jas_image_coord_t vstep;
	int result;
	int i;
	hstep = jas_image_cmpthstep(image, 0);
	vstep = jas_image_cmptvstep(image, 0);
	result = 1;
	for (i = 0; i < image->numcmpts_; ++i) {
		if (jas_image_cmpthstep(image, i) != hstep ||
		  jas_image_cmptvstep(image, i) != vstep) {
			result = 0;
			break;
		}
	}
	return result;
}