static void prep_temp_blob(struct diff_tempfile *temp,
			   void *blob,
			   unsigned long size,
			   const unsigned char *sha1,
			   int mode)
{
	int fd;

	fd = git_mkstemp(temp->tmp_path, PATH_MAX, ".diff_XXXXXX");
	if (fd < 0)
		die("unable to create temp-file: %s", strerror(errno));
	if (write_in_full(fd, blob, size) != size)
		die("unable to write temp-file");
	close(fd);
	temp->name = temp->tmp_path;
	strcpy(temp->hex, sha1_to_hex(sha1));
	temp->hex[40] = 0;
	sprintf(temp->mode, "%06o", mode);
}