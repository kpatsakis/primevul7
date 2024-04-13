CNT_Request(struct req *req)
{
	struct worker *wrk;
	enum req_fsm_nxt nxt;

	CHECK_OBJ_NOTNULL(req, REQ_MAGIC);
	assert(IS_TOPREQ(req) || req->vcl0 == NULL);

	wrk = req->wrk;
	CHECK_OBJ_NOTNULL(wrk, WORKER_MAGIC);

	CHECK_OBJ_NOTNULL(req->transport, TRANSPORT_MAGIC);
	AN(req->transport->deliver);
	AN(req->transport->minimal_response);

	/*
	 * Possible entrance states
	 */
	assert(
	    req->req_step == R_STP_LOOKUP ||
	    req->req_step == R_STP_TRANSPORT);

	AN(req->vsl->wid & VSL_CLIENTMARKER);
	AN(req->vcl);
	CHECK_OBJ(req->privs, VRT_PRIVS_MAGIC);

	for (nxt = REQ_FSM_MORE; nxt == REQ_FSM_MORE; ) {
		/*
		 * This is a good place to be paranoid about the various
		 * pointers still pointing to the things we expect.
		 */
		CHECK_OBJ_NOTNULL(wrk, WORKER_MAGIC);
		CHECK_OBJ_ORNULL(wrk->nobjhead, OBJHEAD_MAGIC);
		CHECK_OBJ_NOTNULL(req, REQ_MAGIC);

		switch (req->req_step) {
#define REQ_STEP(l,u,arg) \
		    case R_STP_##u: \
			if (DO_DEBUG(DBG_REQ_STATE)) \
				cnt_diag(req, #u); \
			nxt = cnt_##l arg; \
			break;
#include "tbl/steps.h"
		default:
			WRONG("State engine misfire");
		}
		CHECK_OBJ_ORNULL(wrk->nobjhead, OBJHEAD_MAGIC);
	}
	wrk->vsl = NULL;
	if (nxt == REQ_FSM_DONE) {
		if (IS_TOPREQ(req) && req->vcl0 != NULL)
			VCL_Rel(&req->vcl0);
		VCL_TaskLeave(req->vcl, req->privs);
		AN(req->vsl->wid);
		VRB_Free(req);
		req->wrk = NULL;
	}
	assert(nxt == REQ_FSM_DISEMBARK || req->ws->r == NULL);
	return (nxt);
}