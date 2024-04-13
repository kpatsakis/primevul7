struct bpf_prog *bpf_prog_realloc(struct bpf_prog *fp_old, unsigned int size,
				  gfp_t gfp_extra_flags)
{
	gfp_t gfp_flags = GFP_KERNEL | __GFP_ZERO | gfp_extra_flags;
	struct bpf_prog *fp;
	u32 pages, delta;
	int ret;

	BUG_ON(fp_old == NULL);

	size = round_up(size, PAGE_SIZE);
	pages = size / PAGE_SIZE;
	if (pages <= fp_old->pages)
		return fp_old;

	delta = pages - fp_old->pages;
	ret = __bpf_prog_charge(fp_old->aux->user, delta);
	if (ret)
		return NULL;

	fp = __vmalloc(size, gfp_flags, PAGE_KERNEL);
	if (fp == NULL) {
		__bpf_prog_uncharge(fp_old->aux->user, delta);
	} else {
		memcpy(fp, fp_old, fp_old->pages * PAGE_SIZE);
		fp->pages = pages;
		fp->aux->prog = fp;

		/* We keep fp->aux from fp_old around in the new
		 * reallocated structure.
		 */
		fp_old->aux = NULL;
		__bpf_prog_free(fp_old);
	}

	return fp;
}