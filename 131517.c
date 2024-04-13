static WERROR dnsserver_query_zone(struct dnsserver_state *dsstate,
					TALLOC_CTX *mem_ctx,
					struct dnsserver_zone *z,
					const char *operation,
					const unsigned int client_version,
					enum DNS_RPC_TYPEID *typeid,
					union DNSSRV_RPC_UNION *r)
{
	uint8_t is_integer, is_addresses, is_string;
	uint32_t answer_integer = 0;
	struct IP4_ARRAY *answer_iparray;
	struct DNS_ADDR_ARRAY *answer_addrarray;
	char *answer_string;
	struct dnsserver_zoneinfo *zoneinfo;

	zoneinfo = z->zoneinfo;

	if (strcasecmp(operation, "Zone") == 0) {
		if (client_version == DNS_CLIENT_VERSION_W2K) {
			*typeid = DNSSRV_TYPEID_ZONE_W2K;
			r->ZoneW2K = talloc_zero(mem_ctx, struct DNS_RPC_ZONE_W2K);

			r->ZoneW2K->pszZoneName = talloc_strdup(mem_ctx, z->name);
			r->ZoneW2K->Flags = zoneinfo->Flags;
			r->ZoneW2K->ZoneType = zoneinfo->dwZoneType;
			r->ZoneW2K->Version = zoneinfo->Version;
		} else {
			*typeid = DNSSRV_TYPEID_ZONE;
			r->Zone = talloc_zero(mem_ctx, struct DNS_RPC_ZONE_DOTNET);

			r->Zone->dwRpcStructureVersion = 0x01;
			r->Zone->pszZoneName = talloc_strdup(mem_ctx, z->name);
			r->Zone->Flags = zoneinfo->Flags;
			r->Zone->ZoneType = zoneinfo->dwZoneType;
			r->Zone->Version = zoneinfo->Version;
			r->Zone->dwDpFlags = z->partition->dwDpFlags;
			r->Zone->pszDpFqdn = talloc_strdup(mem_ctx, z->partition->pszDpFqdn);
		}
		return WERR_OK;
	}

	if (strcasecmp(operation, "ZoneInfo") == 0) {
		if (client_version == DNS_CLIENT_VERSION_W2K) {
			*typeid = DNSSRV_TYPEID_ZONE_INFO_W2K;
			r->ZoneInfoW2K = talloc_zero(mem_ctx, struct DNS_RPC_ZONE_INFO_W2K);

			r->ZoneInfoW2K->pszZoneName = talloc_strdup(mem_ctx, z->name);
			r->ZoneInfoW2K->dwZoneType = zoneinfo->dwZoneType;
			r->ZoneInfoW2K->fReverse = zoneinfo->fReverse;
			r->ZoneInfoW2K->fAllowUpdate = zoneinfo->fAllowUpdate;
			r->ZoneInfoW2K->fPaused = zoneinfo->fPaused;
			r->ZoneInfoW2K->fShutdown = zoneinfo->fShutdown;
			r->ZoneInfoW2K->fAutoCreated = zoneinfo->fAutoCreated;
			r->ZoneInfoW2K->fUseDatabase = zoneinfo->fUseDatabase;
			r->ZoneInfoW2K->pszDataFile = talloc_strdup(mem_ctx, zoneinfo->pszDataFile);
			r->ZoneInfoW2K->aipMasters = ip4_array_copy(mem_ctx, zoneinfo->aipMasters);
			r->ZoneInfoW2K->fSecureSecondaries = zoneinfo->fSecureSecondaries;
			r->ZoneInfoW2K->fNotifyLevel = zoneinfo->fNotifyLevel;
			r->ZoneInfoW2K->aipSecondaries = ip4_array_copy(mem_ctx, zoneinfo->aipSecondaries);
			r->ZoneInfoW2K->aipNotify = ip4_array_copy(mem_ctx, zoneinfo->aipNotify);
			r->ZoneInfoW2K->fUseWins = zoneinfo->fUseWins;
			r->ZoneInfoW2K->fUseNbstat = zoneinfo->fUseNbstat;
			r->ZoneInfoW2K->fAging = zoneinfo->fAging;
			r->ZoneInfoW2K->dwNoRefreshInterval = zoneinfo->dwNoRefreshInterval;
			r->ZoneInfoW2K->dwRefreshInterval = zoneinfo->dwRefreshInterval;
			r->ZoneInfoW2K->dwAvailForScavengeTime = zoneinfo->dwAvailForScavengeTime;
			r->ZoneInfoW2K->aipScavengeServers = ip4_array_copy(mem_ctx, zoneinfo->aipScavengeServers);

		} else if (client_version == DNS_CLIENT_VERSION_DOTNET) {
			*typeid = DNSSRV_TYPEID_ZONE_INFO_DOTNET;
			r->ZoneInfoDotNet = talloc_zero(mem_ctx, struct DNS_RPC_ZONE_INFO_DOTNET);

			r->ZoneInfoDotNet->dwRpcStructureVersion = 0x01;
			r->ZoneInfoDotNet->pszZoneName = talloc_strdup(mem_ctx, z->name);
			r->ZoneInfoDotNet->dwZoneType = zoneinfo->dwZoneType;
			r->ZoneInfoDotNet->fReverse = zoneinfo->fReverse;
			r->ZoneInfoDotNet->fAllowUpdate = zoneinfo->fAllowUpdate;
			r->ZoneInfoDotNet->fPaused = zoneinfo->fPaused;
			r->ZoneInfoDotNet->fShutdown = zoneinfo->fShutdown;
			r->ZoneInfoDotNet->fAutoCreated = zoneinfo->fAutoCreated;
			r->ZoneInfoDotNet->fUseDatabase = zoneinfo->fUseDatabase;
			r->ZoneInfoDotNet->pszDataFile = talloc_strdup(mem_ctx, zoneinfo->pszDataFile);
			r->ZoneInfoDotNet->aipMasters = ip4_array_copy(mem_ctx, zoneinfo->aipMasters);
			r->ZoneInfoDotNet->fSecureSecondaries = zoneinfo->fSecureSecondaries;
			r->ZoneInfoDotNet->fNotifyLevel = zoneinfo->fNotifyLevel;
			r->ZoneInfoDotNet->aipSecondaries = ip4_array_copy(mem_ctx, zoneinfo->aipSecondaries);
			r->ZoneInfoDotNet->aipNotify = ip4_array_copy(mem_ctx, zoneinfo->aipNotify);
			r->ZoneInfoDotNet->fUseWins = zoneinfo->fUseWins;
			r->ZoneInfoDotNet->fUseNbstat = zoneinfo->fUseNbstat;
			r->ZoneInfoDotNet->fAging = zoneinfo->fAging;
			r->ZoneInfoDotNet->dwNoRefreshInterval = zoneinfo->dwNoRefreshInterval;
			r->ZoneInfoDotNet->dwRefreshInterval = zoneinfo->dwRefreshInterval;
			r->ZoneInfoDotNet->dwAvailForScavengeTime = zoneinfo->dwAvailForScavengeTime;
			r->ZoneInfoDotNet->aipScavengeServers = ip4_array_copy(mem_ctx, zoneinfo->aipScavengeServers);
			r->ZoneInfoDotNet->dwForwarderTimeout = zoneinfo->dwForwarderTimeout;
			r->ZoneInfoDotNet->fForwarderSlave = zoneinfo->fForwarderSlave;
			r->ZoneInfoDotNet->aipLocalMasters = ip4_array_copy(mem_ctx, zoneinfo->aipLocalMasters);
			r->ZoneInfoDotNet->dwDpFlags = z->partition->dwDpFlags;
			r->ZoneInfoDotNet->pszDpFqdn = talloc_strdup(mem_ctx, z->partition->pszDpFqdn);
			r->ZoneInfoDotNet->pwszZoneDn = talloc_strdup(mem_ctx, zoneinfo->pwszZoneDn);
			r->ZoneInfoDotNet->dwLastSuccessfulSoaCheck = zoneinfo->dwLastSuccessfulSoaCheck;
			r->ZoneInfoDotNet->dwLastSuccessfulXfr = zoneinfo->dwLastSuccessfulXfr;

		} else {
			*typeid = DNSSRV_TYPEID_ZONE_INFO;
			r->ZoneInfo = talloc_zero(mem_ctx, struct DNS_RPC_ZONE_INFO_LONGHORN);

			r->ZoneInfo->dwRpcStructureVersion = 0x02;
			r->ZoneInfo->pszZoneName = talloc_strdup(mem_ctx, z->name);
			r->ZoneInfo->dwZoneType = zoneinfo->dwZoneType;
			r->ZoneInfo->fReverse = zoneinfo->fReverse;
			r->ZoneInfo->fAllowUpdate = zoneinfo->fAllowUpdate;
			r->ZoneInfo->fPaused = zoneinfo->fPaused;
			r->ZoneInfo->fShutdown = zoneinfo->fShutdown;
			r->ZoneInfo->fAutoCreated = zoneinfo->fAutoCreated;
			r->ZoneInfo->fUseDatabase = zoneinfo->fUseDatabase;
			r->ZoneInfo->pszDataFile = talloc_strdup(mem_ctx, zoneinfo->pszDataFile);
			r->ZoneInfo->aipMasters = ip4_array_to_dns_addr_array(mem_ctx, zoneinfo->aipMasters);
			r->ZoneInfo->fSecureSecondaries = zoneinfo->fSecureSecondaries;
			r->ZoneInfo->fNotifyLevel = zoneinfo->fNotifyLevel;
			r->ZoneInfo->aipSecondaries = ip4_array_to_dns_addr_array(mem_ctx, zoneinfo->aipSecondaries);
			r->ZoneInfo->aipNotify = ip4_array_to_dns_addr_array(mem_ctx, zoneinfo->aipNotify);
			r->ZoneInfo->fUseWins = zoneinfo->fUseWins;
			r->ZoneInfo->fUseNbstat = zoneinfo->fUseNbstat;
			r->ZoneInfo->fAging = zoneinfo->fAging;
			r->ZoneInfo->dwNoRefreshInterval = zoneinfo->dwNoRefreshInterval;
			r->ZoneInfo->dwRefreshInterval = zoneinfo->dwRefreshInterval;
			r->ZoneInfo->dwAvailForScavengeTime = zoneinfo->dwAvailForScavengeTime;
			r->ZoneInfo->aipScavengeServers = ip4_array_to_dns_addr_array(mem_ctx, zoneinfo->aipScavengeServers);
			r->ZoneInfo->dwForwarderTimeout = zoneinfo->dwForwarderTimeout;
			r->ZoneInfo->fForwarderSlave = zoneinfo->fForwarderSlave;
			r->ZoneInfo->aipLocalMasters = ip4_array_to_dns_addr_array(mem_ctx, zoneinfo->aipLocalMasters);
			r->ZoneInfo->dwDpFlags = z->partition->dwDpFlags;
			r->ZoneInfo->pszDpFqdn = talloc_strdup(mem_ctx, z->partition->pszDpFqdn);
			r->ZoneInfo->pwszZoneDn = talloc_strdup(mem_ctx, zoneinfo->pwszZoneDn);
			r->ZoneInfo->dwLastSuccessfulSoaCheck = zoneinfo->dwLastSuccessfulSoaCheck;
			r->ZoneInfo->dwLastSuccessfulXfr = zoneinfo->dwLastSuccessfulXfr;

			r->ZoneInfo->fQueuedForBackgroundLoad = zoneinfo->fQueuedForBackgroundLoad;
			r->ZoneInfo->fBackgroundLoadInProgress = zoneinfo->fBackgroundLoadInProgress;
			r->ZoneInfo->fReadOnlyZone = zoneinfo->fReadOnlyZone;
			r->ZoneInfo->dwLastXfrAttempt = zoneinfo->dwLastXfrAttempt;
			r->ZoneInfo->dwLastXfrResult = zoneinfo->dwLastXfrResult;
		}

		return WERR_OK;
	}

	is_integer = 0;

	if (strcasecmp(operation, "AllowUpdate") == 0) {
		answer_integer = zoneinfo->fAllowUpdate;
		is_integer = 1;
	} else if (strcasecmp(operation, "Secured") == 0) {
		answer_integer = 0;
		is_integer = 1;
	} else if (strcasecmp(operation, "DsIntegrated") == 0) {
		answer_integer = zoneinfo->fUseDatabase;
		is_integer = 1;
	} else if (strcasecmp(operation, "LogUpdates") == 0) {
		answer_integer = 0;
		is_integer = 1;
	} else if (strcasecmp(operation, "NoRefreshInterval") == 0) {
		answer_integer = zoneinfo->dwNoRefreshInterval;
		is_integer = 1;
	} else if (strcasecmp(operation, "NotifyLevel") == 0) {
		answer_integer = zoneinfo->fNotifyLevel;
		is_integer = 1;
	} else if (strcasecmp(operation, "RefreshInterval") == 0) {
		answer_integer = zoneinfo->dwRefreshInterval;
		is_integer = 1;
	} else if (strcasecmp(operation, "SecureSecondaries") == 0) {
		answer_integer = zoneinfo->fSecureSecondaries;
		is_integer = 1;
	} else if (strcasecmp(operation, "Type") == 0) {
		answer_integer = zoneinfo->dwZoneType;
		is_integer = 1;
	} else if (strcasecmp(operation, "Aging") == 0) {
		answer_integer = zoneinfo->fAging;
		is_integer = 1;
	} else if (strcasecmp(operation, "ForwarderSlave") == 0) {
		answer_integer = zoneinfo->fForwarderSlave;
		is_integer = 1;
	} else if (strcasecmp(operation, "ForwarderTimeout") == 0) {
		answer_integer = zoneinfo->dwForwarderTimeout;
		is_integer = 1;
	} else if (strcasecmp(operation, "Unicode") == 0) {
		answer_integer = 0;
		is_integer = 1;
	}

	if (is_integer == 1) {
		*typeid = DNSSRV_TYPEID_DWORD;
		r->Dword = answer_integer;
		return WERR_OK;
	}

	is_addresses = 0;

	if (strcasecmp(operation, "AllowNSRecordsAutoCreation") == 0) {
		if (client_version == DNS_CLIENT_VERSION_LONGHORN) {
			answer_addrarray = NULL;
		} else {
			answer_iparray = NULL;
		}
		is_addresses = 1;
	} else if (strcasecmp(operation, "ScavengeServers") == 0) {
		if (client_version == DNS_CLIENT_VERSION_LONGHORN) {
			answer_addrarray = ip4_array_to_dns_addr_array(mem_ctx, zoneinfo->aipScavengeServers);
		} else {
			answer_iparray = ip4_array_copy(mem_ctx, zoneinfo->aipScavengeServers);
		}
		is_addresses = 1;
	} else if (strcasecmp(operation, "MasterServers") == 0) {
		if (client_version == DNS_CLIENT_VERSION_LONGHORN) {
			answer_addrarray = ip4_array_to_dns_addr_array(mem_ctx, zoneinfo->aipMasters);
		} else {
			answer_iparray = ip4_array_copy(mem_ctx, zoneinfo->aipMasters);
		}
		is_addresses = 1;
	} else if (strcasecmp(operation, "LocalMasterServers") == 0) {
		if (client_version == DNS_CLIENT_VERSION_LONGHORN) {
			answer_addrarray = ip4_array_to_dns_addr_array(mem_ctx, zoneinfo->aipLocalMasters);
		} else {
			answer_iparray = ip4_array_copy(mem_ctx, zoneinfo->aipLocalMasters);
		}
		is_addresses = 1;
	} else if (strcasecmp(operation, "NotifyServers") == 0) {
		if (client_version == DNS_CLIENT_VERSION_LONGHORN) {
			answer_addrarray = ip4_array_to_dns_addr_array(mem_ctx, zoneinfo->aipNotify);
		} else {
			answer_iparray = ip4_array_copy(mem_ctx, zoneinfo->aipNotify);
		}
		is_addresses = 1;
	} else if (strcasecmp(operation, "SecondaryServers") == 0) {
		if (client_version == DNS_CLIENT_VERSION_LONGHORN) {
			answer_addrarray = ip4_array_to_dns_addr_array(mem_ctx, zoneinfo->aipSecondaries);
		} else {
			answer_iparray = ip4_array_copy(mem_ctx, zoneinfo->aipSecondaries);
		}
		is_addresses = 1;
	}

	if (is_addresses == 1) {
		if (client_version == DNS_CLIENT_VERSION_LONGHORN) {
			*typeid = DNSSRV_TYPEID_ADDRARRAY;
			r->AddrArray = answer_addrarray;
		} else {
			*typeid = DNSSRV_TYPEID_IPARRAY;
			r->IpArray = answer_iparray;
		}
		return WERR_OK;
	}

	is_string = 0;

	if (strcasecmp(operation, "DatabaseFile") == 0) {
		answer_string = talloc_strdup(mem_ctx, zoneinfo->pszDataFile);
		is_string = 1;
	} else if (strcasecmp(operation, "ApplicationDirectoryPartition") == 0) {
		answer_string = talloc_strdup(mem_ctx, z->partition->pszDpFqdn);
		is_string = 1;
	} else if (strcasecmp(operation, "BreakOnNameUpdate") == 0) {
		answer_string = NULL;
		is_string = 1;
	}

	if (is_string == 1) {
		*typeid = DNSSRV_TYPEID_LPSTR;
		r->String = answer_string;
		return WERR_OK;
	}

	DEBUG(0,("dnsserver: Invalid zone operation %s", operation));
	return WERR_DNS_ERROR_INVALID_PROPERTY;

}