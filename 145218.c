cnt_purge(struct worker *wrk, struct req *req)
{
	struct objcore *oc, *boc;
	enum lookup_e lr;

	CHECK_OBJ_NOTNULL(wrk, WORKER_MAGIC);
	CHECK_OBJ_NOTNULL(req, REQ_MAGIC);
	AZ(req->objcore);
	AZ(req->stale_oc);

	AN(req->vcl);

	VRY_Prep(req);

	AZ(req->objcore);
	req->hash_always_miss = 1;
	lr = HSH_Lookup(req, &oc, &boc);
	assert (lr == HSH_MISS);
	AZ(oc);
	CHECK_OBJ_NOTNULL(boc, OBJCORE_MAGIC);
	VRY_Finish(req, DISCARD);

	(void)HSH_Purge(wrk, boc->objhead, req->t_req, 0, 0, 0);

	AZ(HSH_DerefObjCore(wrk, &boc, 1));

	VCL_purge_method(req->vcl, wrk, req, NULL, NULL);
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
		WRONG("Illegal return from vcl_purge{}");
	}
	return (REQ_FSM_MORE);
}