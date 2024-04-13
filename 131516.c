static WERROR dcesrv_DnssrvOperation2(struct dcesrv_call_state *dce_call, TALLOC_CTX *mem_ctx, struct DnssrvOperation2 *r)
{
	struct dnsserver_state *dsstate;
	struct dnsserver_zone *z = NULL;
	uint32_t request_filter = 0;
	WERROR ret;

	if ((dsstate = dnsserver_connect(dce_call)) == NULL) {
		return WERR_DNS_ERROR_DS_UNAVAILABLE;
	}

	if (r->in.dwContext == 0) {
		if (r->in.pszZone != NULL) {
			request_filter = dnsserver_zone_to_request_filter(r->in.pszZone);
		}
	} else {
		request_filter = r->in.dwContext;
	}

	if (r->in.pszZone == NULL) {
		ret = dnsserver_operate_server(dsstate, mem_ctx,
						r->in.pszOperation,
						r->in.dwClientVersion,
						r->in.dwTypeId,
						&r->in.pData);
	} else {
		z = dnsserver_find_zone(dsstate->zones, r->in.pszZone);
		/*
		 * In the case that request_filter is not 0 and z is NULL,
		 * the request is for a multizone operation, which we do not
		 * yet support, so just error on NULL zone name.
		 */
		if (z == NULL) {
			return WERR_DNS_ERROR_ZONE_DOES_NOT_EXIST;
		}

		ret = dnsserver_operate_zone(dsstate, mem_ctx, z,
						request_filter,
						r->in.pszOperation,
						r->in.dwClientVersion,
						r->in.dwTypeId,
						&r->in.pData);
	}

	if (W_ERROR_EQUAL(ret, WERR_CALL_NOT_IMPLEMENTED)) {
		NDR_PRINT_FUNCTION_DEBUG(DnssrvOperation2, NDR_IN, r);
	}
	return ret;
}