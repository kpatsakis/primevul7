static int nsim_bpf_string_show(struct seq_file *file, void *data)
{
	const char **str = file->private;

	if (*str)
		seq_printf(file, "%s\n", *str);

	return 0;
}