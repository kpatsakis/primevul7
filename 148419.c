sldns_is_last_of_string(const char* str)
{
	if(*str == 0) return 1;
	while(isspace((unsigned char)*str))
		str++;
	if(*str == 0) return 1;
	return 0;
}