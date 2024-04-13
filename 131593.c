static bool convert_canon_ace_to_posix_perms( files_struct *fsp, canon_ace *file_ace_list, mode_t *posix_perms)
{
	int snum = SNUM(fsp->conn);
	size_t ace_count = count_canon_ace_list(file_ace_list);
	canon_ace *ace_p;
	canon_ace *owner_ace = NULL;
	canon_ace *group_ace = NULL;
	canon_ace *other_ace = NULL;
	mode_t and_bits;
	mode_t or_bits;

	if (ace_count != 3) {
		DEBUG(3,("convert_canon_ace_to_posix_perms: Too many ACE entries for file %s to convert to \
posix perms.\n", fsp->fsp_name ));
		return False;
	}

	for (ace_p = file_ace_list; ace_p; ace_p = ace_p->next) {
		if (ace_p->owner_type == UID_ACE)
			owner_ace = ace_p;
		else if (ace_p->owner_type == GID_ACE)
			group_ace = ace_p;
		else if (ace_p->owner_type == WORLD_ACE)
			other_ace = ace_p;
	}

	if (!owner_ace || !group_ace || !other_ace) {
		DEBUG(3,("convert_canon_ace_to_posix_perms: Can't get standard entries for file %s.\n",
				fsp->fsp_name ));
		return False;
	}

	*posix_perms = (mode_t)0;

	*posix_perms |= owner_ace->perms;
	*posix_perms |= MAP_PERM(group_ace->perms, S_IRUSR, S_IRGRP);
	*posix_perms |= MAP_PERM(group_ace->perms, S_IWUSR, S_IWGRP);
	*posix_perms |= MAP_PERM(group_ace->perms, S_IXUSR, S_IXGRP);
	*posix_perms |= MAP_PERM(other_ace->perms, S_IRUSR, S_IROTH);
	*posix_perms |= MAP_PERM(other_ace->perms, S_IWUSR, S_IWOTH);
	*posix_perms |= MAP_PERM(other_ace->perms, S_IXUSR, S_IXOTH);

	/* The owner must have at least read access. */

	*posix_perms |= S_IRUSR;
	if (fsp->is_directory)
		*posix_perms |= (S_IWUSR|S_IXUSR);

	/* If requested apply the masks. */

	/* Get the initial bits to apply. */

	if (fsp->is_directory) {
		and_bits = lp_dir_security_mask(snum);
		or_bits = lp_force_dir_security_mode(snum);
	} else {
		and_bits = lp_security_mask(snum);
		or_bits = lp_force_security_mode(snum);
	}

	*posix_perms = (((*posix_perms) & and_bits)|or_bits);

	DEBUG(10,("convert_canon_ace_to_posix_perms: converted u=%o,g=%o,w=%o to perm=0%o for file %s.\n",
		(int)owner_ace->perms, (int)group_ace->perms, (int)other_ace->perms, (int)*posix_perms,
		fsp->fsp_name ));

	return True;
}