static int fill_files_note(struct memelfnote *note)
{
	struct vm_area_struct *vma;
	unsigned count, size, names_ofs, remaining, n;
	user_long_t *data;
	user_long_t *start_end_ofs;
	char *name_base, *name_curpos;

	/* *Estimated* file count and total data size needed */
	count = current->mm->map_count;
	size = count * 64;

	names_ofs = (2 + 3 * count) * sizeof(data[0]);
 alloc:
	if (size >= MAX_FILE_NOTE_SIZE) /* paranoia check */
		return -EINVAL;
	size = round_up(size, PAGE_SIZE);
	data = vmalloc(size);
	if (!data)
		return -ENOMEM;

	start_end_ofs = data + 2;
	name_base = name_curpos = ((char *)data) + names_ofs;
	remaining = size - names_ofs;
	count = 0;
	for (vma = current->mm->mmap; vma != NULL; vma = vma->vm_next) {
		struct file *file;
		const char *filename;

		file = vma->vm_file;
		if (!file)
			continue;
		filename = file_path(file, name_curpos, remaining);
		if (IS_ERR(filename)) {
			if (PTR_ERR(filename) == -ENAMETOOLONG) {
				vfree(data);
				size = size * 5 / 4;
				goto alloc;
			}
			continue;
		}

		/* file_path() fills at the end, move name down */
		/* n = strlen(filename) + 1: */
		n = (name_curpos + remaining) - filename;
		remaining = filename - name_curpos;
		memmove(name_curpos, filename, n);
		name_curpos += n;

		*start_end_ofs++ = vma->vm_start;
		*start_end_ofs++ = vma->vm_end;
		*start_end_ofs++ = vma->vm_pgoff;
		count++;
	}

	/* Now we know exact count of files, can store it */
	data[0] = count;
	data[1] = PAGE_SIZE;
	/*
	 * Count usually is less than current->mm->map_count,
	 * we need to move filenames down.
	 */
	n = current->mm->map_count - count;
	if (n != 0) {
		unsigned shift_bytes = n * 3 * sizeof(data[0]);
		memmove(name_base - shift_bytes, name_base,
			name_curpos - name_base);
		name_curpos -= shift_bytes;
	}

	size = name_curpos - (char *)data;
	fill_note(note, "CORE", NT_FILE, size, data);
	return 0;
}