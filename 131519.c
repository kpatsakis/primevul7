static WERROR dnsserver_operate_zone(struct dnsserver_state *dsstate,
					TALLOC_CTX *mem_ctx,
					struct dnsserver_zone *z,
					unsigned int request_filter,
					const char *operation,
					const unsigned int client_version,
					enum DNS_RPC_TYPEID typeid,
					union DNSSRV_RPC_UNION *r)
{
	bool valid_operation = false;

	if (strcasecmp(operation, "ResetDwordProperty") == 0) {

		if (typeid != DNSSRV_TYPEID_NAME_AND_PARAM) {
			return WERR_DNS_ERROR_INVALID_PROPERTY;
		}

		return dnsserver_db_do_reset_dword(dsstate->samdb, z,
					           r->NameAndParam);

	} else if (strcasecmp(operation, "ZoneTypeReset") == 0) {
		valid_operation = true;
	} else if (strcasecmp(operation, "PauseZone") == 0) {
		valid_operation = true;
	} else if (strcasecmp(operation, "ResumeZone") == 0) {
		valid_operation = true;
	} else if (strcasecmp(operation, "DeleteZone") == 0) {
		valid_operation = true;
	} else if (strcasecmp(operation, "ReloadZone") == 0) {
		valid_operation = true;
	} else if (strcasecmp(operation, "RefreshZone") == 0) {
		valid_operation = true;
	} else if (strcasecmp(operation, "ExpireZone") == 0) {
		valid_operation = true;
	} else if (strcasecmp(operation, "IncrementVersion") == 0) {
		valid_operation = true;
	} else if (strcasecmp(operation, "WriteBackFile") == 0) {
		valid_operation = true;
	} else if (strcasecmp(operation, "DeleteZoneFromDs") == 0) {
		WERROR status;
		if (z == NULL) {
			return WERR_DNS_ERROR_ZONE_DOES_NOT_EXIST;
		}
		status =  dnsserver_db_delete_zone(dsstate->samdb, z);
		if (W_ERROR_IS_OK(status)) {
			dnsserver_reload_zones(dsstate);
		}
		return status;
	} else if (strcasecmp(operation, "UpdateZoneFromDs") == 0) {
		valid_operation = true;
	} else if (strcasecmp(operation, "ZoneExport") == 0) {
		valid_operation = true;
	} else if (strcasecmp(operation, "ZoneChangeDirectoryPartition") == 0) {
		valid_operation = true;
	} else if (strcasecmp(operation, "DeleteNode") == 0) {
		valid_operation = true;
	} else if (strcasecmp(operation, "DeleteRecordSet") == 0) {
		valid_operation = true;
	} else if (strcasecmp(operation, "ForceAgingOnNode") == 0) {
		valid_operation = true;
	} else if (strcasecmp(operation, "DatabaseFile") == 0) {
		valid_operation = true;
	} else if (strcasecmp(operation, "MasterServers") == 0) {
		valid_operation = true;
	} else if (strcasecmp(operation, "LocalMasterServers") == 0) {
		valid_operation = true;
	} else if (strcasecmp(operation, "NotifyServers") == 0) {
		valid_operation = true;
	} else if (strcasecmp(operation, "SecondaryServers") == 0) {
		valid_operation = true;
	} else if (strcasecmp(operation, "ScavengingServers") == 0) {
		valid_operation = true;
	} else if (strcasecmp(operation, "AllowNSRecordsAutoCreation") == 0) {
		valid_operation = true;
	} else if (strcasecmp(operation, "BreakOnNameUpdate") == 0) {
		valid_operation = true;
	} else if (strcasecmp(operation, "ApplicationDirectoryPartition") == 0) {
		valid_operation = true;
	}

	if (valid_operation) {
		DEBUG(0, ("dnsserver: zone operation '%s' not implemented", operation));
		return WERR_CALL_NOT_IMPLEMENTED;
	}

	DEBUG(0, ("dnsserver: invalid zone operation '%s'", operation));
	return WERR_DNS_ERROR_INVALID_PROPERTY;
}