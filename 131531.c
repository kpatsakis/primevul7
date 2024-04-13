static WERROR dnsserver_complex_operate_zone(struct dnsserver_state *dsstate,
					TALLOC_CTX *mem_ctx,
					struct dnsserver_zone *z,
					const char *operation,
					const unsigned int client_version,
					enum DNS_RPC_TYPEID typeid_in,
					union DNSSRV_RPC_UNION *rin,
					enum DNS_RPC_TYPEID *typeid_out,
					union DNSSRV_RPC_UNION *rout)
{
	if (strcasecmp(operation, "QueryDwordProperty") == 0) {
		if (typeid_in == DNSSRV_TYPEID_LPSTR) {
			return dnsserver_query_zone(dsstate, mem_ctx, z,
						rin->String,
						client_version,
						typeid_out,
						rout);

		}
	}

	DEBUG(0,("dnsserver: Invalid zone operation %s", operation));
	return WERR_DNS_ERROR_INVALID_PROPERTY;
}