static jas_image_cmpt_t *jas_image_cmpt_create0()
{
	jas_image_cmpt_t *cmpt;
	if (!(cmpt = jas_malloc(sizeof(jas_image_cmpt_t)))) {
		return 0;
	}
	memset(cmpt, 0, sizeof(jas_image_cmpt_t));
	cmpt->type_ = JAS_IMAGE_CT_UNKNOWN;
	return cmpt;
}