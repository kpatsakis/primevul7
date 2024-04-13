static WERROR dcesrv_DnssrvEnumRecords(struct dcesrv_call_state *dce_call, TALLOC_CTX *mem_ctx, struct DnssrvEnumRecords *r)
{
	struct dnsserver_state *dsstate;
	struct dnsserver_zone *z;
	WERROR ret;

	ZERO_STRUCTP(r->out.pdwBufferLength);
	ZERO_STRUCTP(r->out.pBuffer);

	if ((dsstate = dnsserver_connect(dce_call)) == NULL) {
		return WERR_DNS_ERROR_DS_UNAVAILABLE;
	}

	if (r->in.pszZone == NULL) {
		return WERR_DNS_ERROR_NAME_DOES_NOT_EXIST;
	}

	if (strcasecmp(r->in.pszZone, "..RootHints") == 0) {
		ret = dnsserver_enumerate_root_records(dsstate, mem_ctx,
					DNS_CLIENT_VERSION_W2K,
					r->in.pszNodeName,
					r->in.wRecordType,
					r->in.fSelectFlag,
					r->out.pdwBufferLength,
					r->out.pBuffer);
	} else {
		z = dnsserver_find_zone(dsstate->zones, r->in.pszZone);
		if (z == NULL) {
			return WERR_DNS_ERROR_NAME_DOES_NOT_EXIST;
		}

		ret = dnsserver_enumerate_records(dsstate, mem_ctx, z,
					DNS_CLIENT_VERSION_W2K,
					r->in.pszNodeName,
					r->in.pszStartChild,
					r->in.wRecordType,
					r->in.fSelectFlag,
					r->in.pszFilterStart,
					r->in.pszFilterStop,
					r->out.pdwBufferLength,
					r->out.pBuffer);
	}

	if (W_ERROR_EQUAL(ret, WERR_CALL_NOT_IMPLEMENTED)) {
		NDR_PRINT_FUNCTION_DEBUG(DnssrvEnumRecords, NDR_IN, r);
	}
	return ret;
}