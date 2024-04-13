cnt_miss(struct worker *wrk, struct req *req)
{

	CHECK_OBJ_NOTNULL(wrk, WORKER_MAGIC);
	CHECK_OBJ_NOTNULL(req, REQ_MAGIC);
	AN(req->vcl);
	CHECK_OBJ_NOTNULL(req->objcore, OBJCORE_MAGIC);
	CHECK_OBJ_ORNULL(req->stale_oc, OBJCORE_MAGIC);

	VCL_miss_method(req->vcl, wrk, req, NULL, NULL);
	switch (wrk->handling) {
	case VCL_RET_FETCH:
		wrk->stats->cache_miss++;
		VBF_Fetch(wrk, req, req->objcore, req->stale_oc, VBF_NORMAL);
		if (req->stale_oc != NULL)
			(void)HSH_DerefObjCore(wrk, &req->stale_oc, 0);
		req->req_step = R_STP_FETCH;
		return (REQ_FSM_MORE);
	case VCL_RET_FAIL:
		req->req_step = R_STP_VCLFAIL;
		break;
	case VCL_RET_SYNTH:
		req->req_step = R_STP_SYNTH;
		break;
	case VCL_RET_RESTART:
		req->req_step = R_STP_RESTART;
		break;
	case VCL_RET_PASS:
		req->req_step = R_STP_PASS;
		break;
	default:
		WRONG("Illegal return from vcl_miss{}");
	}
	VRY_Clear(req);
	if (req->stale_oc != NULL)
		(void)HSH_DerefObjCore(wrk, &req->stale_oc, 0);
	AZ(HSH_DerefObjCore(wrk, &req->objcore, 1));
	return (REQ_FSM_MORE);
}