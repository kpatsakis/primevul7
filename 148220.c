domainname_has_safe_characters(char* s, size_t slen) {
	size_t i;
	for(i = 0; i < slen; i++) {
		if(s[i] == '\0') return 1;
		if((s[i] == '-' && i != 0)
			|| (s[i] == '.' && (i != 0 || s[1] == '\0'))
			|| (s[i] == '_') || (s[i] >= '0' && s[i] <= '9')
			|| (s[i] >= 'A' && s[i] <= 'Z')
			|| (s[i] >= 'a' && s[i] <= 'z')) {
			continue;
		}
		return 0;
	}
	return 1;
}