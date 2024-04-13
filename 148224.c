ipseckey_has_safe_characters(char* s, size_t slen) {
	int precedence, gateway_type, algorithm;
	char* gateway;
	gateway = (char*)calloc(slen, sizeof(char));
	if(!gateway) {
		log_err("ipsecmod: out of memory when calling the hook");
		return 0;
	}
	if(sscanf(s, "%d %d %d %s ",
			&precedence, &gateway_type, &algorithm, gateway) != 4) {
		free(gateway);
		return 0;
	}
	if(gateway_type != 3) {
		free(gateway);
		return 1;
	}
	if(domainname_has_safe_characters(gateway, slen)) {
		free(gateway);
		return 1;
	}
	free(gateway);
	return 0;
}