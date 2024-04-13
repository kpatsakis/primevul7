ldns_tolower_str(char* s)
{
	if(s) {
		while(*s) {
			*s = (char)tolower((unsigned char)*s);
			s++;
		}
	}
}