static void selinux_file_free_security(struct file *file)
{
	file_free_security(file);
}