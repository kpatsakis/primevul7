cnt_diag(struct req *req, const char *state)
{

	CHECK_OBJ_NOTNULL(req, REQ_MAGIC);

	VSLb(req->vsl,  SLT_Debug, "vxid %u STP_%s sp %p vcl %p",
	    req->vsl->wid, state, req->sp, req->vcl);
	VSL_Flush(req->vsl, 0);
}