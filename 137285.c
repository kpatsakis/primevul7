int bpf_prog_calc_tag(struct bpf_prog *fp)
{
	const u32 bits_offset = SHA_MESSAGE_BYTES - sizeof(__be64);
	u32 raw_size = bpf_prog_tag_scratch_size(fp);
	u32 digest[SHA_DIGEST_WORDS];
	u32 ws[SHA_WORKSPACE_WORDS];
	u32 i, bsize, psize, blocks;
	struct bpf_insn *dst;
	bool was_ld_map;
	u8 *raw, *todo;
	__be32 *result;
	__be64 *bits;

	raw = vmalloc(raw_size);
	if (!raw)
		return -ENOMEM;

	sha_init(digest);
	memset(ws, 0, sizeof(ws));

	/* We need to take out the map fd for the digest calculation
	 * since they are unstable from user space side.
	 */
	dst = (void *)raw;
	for (i = 0, was_ld_map = false; i < fp->len; i++) {
		dst[i] = fp->insnsi[i];
		if (!was_ld_map &&
		    dst[i].code == (BPF_LD | BPF_IMM | BPF_DW) &&
		    dst[i].src_reg == BPF_PSEUDO_MAP_FD) {
			was_ld_map = true;
			dst[i].imm = 0;
		} else if (was_ld_map &&
			   dst[i].code == 0 &&
			   dst[i].dst_reg == 0 &&
			   dst[i].src_reg == 0 &&
			   dst[i].off == 0) {
			was_ld_map = false;
			dst[i].imm = 0;
		} else {
			was_ld_map = false;
		}
	}

	psize = bpf_prog_insn_size(fp);
	memset(&raw[psize], 0, raw_size - psize);
	raw[psize++] = 0x80;

	bsize  = round_up(psize, SHA_MESSAGE_BYTES);
	blocks = bsize / SHA_MESSAGE_BYTES;
	todo   = raw;
	if (bsize - psize >= sizeof(__be64)) {
		bits = (__be64 *)(todo + bsize - sizeof(__be64));
	} else {
		bits = (__be64 *)(todo + bsize + bits_offset);
		blocks++;
	}
	*bits = cpu_to_be64((psize - 1) << 3);

	while (blocks--) {
		sha_transform(digest, todo, ws);
		todo += SHA_MESSAGE_BYTES;
	}

	result = (__force __be32 *)digest;
	for (i = 0; i < SHA_DIGEST_WORDS; i++)
		result[i] = cpu_to_be32(digest[i]);
	memcpy(fp->tag, result, sizeof(fp->tag));

	vfree(raw);
	return 0;
}