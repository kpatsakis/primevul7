static WERROR dnsserver_enumerate_root_records(struct dnsserver_state *dsstate,
					TALLOC_CTX *mem_ctx,
					unsigned int client_version,
					const char *node_name,
					enum dns_record_type record_type,
					unsigned int select_flag,
					unsigned int *buffer_length,
					struct DNS_RPC_RECORDS_ARRAY **buffer)
{
	TALLOC_CTX *tmp_ctx;
	struct dnsserver_zone *z;
	const char * const attrs[] = { "name", "dnsRecord", NULL };
	struct ldb_result *res;
	struct DNS_RPC_RECORDS_ARRAY *recs;
	char **add_names;
	char *rname;
	int add_count;
	int i, ret, len;
	WERROR status;

	tmp_ctx = talloc_new(mem_ctx);
	W_ERROR_HAVE_NO_MEMORY(tmp_ctx);

	z = dnsserver_find_zone(dsstate->zones, ".");
	if (z == NULL) {
		return WERR_DNS_ERROR_NAME_DOES_NOT_EXIST;
	}

	ret = ldb_search(dsstate->samdb, tmp_ctx, &res, z->zone_dn,
			 LDB_SCOPE_ONELEVEL, attrs,
			 "(&(objectClass=dnsNode)(name=@)(!(dNSTombstoned=TRUE)))");
	if (ret != LDB_SUCCESS) {
		talloc_free(tmp_ctx);
		return WERR_INTERNAL_DB_ERROR;
	}
	if (res->count == 0) {
		talloc_free(tmp_ctx);
		return WERR_DNS_ERROR_NAME_DOES_NOT_EXIST;
	}

	recs = talloc_zero(mem_ctx, struct DNS_RPC_RECORDS_ARRAY);
	W_ERROR_HAVE_NO_MEMORY_AND_FREE(recs, tmp_ctx);

	add_names = NULL;
	add_count = 0;

	for (i=0; i<res->count; i++) {
		status = dns_fill_records_array(tmp_ctx, NULL, record_type,
						select_flag, NULL,
						res->msgs[i], 0, recs,
						&add_names, &add_count);
		if (!W_ERROR_IS_OK(status)) {
			talloc_free(tmp_ctx);
			return status;
		}
	}
	talloc_free(res);

	/* Add any additional records */
	if (select_flag & DNS_RPC_VIEW_ADDITIONAL_DATA) {
		for (i=0; i<add_count; i++) {
			char *encoded_name
				= ldb_binary_encode_string(tmp_ctx,
							   add_names[i]);
			ret = ldb_search(dsstate->samdb, tmp_ctx, &res, z->zone_dn,
					 LDB_SCOPE_ONELEVEL, attrs,
					 "(&(objectClass=dnsNode)(name=%s)(!(dNSTombstoned=TRUE)))",
					 encoded_name);
			if (ret != LDB_SUCCESS || res->count == 0) {
				talloc_free(res);
				continue;
			}

			len = strlen(add_names[i]);
			if (add_names[i][len-1] == '.') {
				rname = talloc_strdup(tmp_ctx, add_names[i]);
			} else {
				rname = talloc_asprintf(tmp_ctx, "%s.", add_names[i]);
			}
			status = dns_fill_records_array(tmp_ctx, NULL, DNS_TYPE_A,
							select_flag, rname,
							res->msgs[0], 0, recs,
							NULL, NULL);
			talloc_free(rname);
			talloc_free(res);
			if (!W_ERROR_IS_OK(status)) {
				talloc_free(tmp_ctx);
				return status;
			}
		}
	}

	talloc_free(tmp_ctx);

	*buffer_length = ndr_size_DNS_RPC_RECORDS_ARRAY(recs, 0);
	*buffer = recs;

	return WERR_OK;
}