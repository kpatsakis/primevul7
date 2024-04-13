cnt_transport(struct worker *wrk, struct req *req)
{
	const char *p;

	CHECK_OBJ_NOTNULL(wrk, WORKER_MAGIC);
	CHECK_OBJ_NOTNULL(req, REQ_MAGIC);
	CHECK_OBJ_NOTNULL(req->http, HTTP_MAGIC);
	CHECK_OBJ_NOTNULL(req->transport, TRANSPORT_MAGIC);
	assert(req->req_body_status != REQ_BODY_INIT);

	if (http_GetHdr(req->http, H_Expect, &p)) {
		if (strcasecmp(p, "100-continue")) {
			req->doclose = SC_RX_JUNK;
			(void)req->transport->minimal_response(req, 417);
			wrk->stats->client_req_417++;
			return (REQ_FSM_DONE);
		}
		if (req->http->protover >= 11 &&
		    req->htc->pipeline_b == NULL)	// XXX: HTTP1 vs 2 ?
			req->want100cont = 1;
		http_Unset(req->http, H_Expect);
	}

	AZ(req->err_code);

	req->doclose = http_DoConnection(req->http);
	if (req->doclose == SC_RX_BAD) {
		(void)req->transport->minimal_response(req, 400);
		return (REQ_FSM_DONE);
	}

	if (req->req_body_status < REQ_BODY_TAKEN) {
		AN(req->transport->req_body != NULL);
		VFP_Setup(req->vfc, wrk);
		req->vfc->resp = req->http;		// XXX
		req->transport->req_body(req);
	}

	req->ws_req = WS_Snapshot(req->ws);
	HTTP_Clone(req->http0, req->http);	// For ESI & restart
	req->req_step = R_STP_RECV;
	return (REQ_FSM_MORE);
}