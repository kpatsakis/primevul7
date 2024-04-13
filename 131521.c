static WERROR dcesrv_DnssrvUpdateRecord(struct dcesrv_call_state *dce_call, TALLOC_CTX *mem_ctx, struct DnssrvUpdateRecord *r)
{
	struct dnsserver_state *dsstate;
	struct dnsserver_zone *z;
	WERROR ret;

	if ((dsstate = dnsserver_connect(dce_call)) == NULL) {
		return WERR_DNS_ERROR_DS_UNAVAILABLE;
	}

	if (r->in.pszZone == NULL) {
		return WERR_DNS_ERROR_NAME_DOES_NOT_EXIST;
	}

	z = dnsserver_find_zone(dsstate->zones, r->in.pszZone);
	if (z == NULL) {
		return WERR_DNS_ERROR_NAME_DOES_NOT_EXIST;
	}

	ret = dnsserver_update_record(dsstate, mem_ctx, z,
					DNS_CLIENT_VERSION_W2K,
					r->in.pszNodeName,
					r->in.pAddRecord,
					r->in.pDeleteRecord);

	if (W_ERROR_EQUAL(ret, WERR_CALL_NOT_IMPLEMENTED)) {
		NDR_PRINT_FUNCTION_DEBUG(DnssrvUpdateRecord, NDR_IN, r);
	}
	return ret;
}