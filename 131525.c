static WERROR dcesrv_DnssrvComplexOperation(struct dcesrv_call_state *dce_call, TALLOC_CTX *mem_ctx, struct DnssrvComplexOperation *r)
{
	struct dnsserver_state *dsstate;
	struct dnsserver_zone *z;
	WERROR ret;

	ZERO_STRUCTP(r->out.pdwTypeOut);
	ZERO_STRUCTP(r->out.ppDataOut);

	if ((dsstate = dnsserver_connect(dce_call)) == NULL) {
		return WERR_DNS_ERROR_DS_UNAVAILABLE;
	}

	if (r->in.pszZone == NULL) {
		/* Server operation */
		ret = dnsserver_complex_operate_server(dsstate, mem_ctx,
							r->in.pszOperation,
							DNS_CLIENT_VERSION_W2K,
							r->in.dwTypeIn,
							&r->in.pDataIn,
							r->out.pdwTypeOut,
							r->out.ppDataOut);
	} else {
		z = dnsserver_find_zone(dsstate->zones, r->in.pszZone);
		if (z == NULL) {
			return WERR_DNS_ERROR_ZONE_DOES_NOT_EXIST;
		}

		ret = dnsserver_complex_operate_zone(dsstate, mem_ctx, z,
							r->in.pszOperation,
							DNS_CLIENT_VERSION_W2K,
							r->in.dwTypeIn,
							&r->in.pDataIn,
							r->out.pdwTypeOut,
							r->out.ppDataOut);
	}

	if (W_ERROR_EQUAL(ret, WERR_CALL_NOT_IMPLEMENTED)) {
		NDR_PRINT_FUNCTION_DEBUG(DnssrvComplexOperation, NDR_IN, r);
	}
	return ret;
}