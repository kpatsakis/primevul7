cnt_pipe(struct worker *wrk, struct req *req)
{
	struct busyobj *bo;
	enum req_fsm_nxt nxt;

	CHECK_OBJ_NOTNULL(wrk, WORKER_MAGIC);
	CHECK_OBJ_NOTNULL(req, REQ_MAGIC);
	AZ(req->objcore);
	AZ(req->stale_oc);
	AN(req->vcl);

	wrk->stats->s_pipe++;
	bo = VBO_GetBusyObj(wrk, req);
	CHECK_OBJ_NOTNULL(bo, BUSYOBJ_MAGIC);
	VSLb(bo->vsl, SLT_Begin, "bereq %u pipe", VXID(req->vsl->wid));
	VSLb(req->vsl, SLT_Link, "bereq %u pipe", VXID(bo->vsl->wid));
	THR_SetBusyobj(bo);
	bo->sp = req->sp;
	SES_Ref(bo->sp);

	HTTP_Setup(bo->bereq, bo->ws, bo->vsl, SLT_BereqMethod);
	http_FilterReq(bo->bereq, req->http, 0);	// XXX: 0 ?
	http_PrintfHeader(bo->bereq, "X-Varnish: %u", VXID(req->vsl->wid));
	http_ForceHeader(bo->bereq, H_Connection, "close");

	if (req->want100cont) {
		http_SetHeader(bo->bereq, "Expect: 100-continue");
		req->want100cont = 0;
	}

	VCL_pipe_method(req->vcl, wrk, req, bo, NULL);

	switch (wrk->handling) {
	case VCL_RET_SYNTH:
		req->req_step = R_STP_SYNTH;
		nxt = REQ_FSM_MORE;
		break;
	case VCL_RET_PIPE:
		if (V1P_Enter() == 0) {
			AZ(bo->req);
			bo->req = req;
			bo->wrk = wrk;
			SES_Close(req->sp, VDI_Http1Pipe(req, bo));
			nxt = REQ_FSM_DONE;
			V1P_Leave();
			break;
		}
		wrk->stats->pipe_limited++;
		/* fall through */
	case VCL_RET_FAIL:
		req->req_step = R_STP_VCLFAIL;
		nxt = REQ_FSM_MORE;
		break;
	default:
		WRONG("Illegal return from vcl_pipe{}");
	}
	http_Teardown(bo->bereq);
	SES_Rel(bo->sp);
	VBO_ReleaseBusyObj(wrk, &bo);
	THR_SetBusyobj(NULL);
	return (nxt);
}