filename_extension(char *path, int is_url)
{
    char *last_dot = "", *p = path;
    int i;

    if (path == NULL)
	return last_dot;
    if (*p == '.')
	p++;
    for (; *p; p++) {
	if (*p == '.') {
	    last_dot = p;
	}
	else if (is_url && *p == '?')
	    break;
    }
    if (*last_dot == '.') {
	for (i = 1; i < 8 && last_dot[i]; i++) {
	    if (is_url && !IS_ALNUM(last_dot[i]))
		break;
	}
	return allocStr(last_dot, i);
    }
    else
	return last_dot;
}