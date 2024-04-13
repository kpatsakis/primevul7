static bool cname_self_reference(const char* node_name,
				 const char* zone_name,
				 struct DNS_RPC_NAME name) {
	size_t node_len, zone_len;

	if (node_name == NULL || zone_name == NULL) {
		return false;
	}

	node_len = strlen(node_name);
	zone_len = strlen(zone_name);

	if (node_len == 0 ||
	    zone_len == 0 ||
	    (name.len != node_len + zone_len + 1)) {
		return false;
	}

	if (strncmp(node_name, name.str, node_len) == 0 &&
	    name.str[node_len] == '.' &&
	    strncmp(zone_name, name.str + node_len + 1, zone_len) == 0) {
		return true;
	}

	return false;
}