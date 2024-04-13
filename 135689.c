static void diff_fill_sha1_info(struct diff_filespec *one)
{
	if (DIFF_FILE_VALID(one)) {
		if (!one->sha1_valid) {
			struct stat st;
			if (!strcmp(one->path, "-")) {
				hashcpy(one->sha1, null_sha1);
				return;
			}
			if (lstat(one->path, &st) < 0)
				die("stat %s", one->path);
			if (index_path(one->sha1, one->path, &st, 0))
				die("cannot hash %s\n", one->path);
		}
	}
	else
		hashclr(one->sha1);
}