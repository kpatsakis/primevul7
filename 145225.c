cnt_recv(struct worker *wrk, struct req *req)
{
	unsigned recv_handling;
	struct VSHA256Context sha256ctx;
	const char *ci, *cp, *endpname;

	CHECK_OBJ_NOTNULL(wrk, WORKER_MAGIC);
	CHECK_OBJ_NOTNULL(req, REQ_MAGIC);
	AN(req->vcl);
	AZ(req->objcore);
	AZ(req->stale_oc);
	AZ(req->err_code);

	AZ(isnan(req->t_first));
	AZ(isnan(req->t_prev));
	AZ(isnan(req->t_req));

	ci = SES_Get_String_Attr(req->sp, SA_CLIENT_IP);
	cp = SES_Get_String_Attr(req->sp, SA_CLIENT_PORT);
	CHECK_OBJ_NOTNULL(req->sp->listen_sock, LISTEN_SOCK_MAGIC);
	endpname = req->sp->listen_sock->name;
	AN(endpname);
	VSLb(req->vsl, SLT_ReqStart, "%s %s %s", ci, cp, endpname);

	http_VSL_log(req->http);

	cnt_recv_prep(req, ci);

	if (req->req_body_status == REQ_BODY_FAIL) {
		req->doclose = SC_OVERLOAD;
		return (REQ_FSM_DONE);
	}

	VCL_recv_method(req->vcl, wrk, req, NULL, NULL);
	if (wrk->handling == VCL_RET_VCL && req->restarts == 0) {
		Req_Rollback(req);
		cnt_recv_prep(req, ci);
		VCL_recv_method(req->vcl, wrk, req, NULL, NULL);
	}

	if (req->want100cont && !req->late100cont) {
		req->want100cont = 0;
		if (req->transport->minimal_response(req, 100)) {
			req->doclose = SC_REM_CLOSE;
			return (REQ_FSM_DONE);
		}
	}

	/* Attempts to cache req.body may fail */
	if (req->req_body_status == REQ_BODY_FAIL) {
		req->doclose = SC_RX_BODY;
		return (REQ_FSM_DONE);
	}

	recv_handling = wrk->handling;

	/* We wash the A-E header here for the sake of VRY */
	if (cache_param->http_gzip_support &&
	     (recv_handling != VCL_RET_PIPE) &&
	     (recv_handling != VCL_RET_PASS)) {
		if (RFC2616_Req_Gzip(req->http)) {
			http_ForceHeader(req->http, H_Accept_Encoding, "gzip");
		} else {
			http_Unset(req->http, H_Accept_Encoding);
		}
	}

	VSHA256_Init(&sha256ctx);
	VCL_hash_method(req->vcl, wrk, req, NULL, &sha256ctx);
	if (wrk->handling == VCL_RET_FAIL)
		recv_handling = wrk->handling;
	else
		assert(wrk->handling == VCL_RET_LOOKUP);
	VSHA256_Final(req->digest, &sha256ctx);

	switch (recv_handling) {
	case VCL_RET_VCL:
		VSLb(req->vsl, SLT_VCL_Error,
		    "Illegal return(vcl): %s",
		    req->restarts ? "Not after restarts" :
		    "Only from active VCL");
		req->err_code = 503;
		req->req_step = R_STP_SYNTH;
		break;
	case VCL_RET_PURGE:
		req->req_step = R_STP_PURGE;
		break;
	case VCL_RET_HASH:
		req->req_step = R_STP_LOOKUP;
		break;
	case VCL_RET_PIPE:
		if (!IS_TOPREQ(req)) {
			VSLb(req->vsl, SLT_VCL_Error,
			    "vcl_recv{} returns pipe for ESI included object."
			    "  Doing pass.");
			req->req_step = R_STP_PASS;
		} else if (req->http0->protover > 11) {
			VSLb(req->vsl, SLT_VCL_Error,
			    "vcl_recv{} returns pipe for HTTP/2 request."
			    "  Doing pass.");
			req->req_step = R_STP_PASS;
		} else {
			req->req_step = R_STP_PIPE;
		}
		break;
	case VCL_RET_PASS:
		req->req_step = R_STP_PASS;
		break;
	case VCL_RET_SYNTH:
		req->req_step = R_STP_SYNTH;
		break;
	case VCL_RET_RESTART:
		req->req_step = R_STP_RESTART;
		break;
	case VCL_RET_FAIL:
		req->req_step = R_STP_VCLFAIL;
		break;
	default:
		WRONG("Illegal return from vcl_recv{}");
	}
	return (REQ_FSM_MORE);
}