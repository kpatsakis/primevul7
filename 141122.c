static int selinux_file_alloc_security(struct file *file)
{
	return file_alloc_security(file);
}