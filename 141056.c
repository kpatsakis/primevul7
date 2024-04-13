static inline int selinux_option(char *option, int len)
{
	return (match_prefix(CONTEXT_STR, sizeof(CONTEXT_STR)-1, option, len) ||
		match_prefix(FSCONTEXT_STR, sizeof(FSCONTEXT_STR)-1, option, len) ||
		match_prefix(DEFCONTEXT_STR, sizeof(DEFCONTEXT_STR)-1, option, len) ||
		match_prefix(ROOTCONTEXT_STR, sizeof(ROOTCONTEXT_STR)-1, option, len) ||
		match_prefix(LABELSUPP_STR, sizeof(LABELSUPP_STR)-1, option, len));
}