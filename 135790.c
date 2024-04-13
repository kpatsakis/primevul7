static void strip_prefix(int prefix_length, const char **namep, const char **otherp)
{
	/* Strip the prefix but do not molest /dev/null and absolute paths */
	if (*namep && **namep != '/')
		*namep += prefix_length;
	if (*otherp && **otherp != '/')
		*otherp += prefix_length;
}