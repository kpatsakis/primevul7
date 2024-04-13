static WERROR dnsserver_operate_server(struct dnsserver_state *dsstate,
					TALLOC_CTX *mem_ctx,
					const char *operation,
					const unsigned int client_version,
					enum DNS_RPC_TYPEID typeid,
					union DNSSRV_RPC_UNION *r)
{
	bool valid_operation = false;

	if (strcasecmp(operation, "ResetDwordProperty") == 0) {
		valid_operation = true;
	} else if (strcasecmp(operation, "Restart") == 0) {
		valid_operation = true;
	} else if (strcasecmp(operation, "ClearDebugLog") == 0) {
		valid_operation = true;
	} else if (strcasecmp(operation, "ClearCache") == 0) {
		valid_operation = true;
	} else if (strcasecmp(operation, "WriteDirtyZones") == 0) {
		valid_operation = true;
	} else if (strcasecmp(operation, "ZoneCreate") == 0) {
		struct dnsserver_zone *z, *z2;
		WERROR status;
		int len;

		z = talloc_zero(mem_ctx, struct dnsserver_zone);
		W_ERROR_HAVE_NO_MEMORY(z);
		z->partition = talloc_zero(z, struct dnsserver_partition);
		W_ERROR_HAVE_NO_MEMORY_AND_FREE(z->partition, z);
		z->zoneinfo = talloc_zero(z, struct dnsserver_zoneinfo);
		W_ERROR_HAVE_NO_MEMORY_AND_FREE(z->zoneinfo, z);

		if (typeid == DNSSRV_TYPEID_ZONE_CREATE_W2K) {
			len = strlen(r->ZoneCreateW2K->pszZoneName);
			if (r->ZoneCreateW2K->pszZoneName[len-1] == '.') {
				len -= 1;
			}
			z->name = talloc_strndup(z, r->ZoneCreateW2K->pszZoneName, len);
			z->zoneinfo->dwZoneType = r->ZoneCreateW2K->dwZoneType;
			z->zoneinfo->fAllowUpdate = r->ZoneCreateW2K->fAllowUpdate;
			z->zoneinfo->fAging = r->ZoneCreateW2K->fAging;
			z->zoneinfo->Flags = r->ZoneCreateW2K->dwFlags;
		} else if (typeid == DNSSRV_TYPEID_ZONE_CREATE_DOTNET) {
			len = strlen(r->ZoneCreateDotNet->pszZoneName);
			if (r->ZoneCreateDotNet->pszZoneName[len-1] == '.') {
				len -= 1;
			}
			z->name = talloc_strndup(z, r->ZoneCreateDotNet->pszZoneName, len);
			z->zoneinfo->dwZoneType = r->ZoneCreateDotNet->dwZoneType;
			z->zoneinfo->fAllowUpdate = r->ZoneCreateDotNet->fAllowUpdate;
			z->zoneinfo->fAging = r->ZoneCreateDotNet->fAging;
			z->zoneinfo->Flags = r->ZoneCreateDotNet->dwFlags;
			z->partition->dwDpFlags = r->ZoneCreateDotNet->dwDpFlags;
		} else if (typeid == DNSSRV_TYPEID_ZONE_CREATE) {
			len = strlen(r->ZoneCreate->pszZoneName);
			if (r->ZoneCreate->pszZoneName[len-1] == '.') {
				len -= 1;
			}
			z->name = talloc_strndup(z, r->ZoneCreate->pszZoneName, len);
			z->zoneinfo->dwZoneType = r->ZoneCreate->dwZoneType;
			z->zoneinfo->fAllowUpdate = r->ZoneCreate->fAllowUpdate;
			z->zoneinfo->fAging = r->ZoneCreate->fAging;
			z->zoneinfo->Flags = r->ZoneCreate->dwFlags;
			z->partition->dwDpFlags = r->ZoneCreate->dwDpFlags;
		} else {
			talloc_free(z);
			return WERR_DNS_ERROR_INVALID_PROPERTY;
		}

		z2 = dnsserver_find_zone(dsstate->zones, z->name);
		if (z2 != NULL) {
			talloc_free(z);
			return WERR_DNS_ERROR_ZONE_ALREADY_EXISTS;
		}

		status = dnsserver_db_create_zone(dsstate->samdb, dsstate->partitions, z,
						  dsstate->lp_ctx);
		talloc_free(z);

		if (W_ERROR_IS_OK(status)) {
			dnsserver_reload_zones(dsstate);
		}
		return status;
	} else if (strcasecmp(operation, "ClearStatistics") == 0) {
		valid_operation = true;
	} else if (strcasecmp(operation, "EnlistDirectoryPartition") == 0) {
		valid_operation = true;
	} else if (strcasecmp(operation, "StartScavenging") == 0) {
		valid_operation = true;
	} else if (strcasecmp(operation, "AbortScavenging") == 0) {
		valid_operation = true;
	} else if (strcasecmp(operation, "AutoConfigure") == 0) {
		valid_operation = true;
	} else if (strcasecmp(operation, "ExportSettings") == 0) {
		valid_operation = true;
	} else if (strcasecmp(operation, "PrepareForDemotion") == 0) {
		valid_operation = true;
	} else if (strcasecmp(operation, "PrepareForUninstall") == 0) {
		valid_operation = true;
	} else if (strcasecmp(operation, "DeleteNode") == 0) {
		valid_operation = true;
	} else if (strcasecmp(operation, "DeleteRecord") == 0) {
		valid_operation = true;
	} else if (strcasecmp(operation, "WriteBackFile") == 0) {
		valid_operation = true;
	} else if (strcasecmp(operation, "ListenAddresses") == 0) {
		valid_operation = true;
	} else if (strcasecmp(operation, "Forwarders") == 0) {
		valid_operation = true;
	} else if (strcasecmp(operation, "LogFilePath") == 0) {
		valid_operation = true;
	} else if (strcasecmp(operation, "LogIpFilterList") == 0) {
		valid_operation = true;
	} else if (strcasecmp(operation, "ForestDirectoryPartitionBaseName") == 0) {
		valid_operation = true;
	} else if (strcasecmp(operation, "DomainDirectoryPartitionBaseName") == 0) {
		valid_operation = true;
	} else if (strcasecmp(operation, "GlobalQueryBlockList") == 0) {
		valid_operation = true;
	} else if (strcasecmp(operation, "BreakOnReceiveFrom") == 0) {
		valid_operation = true;
	} else if (strcasecmp(operation, "BreakOnUpdateFrom") == 0) {
		valid_operation = true;
	} else if (strcasecmp(operation, "ServerLevelPluginDll") == 0) {
		valid_operation = true;
	}

	if (valid_operation) {
		DEBUG(0, ("dnsserver: server operation '%s' not implemented", operation));
		return WERR_CALL_NOT_IMPLEMENTED;
	}

	DEBUG(0, ("dnsserver: invalid server operation '%s'", operation));
	return WERR_DNS_ERROR_INVALID_PROPERTY;
}