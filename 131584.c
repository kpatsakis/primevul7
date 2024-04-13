static bool nt4_compatible_acls(void)
{
	int compat = lp_acl_compatibility();

	if (compat == ACL_COMPAT_AUTO) {
		enum remote_arch_types ra_type = get_remote_arch();

		/* Automatically adapt to client */
		return (ra_type <= RA_WINNT);
	} else
		return (compat == ACL_COMPAT_WINNT);
}