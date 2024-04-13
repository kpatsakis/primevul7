static struct dnsserver_state *dnsserver_connect(struct dcesrv_call_state *dce_call)
{
	struct auth_session_info *session_info =
		dcesrv_call_session_info(dce_call);
	struct dnsserver_state *dsstate;
	struct dnsserver_zone *zones, *z, *znext;
	struct dnsserver_partition *partitions, *p;
	NTSTATUS status;

	dsstate = dcesrv_iface_state_find_conn(dce_call,
					       DNSSERVER_STATE_MAGIC,
					       struct dnsserver_state);
	if (dsstate != NULL) {
		return dsstate;
	}

	dsstate = talloc_zero(dce_call, struct dnsserver_state);
	if (dsstate == NULL) {
		return NULL;
	}

	dsstate->lp_ctx = dce_call->conn->dce_ctx->lp_ctx;

	/* FIXME: create correct auth_session_info for connecting user */
	dsstate->samdb = samdb_connect(dsstate,
				       dce_call->event_ctx,
				       dsstate->lp_ctx,
				       session_info,
				       dce_call->conn->remote_address,
				       0);
	if (dsstate->samdb == NULL) {
		DEBUG(0,("dnsserver: Failed to open samdb"));
		goto failed;
	}

	/* Initialize server info */
	dsstate->serverinfo = dnsserver_init_serverinfo(dsstate,
							dsstate->lp_ctx,
							dsstate->samdb);
	if (dsstate->serverinfo == NULL) {
		goto failed;
	}

	/* Search for DNS partitions */
	partitions = dnsserver_db_enumerate_partitions(dsstate, dsstate->serverinfo, dsstate->samdb);
	if (partitions == NULL) {
		goto failed;
	}
	dsstate->partitions = partitions;

	/* Search for DNS zones */
	for (p = partitions; p; p = p->next) {
		zones = dnsserver_db_enumerate_zones(dsstate, dsstate->samdb, p);
		if (zones == NULL) {
			goto failed;
		}
		for (z = zones; z; ) {
			znext = z->next;
			if (dnsserver_find_zone(dsstate->zones, z->name) == NULL) {
				z->zoneinfo = dnsserver_init_zoneinfo(z, dsstate->serverinfo);
				if (z->zoneinfo == NULL) {
					goto failed;
				}
				DLIST_ADD_END(dsstate->zones, z);
				p->zones_count++;
				dsstate->zones_count++;
			} else {
				/* Ignore duplicate zone */
				DEBUG(3,("dnsserver: Ignoring duplicate zone '%s' from '%s'",
					 z->name, ldb_dn_get_linearized(z->zone_dn)));
			}
			z = znext;
		}
	}

	status = dcesrv_iface_state_store_conn(dce_call,
					       DNSSERVER_STATE_MAGIC,
					       dsstate);
	if (!NT_STATUS_IS_OK(status)) {
		goto failed;
	}

	return dsstate;

failed:
	talloc_free(dsstate);
	dsstate = NULL;
	return NULL;
}