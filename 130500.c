append_acs0(string_desc * dst, int code, char *src, size_t off)
{
    if (src != 0 && off < strlen(src)) {
	char temp[3];
	temp[0] = (char) code;
	temp[1] = src[off];
	temp[2] = 0;
	_nc_safe_strcat(dst, temp);
    }
}