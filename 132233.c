nfs_access_calc_mask(u32 access_result, umode_t umode)
{
	int mask = 0;

	if (access_result & NFS_MAY_READ)
		mask |= MAY_READ;
	if (S_ISDIR(umode)) {
		if ((access_result & NFS_DIR_MAY_WRITE) == NFS_DIR_MAY_WRITE)
			mask |= MAY_WRITE;
		if ((access_result & NFS_MAY_LOOKUP) == NFS_MAY_LOOKUP)
			mask |= MAY_EXEC;
	} else if (S_ISREG(umode)) {
		if ((access_result & NFS_FILE_MAY_WRITE) == NFS_FILE_MAY_WRITE)
			mask |= MAY_WRITE;
		if ((access_result & NFS_MAY_EXECUTE) == NFS_MAY_EXECUTE)
			mask |= MAY_EXEC;
	} else if (access_result & NFS_MAY_WRITE)
			mask |= MAY_WRITE;
	return mask;
}