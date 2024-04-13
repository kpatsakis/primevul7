cnt_synth(struct worker *wrk, struct req *req)
{
	struct http *h;
	double now;
	struct vsb *synth_body;
	ssize_t sz, szl;
	uint8_t *ptr;

	CHECK_OBJ_NOTNULL(wrk, WORKER_MAGIC);
	CHECK_OBJ_NOTNULL(req, REQ_MAGIC);

	AZ(req->objcore);
	AZ(req->stale_oc);

	wrk->stats->s_synth++;

	now = W_TIM_real(wrk);
	VSLb_ts_req(req, "Process", now);

	if (req->err_code < 100)
		req->err_code = 501;

	HTTP_Setup(req->resp, req->ws, req->vsl, SLT_RespMethod);
	h = req->resp;
	http_TimeHeader(h, "Date: ", now);
	http_SetHeader(h, "Server: Varnish");
	http_PrintfHeader(req->resp, "X-Varnish: %u", VXID(req->vsl->wid));
	http_PutResponse(h, "HTTP/1.1", req->err_code, req->err_reason);

	/*
	 * For late 100-continue, we suggest to VCL to close the connection to
	 * neither send a 100-continue nor drain-read the request. But VCL has
	 * the option to veto by removing Connection: close
	 */
	if (req->want100cont) {
		http_SetHeader(h, "Connection: close");
	}

	synth_body = VSB_new_auto();
	AN(synth_body);

	VCL_synth_method(req->vcl, wrk, req, NULL, synth_body);

	AZ(VSB_finish(synth_body));

	if (wrk->handling == VCL_RET_FAIL) {
		VSB_destroy(&synth_body);
		req->doclose = SC_VCL_FAILURE;
		VSLb_ts_req(req, "Resp", W_TIM_real(wrk));
		http_Teardown(req->resp);
		return (REQ_FSM_DONE);
	}

	if (wrk->handling == VCL_RET_RESTART &&
	    req->restarts > cache_param->max_restarts)
		wrk->handling = VCL_RET_DELIVER;

	if (wrk->handling == VCL_RET_RESTART) {
		/*
		 * XXX: Should we reset req->doclose = SC_VCL_FAILURE
		 * XXX: If so, to what ?
		 */
		HTTP_Setup(h, req->ws, req->vsl, SLT_RespMethod);
		VSB_destroy(&synth_body);
		req->req_step = R_STP_RESTART;
		return (REQ_FSM_MORE);
	}
	assert(wrk->handling == VCL_RET_DELIVER);

	http_Unset(h, H_Content_Length);
	http_PrintfHeader(req->resp, "Content-Length: %zd",
	    VSB_len(synth_body));

	if (!req->doclose && http_HdrIs(req->resp, H_Connection, "close"))
		req->doclose = SC_RESP_CLOSE;

	/* Discard any lingering request body before delivery */
	(void)VRB_Ignore(req);

	req->objcore = HSH_Private(wrk);
	CHECK_OBJ_NOTNULL(req->objcore, OBJCORE_MAGIC);
	szl = -1;
	if (STV_NewObject(wrk, req->objcore, stv_transient, 1024)) {
		szl = VSB_len(synth_body);
		assert(szl >= 0);
		sz = szl;
		if (sz > 0 &&
		    ObjGetSpace(wrk, req->objcore, &sz, &ptr) && sz >= szl) {
			memcpy(ptr, VSB_data(synth_body), szl);
			ObjExtend(wrk, req->objcore, szl);
		} else if (sz > 0) {
			szl = -1;
		}
	}

	if (szl >= 0)
		AZ(ObjSetU64(wrk, req->objcore, OA_LEN, szl));
	HSH_DerefBoc(wrk, req->objcore);
	VSB_destroy(&synth_body);

	if (szl < 0) {
		VSLb(req->vsl, SLT_Error, "Could not get storage");
		req->doclose = SC_OVERLOAD;
		VSLb_ts_req(req, "Resp", W_TIM_real(wrk));
		(void)HSH_DerefObjCore(wrk, &req->objcore, 1);
		http_Teardown(req->resp);
		return (REQ_FSM_DONE);
	}

	req->req_step = R_STP_TRANSMIT;
	return (REQ_FSM_MORE);
}