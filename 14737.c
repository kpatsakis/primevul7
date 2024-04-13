get_sourced_lnum(
	char_u *(*fgetline)(int, void *, int, getline_opt_T),
	void *cookie)
{
    return fgetline == getsourceline
			? ((source_cookie_T *)cookie)->sourcing_lnum
			: SOURCING_LNUM;
}