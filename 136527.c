int phar_is_tar(char *buf, char *fname) /* {{{ */
{
	tar_header *header = (tar_header *) buf;
	php_uint32 checksum = phar_tar_number(header->checksum, sizeof(header->checksum));
	php_uint32 ret;
	char save[sizeof(header->checksum)];

	/* assume that the first filename in a tar won't begin with <?php */
	if (!strncmp(buf, "<?php", sizeof("<?php")-1)) {
		return 0;
	}

	memcpy(save, header->checksum, sizeof(header->checksum));
	memset(header->checksum, ' ', sizeof(header->checksum));
	ret = (checksum == phar_tar_checksum(buf, 512));
	memcpy(header->checksum, save, sizeof(header->checksum));
	if (!ret && strstr(fname, ".tar")) {
		/* probably a corrupted tar - so we will pretend it is one */
		return 1;
	}
	return ret;
}