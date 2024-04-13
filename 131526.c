static WERROR dcesrv_DnssrvQuery(struct dcesrv_call_state *dce_call, TALLOC_CTX *mem_ctx, struct DnssrvQuery *r)
{
	struct dnsserver_state *dsstate;
	struct dnsserver_zone *z;
	WERROR ret;

	ZERO_STRUCTP(r->out.pdwTypeId);
	ZERO_STRUCTP(r->out.ppData);

	if ((dsstate = dnsserver_connect(dce_call)) == NULL) {
		return WERR_DNS_ERROR_DS_UNAVAILABLE;
	}

	if (r->in.pszZone == NULL) {
		/* FIXME: DNS Server Configuration Access Control List */
		ret = dnsserver_query_server(dsstate, mem_ctx,
						r->in.pszOperation,
						DNS_CLIENT_VERSION_W2K,
						r->out.pdwTypeId,
						r->out.ppData);
	} else {
		z = dnsserver_find_zone(dsstate->zones, r->in.pszZone);
		if (z == NULL) {
			return WERR_DNS_ERROR_ZONE_DOES_NOT_EXIST;
		}

		ret = dnsserver_query_zone(dsstate, mem_ctx, z,
						r->in.pszOperation,
						DNS_CLIENT_VERSION_W2K,
						r->out.pdwTypeId,
						r->out.ppData);
	}

	if (W_ERROR_EQUAL(ret, WERR_CALL_NOT_IMPLEMENTED)) {
		NDR_PRINT_FUNCTION_DEBUG(DnssrvQuery, NDR_IN, r);
	}
	return ret;
}