int bpf_prog_array_copy(struct bpf_prog_array __rcu *old_array,
			struct bpf_prog *exclude_prog,
			struct bpf_prog *include_prog,
			struct bpf_prog_array **new_array)
{
	int new_prog_cnt, carry_prog_cnt = 0;
	struct bpf_prog **existing_prog;
	struct bpf_prog_array *array;
	int new_prog_idx = 0;

	/* Figure out how many existing progs we need to carry over to
	 * the new array.
	 */
	if (old_array) {
		existing_prog = old_array->progs;
		for (; *existing_prog; existing_prog++) {
			if (*existing_prog != exclude_prog &&
			    *existing_prog != &dummy_bpf_prog.prog)
				carry_prog_cnt++;
			if (*existing_prog == include_prog)
				return -EEXIST;
		}
	}

	/* How many progs (not NULL) will be in the new array? */
	new_prog_cnt = carry_prog_cnt;
	if (include_prog)
		new_prog_cnt += 1;

	/* Do we have any prog (not NULL) in the new array? */
	if (!new_prog_cnt) {
		*new_array = NULL;
		return 0;
	}

	/* +1 as the end of prog_array is marked with NULL */
	array = bpf_prog_array_alloc(new_prog_cnt + 1, GFP_KERNEL);
	if (!array)
		return -ENOMEM;

	/* Fill in the new prog array */
	if (carry_prog_cnt) {
		existing_prog = old_array->progs;
		for (; *existing_prog; existing_prog++)
			if (*existing_prog != exclude_prog &&
			    *existing_prog != &dummy_bpf_prog.prog)
				array->progs[new_prog_idx++] = *existing_prog;
	}
	if (include_prog)
		array->progs[new_prog_idx++] = include_prog;
	array->progs[new_prog_idx] = NULL;
	*new_array = array;
	return 0;
}