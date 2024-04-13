cnt_fetch(struct worker *wrk, struct req *req)
{

	CHECK_OBJ_NOTNULL(wrk, WORKER_MAGIC);
	CHECK_OBJ_NOTNULL(req, REQ_MAGIC);
	CHECK_OBJ_NOTNULL(req->objcore, OBJCORE_MAGIC);
	AZ(req->stale_oc);

	wrk->stats->s_fetch++;
	(void)VRB_Ignore(req);

	if (req->objcore->flags & OC_F_FAILED) {
		req->err_code = 503;
		req->req_step = R_STP_SYNTH;
		(void)HSH_DerefObjCore(wrk, &req->objcore, 1);
		AZ(req->objcore);
		return (REQ_FSM_MORE);
	}

	req->req_step = R_STP_DELIVER;
	return (REQ_FSM_MORE);
}