cnt_deliver(struct worker *wrk, struct req *req)
{

	CHECK_OBJ_NOTNULL(wrk, WORKER_MAGIC);
	CHECK_OBJ_NOTNULL(req, REQ_MAGIC);
	CHECK_OBJ_NOTNULL(req->objcore, OBJCORE_MAGIC);
	CHECK_OBJ_NOTNULL(req->objcore->objhead, OBJHEAD_MAGIC);
	AZ(req->stale_oc);
	AN(req->vcl);

	assert(req->objcore->refcnt > 0);

	ObjTouch(req->wrk, req->objcore, req->t_prev);

	HTTP_Setup(req->resp, req->ws, req->vsl, SLT_RespMethod);
	if (HTTP_Decode(req->resp,
	    ObjGetAttr(req->wrk, req->objcore, OA_HEADERS, NULL))) {
		(void)HSH_DerefObjCore(wrk, &req->objcore, HSH_RUSH_POLICY);
		req->err_code = 500;
		req->req_step = R_STP_SYNTH;
		return (REQ_FSM_MORE);
	}
	http_ForceField(req->resp, HTTP_HDR_PROTO, "HTTP/1.1");

	if (req->is_hit)
		http_PrintfHeader(req->resp,
		    "X-Varnish: %u %u", VXID(req->vsl->wid),
		    ObjGetXID(wrk, req->objcore));
	else
		http_PrintfHeader(req->resp,
		    "X-Varnish: %u", VXID(req->vsl->wid));

	/*
	 * We base Age calculation upon the last timestamp taken during
	 * client request processing. This gives some inaccuracy, but
	 * since Age is only full second resolution that shouldn't
	 * matter. (Last request timestamp could be a Start timestamp
	 * taken before the object entered into cache leading to negative
	 * age. Truncate to zero in that case).
	 */
	http_PrintfHeader(req->resp, "Age: %.0f",
	    floor(fmax(0., req->t_prev - req->objcore->t_origin)));

	http_SetHeader(req->resp, "Via: 1.1 varnish (Varnish/6.3)");

	if (cache_param->http_gzip_support &&
	    ObjCheckFlag(req->wrk, req->objcore, OF_GZIPED) &&
	    !RFC2616_Req_Gzip(req->http))
		RFC2616_Weaken_Etag(req->resp);

	VCL_deliver_method(req->vcl, wrk, req, NULL, NULL);
	VSLb_ts_req(req, "Process", W_TIM_real(wrk));

	assert(req->restarts <= cache_param->max_restarts);

	if (wrk->handling != VCL_RET_DELIVER) {
		HSH_Cancel(wrk, req->objcore, NULL);
		(void)HSH_DerefObjCore(wrk, &req->objcore, HSH_RUSH_POLICY);
		http_Teardown(req->resp);

		switch (wrk->handling) {
		case VCL_RET_RESTART:
			req->req_step = R_STP_RESTART;
			break;
		case VCL_RET_FAIL:
			req->req_step = R_STP_VCLFAIL;
			break;
		case VCL_RET_SYNTH:
			req->req_step = R_STP_SYNTH;
			break;
		default:
			WRONG("Illegal return from vcl_deliver{}");
		}

		return (REQ_FSM_MORE);
	}

	assert(wrk->handling == VCL_RET_DELIVER);

	if (IS_TOPREQ(req) && RFC2616_Do_Cond(req))
		http_PutResponse(req->resp, "HTTP/1.1", 304, NULL);

	req->req_step = R_STP_TRANSMIT;
	return (REQ_FSM_MORE);
}