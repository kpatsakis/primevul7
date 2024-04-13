static inline u32 open_file_to_av(struct file *file)
{
	u32 av = file_to_av(file);

	if (selinux_policycap_openperm)
		av |= FILE__OPEN;

	return av;
}