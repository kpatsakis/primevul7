static void apply_default_perms(const struct share_params *params,
				const bool is_directory, canon_ace *pace,
				mode_t type)
{
	mode_t and_bits = (mode_t)0;
	mode_t or_bits = (mode_t)0;

	/* Get the initial bits to apply. */

	if (is_directory) {
		and_bits = lp_dir_security_mask(params->service);
		or_bits = lp_force_dir_security_mode(params->service);
	} else {
		and_bits = lp_security_mask(params->service);
		or_bits = lp_force_security_mode(params->service);
	}

	/* Now bounce them into the S_USR space. */	
	switch(type) {
	case S_IRUSR:
		/* Ensure owner has read access. */
		pace->perms |= S_IRUSR;
		if (is_directory)
			pace->perms |= (S_IWUSR|S_IXUSR);
		and_bits = unix_perms_to_acl_perms(and_bits, S_IRUSR, S_IWUSR, S_IXUSR);
		or_bits = unix_perms_to_acl_perms(or_bits, S_IRUSR, S_IWUSR, S_IXUSR);
		break;
	case S_IRGRP:
		and_bits = unix_perms_to_acl_perms(and_bits, S_IRGRP, S_IWGRP, S_IXGRP);
		or_bits = unix_perms_to_acl_perms(or_bits, S_IRGRP, S_IWGRP, S_IXGRP);
		break;
	case S_IROTH:
		and_bits = unix_perms_to_acl_perms(and_bits, S_IROTH, S_IWOTH, S_IXOTH);
		or_bits = unix_perms_to_acl_perms(or_bits, S_IROTH, S_IWOTH, S_IXOTH);
		break;
	}

	pace->perms = ((pace->perms & and_bits)|or_bits);
}