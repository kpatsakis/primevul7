static bool bpf_prog_array_copy_core(struct bpf_prog **prog,
				     u32 *prog_ids,
				     u32 request_cnt)
{
	int i = 0;

	for (; *prog; prog++) {
		if (*prog == &dummy_bpf_prog.prog)
			continue;
		prog_ids[i] = (*prog)->aux->id;
		if (++i == request_cnt) {
			prog++;
			break;
		}
	}

	return !!(*prog);
}