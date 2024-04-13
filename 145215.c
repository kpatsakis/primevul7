cnt_vclfail(const struct worker *wrk, struct req *req)
{

	CHECK_OBJ_NOTNULL(wrk, WORKER_MAGIC);
	CHECK_OBJ_NOTNULL(req, REQ_MAGIC);

	AZ(req->objcore);
	AZ(req->stale_oc);

	Req_Rollback(req);

	req->err_code = 503;
	req->err_reason = "VCL failed";
	req->req_step = R_STP_SYNTH;
	req->doclose = SC_VCL_FAILURE;
	return (REQ_FSM_MORE);
}