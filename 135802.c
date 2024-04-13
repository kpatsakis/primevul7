void fill_filespec(struct diff_filespec *spec, const unsigned char *sha1,
		   unsigned short mode)
{
	if (mode) {
		spec->mode = canon_mode(mode);
		hashcpy(spec->sha1, sha1);
		spec->sha1_valid = !is_null_sha1(sha1);
	}
}