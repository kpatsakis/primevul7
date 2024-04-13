static int copy_insn(struct uprobe *uprobe, struct file *filp)
{
	struct address_space *mapping = uprobe->inode->i_mapping;
	loff_t offs = uprobe->offset;
	void *insn = &uprobe->arch.insn;
	int size = sizeof(uprobe->arch.insn);
	int len, err = -EIO;

	/* Copy only available bytes, -EIO if nothing was read */
	do {
		if (offs >= i_size_read(uprobe->inode))
			break;

		len = min_t(int, size, PAGE_SIZE - (offs & ~PAGE_MASK));
		err = __copy_insn(mapping, filp, insn, len, offs);
		if (err)
			break;

		insn += len;
		offs += len;
		size -= len;
	} while (size);

	return err;
}