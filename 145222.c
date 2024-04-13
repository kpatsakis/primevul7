CNT_Embark(struct worker *wrk, struct req *req)
{

	CHECK_OBJ_NOTNULL(wrk, WORKER_MAGIC);
	CHECK_OBJ_NOTNULL(req, REQ_MAGIC);
	CHECK_OBJ_NOTNULL(req->topreq, REQ_MAGIC);

	/* wrk can have changed for restarts */
	req->vfc->wrk = req->wrk = wrk;
	wrk->vsl = req->vsl;
	if (req->req_step == R_STP_TRANSPORT && req->vcl == NULL) {
		VCL_Refresh(&wrk->vcl);
		req->vcl = wrk->vcl;
		wrk->vcl = NULL;
		VSLb(req->vsl, SLT_VCL_use, "%s", VCL_Name(req->vcl));
	}

	AZ(req->vcl0);
	AN(req->vcl);
}