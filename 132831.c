const char *ipmi_addr_src_to_str(enum ipmi_addr_src src)
{
	if (src >= SI_LAST)
		src = 0; /* Invalid */
	return addr_src_to_str[src];
}