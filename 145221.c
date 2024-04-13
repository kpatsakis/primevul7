cnt_transmit(struct worker *wrk, struct req *req)
{
	struct boc *boc;
	uint16_t status;
	int sendbody, head;
	intmax_t clval;

	CHECK_OBJ_NOTNULL(wrk, WORKER_MAGIC);
	CHECK_OBJ_NOTNULL(req, REQ_MAGIC);
	CHECK_OBJ_NOTNULL(req->transport, TRANSPORT_MAGIC);
	CHECK_OBJ_NOTNULL(req->objcore, OBJCORE_MAGIC);
	AZ(req->stale_oc);
	AZ(req->res_mode);

	/* Grab a ref to the bo if there is one (=streaming) */
	boc = HSH_RefBoc(req->objcore);
	if (boc && boc->state < BOS_STREAM)
		ObjWaitState(req->objcore, BOS_STREAM);
	clval = http_GetContentLength(req->resp);
	/* RFC 7230, 3.3.3 */
	status = http_GetStatus(req->resp);
	head = !strcmp(req->http0->hd[HTTP_HDR_METHOD].b, "HEAD");

	if (boc != NULL)
		req->resp_len = clval;
	else
		req->resp_len = ObjGetLen(req->wrk, req->objcore);

	if (head || status < 200 || status == 204 || status == 304) {
		// rfc7230,l,1748,1752
		sendbody = 0;
	} else {
		sendbody = 1;
	}

	if (req->filter_list == NULL)
		req->filter_list = resp_Get_Filter_List(req);
	if (req->filter_list == NULL ||
	    VCL_StackVDP(req, req->vcl, req->filter_list)) {
		VSLb(req->vsl, SLT_Error, "Failure to push processors");
		req->doclose = SC_OVERLOAD;
	} else {
		if (cache_param->http_range_support && status == 200)
			http_ForceHeader(req->resp, H_Accept_Ranges, "bytes");

		if (status < 200 || status == 204) {
			// rfc7230,l,1691,1695
			http_Unset(req->resp, H_Content_Length);
		} else if (status == 304) {
			// rfc7230,l,1675,1677
			http_Unset(req->resp, H_Content_Length);
		} else if (clval >= 0 && clval == req->resp_len) {
			/* Reuse C-L header */
		} else if (head && req->objcore->flags & OC_F_HFM) {
			/*
			 * Don't touch C-L header (debatable)
			 *
			 * The only way to do it correctly would be to GET
			 * to the backend, and discard the body once the
			 * filters have had a chance to chew on it, but that
			 * would negate the "pass for huge objects" use case.
			 */
		} else {
			http_Unset(req->resp, H_Content_Length);
			if (req->resp_len >= 0)
				http_PrintfHeader(req->resp,
				    "Content-Length: %jd", req->resp_len);
		}
		if (req->resp_len == 0)
			sendbody = 0;
	}
	req->transport->deliver(req, boc, sendbody);

	VSLb_ts_req(req, "Resp", W_TIM_real(wrk));

	HSH_Cancel(wrk, req->objcore, boc);

	if (boc != NULL)
		HSH_DerefBoc(wrk, req->objcore);

	(void)HSH_DerefObjCore(wrk, &req->objcore, HSH_RUSH_POLICY);
	http_Teardown(req->resp);

	req->filter_list = NULL;
	req->res_mode = 0;
	return (REQ_FSM_DONE);
}