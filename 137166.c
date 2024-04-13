bool bpf_prog_array_compatible(struct bpf_array *array,
			       const struct bpf_prog *fp)
{
	if (fp->kprobe_override)
		return false;

	if (!array->owner_prog_type) {
		/* There's no owner yet where we could check for
		 * compatibility.
		 */
		array->owner_prog_type = fp->type;
		array->owner_jited = fp->jited;

		return true;
	}

	return array->owner_prog_type == fp->type &&
	       array->owner_jited == fp->jited;
}