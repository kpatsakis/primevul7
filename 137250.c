struct bpf_prog *bpf_prog_select_runtime(struct bpf_prog *fp, int *err)
{
#ifndef CONFIG_BPF_JIT_ALWAYS_ON
	u32 stack_depth = max_t(u32, fp->aux->stack_depth, 1);

	fp->bpf_func = interpreters[(round_up(stack_depth, 32) / 32) - 1];
#else
	fp->bpf_func = __bpf_prog_ret0_warn;
#endif

	/* eBPF JITs can rewrite the program in case constant
	 * blinding is active. However, in case of error during
	 * blinding, bpf_int_jit_compile() must always return a
	 * valid program, which in this case would simply not
	 * be JITed, but falls back to the interpreter.
	 */
	if (!bpf_prog_is_dev_bound(fp->aux)) {
		fp = bpf_int_jit_compile(fp);
#ifdef CONFIG_BPF_JIT_ALWAYS_ON
		if (!fp->jited) {
			*err = -ENOTSUPP;
			return fp;
		}
#endif
	} else {
		*err = bpf_prog_offload_compile(fp);
		if (*err)
			return fp;
	}
	bpf_prog_lock_ro(fp);

	/* The tail call compatibility check can only be done at
	 * this late stage as we need to determine, if we deal
	 * with JITed or non JITed program concatenations and not
	 * all eBPF JITs might immediately support all features.
	 */
	*err = bpf_check_tail_call(fp);

	return fp;
}