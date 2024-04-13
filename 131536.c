static WERROR dnsserver_update_record(struct dnsserver_state *dsstate,
					TALLOC_CTX *mem_ctx,
					struct dnsserver_zone *z,
					unsigned int client_version,
					const char *node_name,
					struct DNS_RPC_RECORD_BUF *add_buf,
					struct DNS_RPC_RECORD_BUF *del_buf)
{
	TALLOC_CTX *tmp_ctx;
	char *name;
	WERROR status;

	tmp_ctx = talloc_new(mem_ctx);
	W_ERROR_HAVE_NO_MEMORY(tmp_ctx);

	/* If node_name is @ or zone name, dns record is @ */
	if (strcmp(node_name, "@") == 0 ||
	    strcmp(node_name, ".") == 0 ||
	    strcasecmp(node_name, z->name) == 0) {
		name = talloc_strdup(tmp_ctx, "@");
	} else {
		name = dns_split_node_name(tmp_ctx, node_name, z->name);
	}
	W_ERROR_HAVE_NO_MEMORY_AND_FREE(name, tmp_ctx);

	/* CNAMEs can't point to themselves */
	if (add_buf != NULL && add_buf->rec.wType == DNS_TYPE_CNAME) {
		if (cname_self_reference(node_name, z->name, add_buf->rec.data.name)) {
			return WERR_DNS_ERROR_CNAME_LOOP;
		}
	}

	if (add_buf != NULL) {
		if (del_buf == NULL) {
			/* Add record */
			status = dnsserver_db_add_record(tmp_ctx, dsstate->samdb,
								z, name,
								&add_buf->rec);
		} else {
			/* Update record */
			status = dnsserver_db_update_record(tmp_ctx, dsstate->samdb,
								z, name,
								&add_buf->rec,
								&del_buf->rec);
		}
	} else {
		if (del_buf == NULL) {
			/* Add empty node */
			status = dnsserver_db_add_empty_node(tmp_ctx, dsstate->samdb,
								z, name);
		} else {
			/* Delete record */
			status = dnsserver_db_delete_record(tmp_ctx, dsstate->samdb,
								z, name,
								&del_buf->rec);
		}
	}

	talloc_free(tmp_ctx);
	return status;
}