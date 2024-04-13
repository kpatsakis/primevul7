static mode_t create_default_mode(files_struct *fsp, bool interitable_mode)
{
	int snum = SNUM(fsp->conn);
	mode_t and_bits = (mode_t)0;
	mode_t or_bits = (mode_t)0;
	mode_t mode = interitable_mode
		? unix_mode( fsp->conn, FILE_ATTRIBUTE_ARCHIVE, fsp->fsp_name,
			     NULL )
		: S_IRUSR;

	if (fsp->is_directory)
		mode |= (S_IWUSR|S_IXUSR);

	/*
	 * Now AND with the create mode/directory mode bits then OR with the
	 * force create mode/force directory mode bits.
	 */

	if (fsp->is_directory) {
		and_bits = lp_dir_security_mask(snum);
		or_bits = lp_force_dir_security_mode(snum);
	} else {
		and_bits = lp_security_mask(snum);
		or_bits = lp_force_security_mode(snum);
	}

	return ((mode & and_bits)|or_bits);
}