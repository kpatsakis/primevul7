static void selinux_req_classify_flow(const struct request_sock *req,
				      struct flowi *fl)
{
	fl->flowi_secid = req->secid;
}