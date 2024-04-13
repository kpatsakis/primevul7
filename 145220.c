cnt_lookup(struct worker *wrk, struct req *req)
{
	struct objcore *oc, *busy;
	enum lookup_e lr;
	int had_objhead = 0;

	CHECK_OBJ_NOTNULL(wrk, WORKER_MAGIC);
	CHECK_OBJ_NOTNULL(req, REQ_MAGIC);
	AZ(req->objcore);
	AZ(req->stale_oc);

	AN(req->vcl);

	VRY_Prep(req);

	AZ(req->objcore);
	if (req->hash_objhead)
		had_objhead = 1;
	lr = HSH_Lookup(req, &oc, &busy);
	if (lr == HSH_BUSY) {
		/*
		 * We lost the session to a busy object, disembark the
		 * worker thread.   We return to STP_LOOKUP when the busy
		 * object has been unbusied, and still have the objhead
		 * around to restart the lookup with.
		 */
		return (REQ_FSM_DISEMBARK);
	}
	if (had_objhead)
		VSLb_ts_req(req, "Waitinglist", W_TIM_real(wrk));

	if (req->vcf != NULL) {
		(void)req->vcf->func(req, NULL, NULL, 2);
		req->vcf = NULL;
	}

	if (busy == NULL) {
		VRY_Finish(req, DISCARD);
	} else {
		AN(busy->flags & OC_F_BUSY);
		VRY_Finish(req, KEEP);
	}

	AZ(req->objcore);
	if (lr == HSH_MISS || lr == HSH_HITMISS) {
		AN(busy);
		AN(busy->flags & OC_F_BUSY);
		req->objcore = busy;
		req->stale_oc = oc;
		req->req_step = R_STP_MISS;
		if (lr == HSH_HITMISS)
			req->is_hitmiss = 1;
		return (REQ_FSM_MORE);
	}
	if (lr == HSH_HITPASS) {
		AZ(busy);
		AZ(oc);
		req->req_step = R_STP_PASS;
		req->is_hitpass = 1;
		return (REQ_FSM_MORE);
	}

	assert(lr == HSH_HIT || lr == HSH_GRACE);

	CHECK_OBJ_NOTNULL(oc, OBJCORE_MAGIC);
	AZ(oc->flags & OC_F_BUSY);
	req->objcore = oc;
	AZ(oc->flags & OC_F_HFM);

	VSLb(req->vsl, SLT_Hit, "%u %.6f %.6f %.6f",
	    ObjGetXID(wrk, req->objcore),
	    EXP_Dttl(req, req->objcore),
	    req->objcore->grace,
	    req->objcore->keep);

	VCL_hit_method(req->vcl, wrk, req, NULL, NULL);

	switch (wrk->handling) {
	case VCL_RET_DELIVER:
		if (busy != NULL) {
			AZ(oc->flags & OC_F_HFM);
			CHECK_OBJ_NOTNULL(busy->boc, BOC_MAGIC);
			// XXX: shouldn't we go to miss?
			VBF_Fetch(wrk, req, busy, oc, VBF_BACKGROUND);
		} else {
			(void)VRB_Ignore(req);// XXX: handle err
		}
		wrk->stats->cache_hit++;
		req->is_hit = 1;
		if (lr == HSH_GRACE)
			wrk->stats->cache_hit_grace++;
		req->req_step = R_STP_DELIVER;
		return (REQ_FSM_MORE);
	case VCL_RET_RESTART:
		req->req_step = R_STP_RESTART;
		break;
	case VCL_RET_FAIL:
		req->req_step = R_STP_VCLFAIL;
		break;
	case VCL_RET_SYNTH:
		req->req_step = R_STP_SYNTH;
		break;
	case VCL_RET_PASS:
		wrk->stats->cache_hit++;
		req->is_hit = 1;
		req->req_step = R_STP_PASS;
		break;
	default:
		WRONG("Illegal return from vcl_hit{}");
	}

	/* Drop our object, we won't need it */
	(void)HSH_DerefObjCore(wrk, &req->objcore, HSH_RUSH_POLICY);

	if (busy != NULL) {
		(void)HSH_DerefObjCore(wrk, &busy, 0);
		VRY_Clear(req);
	}

	return (REQ_FSM_MORE);
}