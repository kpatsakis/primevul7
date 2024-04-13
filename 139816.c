static int verify_opcode(struct page *page, unsigned long vaddr, uprobe_opcode_t *new_opcode)
{
	uprobe_opcode_t old_opcode;
	bool is_swbp;

	/*
	 * Note: We only check if the old_opcode is UPROBE_SWBP_INSN here.
	 * We do not check if it is any other 'trap variant' which could
	 * be conditional trap instruction such as the one powerpc supports.
	 *
	 * The logic is that we do not care if the underlying instruction
	 * is a trap variant; uprobes always wins over any other (gdb)
	 * breakpoint.
	 */
	copy_from_page(page, vaddr, &old_opcode, UPROBE_SWBP_INSN_SIZE);
	is_swbp = is_swbp_insn(&old_opcode);

	if (is_swbp_insn(new_opcode)) {
		if (is_swbp)		/* register: already installed? */
			return 0;
	} else {
		if (!is_swbp)		/* unregister: was it changed by us? */
			return 0;
	}

	return 1;
}