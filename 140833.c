static int load_elf_library(struct file *file)
{
	struct elf_phdr *elf_phdata;
	struct elf_phdr *eppnt;
	unsigned long elf_bss, bss, len;
	int retval, error, i, j;
	struct elfhdr elf_ex;

	error = -ENOEXEC;
	retval = kernel_read(file, 0, (char *)&elf_ex, sizeof(elf_ex));
	if (retval != sizeof(elf_ex))
		goto out;

	if (memcmp(elf_ex.e_ident, ELFMAG, SELFMAG) != 0)
		goto out;

	/* First of all, some simple consistency checks */
	if (elf_ex.e_type != ET_EXEC || elf_ex.e_phnum > 2 ||
	    !elf_check_arch(&elf_ex) || !file->f_op->mmap)
		goto out;

	/* Now read in all of the header information */

	j = sizeof(struct elf_phdr) * elf_ex.e_phnum;
	/* j < ELF_MIN_ALIGN because elf_ex.e_phnum <= 2 */

	error = -ENOMEM;
	elf_phdata = kmalloc(j, GFP_KERNEL);
	if (!elf_phdata)
		goto out;

	eppnt = elf_phdata;
	error = -ENOEXEC;
	retval = kernel_read(file, elf_ex.e_phoff, (char *)eppnt, j);
	if (retval != j)
		goto out_free_ph;

	for (j = 0, i = 0; i<elf_ex.e_phnum; i++)
		if ((eppnt + i)->p_type == PT_LOAD)
			j++;
	if (j != 1)
		goto out_free_ph;

	while (eppnt->p_type != PT_LOAD)
		eppnt++;

	/* Now use mmap to map the library into memory. */
	error = vm_mmap(file,
			ELF_PAGESTART(eppnt->p_vaddr),
			(eppnt->p_filesz +
			 ELF_PAGEOFFSET(eppnt->p_vaddr)),
			PROT_READ | PROT_WRITE | PROT_EXEC,
			MAP_FIXED | MAP_PRIVATE | MAP_DENYWRITE,
			(eppnt->p_offset -
			 ELF_PAGEOFFSET(eppnt->p_vaddr)));
	if (error != ELF_PAGESTART(eppnt->p_vaddr))
		goto out_free_ph;

	elf_bss = eppnt->p_vaddr + eppnt->p_filesz;
	if (padzero(elf_bss)) {
		error = -EFAULT;
		goto out_free_ph;
	}

	len = ELF_PAGESTART(eppnt->p_filesz + eppnt->p_vaddr +
			    ELF_MIN_ALIGN - 1);
	bss = eppnt->p_memsz + eppnt->p_vaddr;
	if (bss > len) {
		error = vm_brk(len, bss - len);
		if (error)
			goto out_free_ph;
	}
	error = 0;

out_free_ph:
	kfree(elf_phdata);
out:
	return error;
}