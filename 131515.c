static WERROR dnsserver_query_server(struct dnsserver_state *dsstate,
					TALLOC_CTX *mem_ctx,
					const char *operation,
					const unsigned int client_version,
					enum DNS_RPC_TYPEID *typeid,
					union DNSSRV_RPC_UNION *r)
{
	uint8_t is_integer, is_addresses, is_string, is_wstring, is_stringlist;
	uint32_t answer_integer;
	struct IP4_ARRAY *answer_iparray;
	struct DNS_ADDR_ARRAY *answer_addrarray;
	char *answer_string;
	struct DNS_RPC_UTF8_STRING_LIST *answer_stringlist;
	struct dnsserver_serverinfo *serverinfo;

	serverinfo = dsstate->serverinfo;

	if (strcasecmp(operation, "ServerInfo") == 0) {
		if (client_version == DNS_CLIENT_VERSION_W2K) {
			*typeid = DNSSRV_TYPEID_SERVER_INFO_W2K;
			r->ServerInfoW2K = talloc_zero(mem_ctx, struct DNS_RPC_SERVER_INFO_W2K);

			r->ServerInfoW2K->dwVersion = serverinfo->dwVersion;
			r->ServerInfoW2K->fBootMethod = serverinfo->fBootMethod;
			r->ServerInfoW2K->fAdminConfigured = serverinfo->fAdminConfigured;
			r->ServerInfoW2K->fAllowUpdate = serverinfo->fAllowUpdate;
			r->ServerInfoW2K->fDsAvailable = serverinfo->fDsAvailable;
			r->ServerInfoW2K->pszServerName = talloc_strdup(mem_ctx, serverinfo->pszServerName);
			r->ServerInfoW2K->pszDsContainer = talloc_strdup(mem_ctx, serverinfo->pszDsContainer);
			r->ServerInfoW2K->aipServerAddrs = dns_addr_array_to_ip4_array(mem_ctx,
										       serverinfo->aipServerAddrs);
			r->ServerInfoW2K->aipListenAddrs = dns_addr_array_to_ip4_array(mem_ctx,
										       serverinfo->aipListenAddrs);
			r->ServerInfoW2K->aipForwarders = ip4_array_copy(mem_ctx, serverinfo->aipForwarders);
			r->ServerInfoW2K->dwLogLevel = serverinfo->dwLogLevel;
			r->ServerInfoW2K->dwDebugLevel = serverinfo->dwDebugLevel;
			r->ServerInfoW2K->dwForwardTimeout = serverinfo->dwForwardTimeout;
			r->ServerInfoW2K->dwRpcProtocol = serverinfo->dwRpcProtocol;
			r->ServerInfoW2K->dwNameCheckFlag = serverinfo->dwNameCheckFlag;
			r->ServerInfoW2K->cAddressAnswerLimit = serverinfo->cAddressAnswerLimit;
			r->ServerInfoW2K->dwRecursionRetry = serverinfo->dwRecursionRetry;
			r->ServerInfoW2K->dwRecursionTimeout = serverinfo->dwRecursionTimeout;
			r->ServerInfoW2K->dwMaxCacheTtl = serverinfo->dwMaxCacheTtl;
			r->ServerInfoW2K->dwDsPollingInterval = serverinfo->dwDsPollingInterval;
			r->ServerInfoW2K->dwScavengingInterval = serverinfo->dwScavengingInterval;
			r->ServerInfoW2K->dwDefaultRefreshInterval = serverinfo->dwDefaultRefreshInterval;
			r->ServerInfoW2K->dwDefaultNoRefreshInterval = serverinfo->dwDefaultNoRefreshInterval;
			r->ServerInfoW2K->fAutoReverseZones = serverinfo->fAutoReverseZones;
			r->ServerInfoW2K->fAutoCacheUpdate = serverinfo->fAutoCacheUpdate;
			r->ServerInfoW2K->fRecurseAfterForwarding = serverinfo->fRecurseAfterForwarding;
			r->ServerInfoW2K->fForwardDelegations = serverinfo->fForwardDelegations;
			r->ServerInfoW2K->fNoRecursion = serverinfo->fNoRecursion;
			r->ServerInfoW2K->fSecureResponses = serverinfo->fSecureResponses;
			r->ServerInfoW2K->fRoundRobin = serverinfo->fRoundRobin;
			r->ServerInfoW2K->fLocalNetPriority = serverinfo->fLocalNetPriority;
			r->ServerInfoW2K->fBindSecondaries = serverinfo->fBindSecondaries;
			r->ServerInfoW2K->fWriteAuthorityNs = serverinfo->fWriteAuthorityNs;
			r->ServerInfoW2K->fStrictFileParsing = serverinfo->fStrictFileParsing;
			r->ServerInfoW2K->fLooseWildcarding = serverinfo->fLooseWildcarding;
			r->ServerInfoW2K->fDefaultAgingState = serverinfo->fDefaultAgingState;

		} else if (client_version == DNS_CLIENT_VERSION_DOTNET) {
			*typeid = DNSSRV_TYPEID_SERVER_INFO_DOTNET;
			r->ServerInfoDotNet = talloc_zero(mem_ctx, struct DNS_RPC_SERVER_INFO_DOTNET);

			r->ServerInfoDotNet->dwRpcStructureVersion = 0x01;
			r->ServerInfoDotNet->dwVersion = serverinfo->dwVersion;
			r->ServerInfoDotNet->fBootMethod = serverinfo->fBootMethod;
			r->ServerInfoDotNet->fAdminConfigured = serverinfo->fAdminConfigured;
			r->ServerInfoDotNet->fAllowUpdate = serverinfo->fAllowUpdate;
			r->ServerInfoDotNet->fDsAvailable = serverinfo->fDsAvailable;
			r->ServerInfoDotNet->pszServerName = talloc_strdup(mem_ctx, serverinfo->pszServerName);
			r->ServerInfoDotNet->pszDsContainer = talloc_strdup(mem_ctx, serverinfo->pszDsContainer);
			r->ServerInfoDotNet->aipServerAddrs = dns_addr_array_to_ip4_array(mem_ctx,
											  serverinfo->aipServerAddrs);
			r->ServerInfoDotNet->aipListenAddrs = dns_addr_array_to_ip4_array(mem_ctx,
											  serverinfo->aipListenAddrs);
			r->ServerInfoDotNet->aipForwarders = ip4_array_copy(mem_ctx, serverinfo->aipForwarders);
			r->ServerInfoDotNet->aipLogFilter = ip4_array_copy(mem_ctx, serverinfo->aipLogFilter);
			r->ServerInfoDotNet->pwszLogFilePath = talloc_strdup(mem_ctx, serverinfo->pwszLogFilePath);
			r->ServerInfoDotNet->pszDomainName = talloc_strdup(mem_ctx, serverinfo->pszDomainName);
			r->ServerInfoDotNet->pszForestName = talloc_strdup(mem_ctx, serverinfo->pszForestName);
			r->ServerInfoDotNet->pszDomainDirectoryPartition = talloc_strdup(mem_ctx, serverinfo->pszDomainDirectoryPartition);
			r->ServerInfoDotNet->pszForestDirectoryPartition = talloc_strdup(mem_ctx, serverinfo->pszForestDirectoryPartition);
			r->ServerInfoDotNet->dwLogLevel = serverinfo->dwLogLevel;
			r->ServerInfoDotNet->dwDebugLevel = serverinfo->dwDebugLevel;
			r->ServerInfoDotNet->dwForwardTimeout = serverinfo->dwForwardTimeout;
			r->ServerInfoDotNet->dwRpcProtocol = serverinfo->dwRpcProtocol;
			r->ServerInfoDotNet->dwNameCheckFlag = serverinfo->dwNameCheckFlag;
			r->ServerInfoDotNet->cAddressAnswerLimit = serverinfo->cAddressAnswerLimit;
			r->ServerInfoDotNet->dwRecursionRetry = serverinfo->dwRecursionRetry;
			r->ServerInfoDotNet->dwRecursionTimeout = serverinfo->dwRecursionTimeout;
			r->ServerInfoDotNet->dwMaxCacheTtl = serverinfo->dwMaxCacheTtl;
			r->ServerInfoDotNet->dwDsPollingInterval = serverinfo->dwDsPollingInterval;
			r->ServerInfoDotNet->dwLocalNetPriorityNetMask = serverinfo->dwLocalNetPriorityNetMask;
			r->ServerInfoDotNet->dwScavengingInterval = serverinfo->dwScavengingInterval;
			r->ServerInfoDotNet->dwDefaultRefreshInterval = serverinfo->dwDefaultRefreshInterval;
			r->ServerInfoDotNet->dwDefaultNoRefreshInterval = serverinfo->dwDefaultNoRefreshInterval;
			r->ServerInfoDotNet->dwLastScavengeTime = serverinfo->dwLastScavengeTime;
			r->ServerInfoDotNet->dwEventLogLevel = serverinfo->dwEventLogLevel;
			r->ServerInfoDotNet->dwLogFileMaxSize = serverinfo->dwLogFileMaxSize;
			r->ServerInfoDotNet->dwDsForestVersion = serverinfo->dwDsForestVersion;
			r->ServerInfoDotNet->dwDsDomainVersion = serverinfo->dwDsDomainVersion;
			r->ServerInfoDotNet->dwDsDsaVersion = serverinfo->dwDsDsaVersion;
			r->ServerInfoDotNet->fAutoReverseZones = serverinfo->fAutoReverseZones;
			r->ServerInfoDotNet->fAutoCacheUpdate = serverinfo->fAutoCacheUpdate;
			r->ServerInfoDotNet->fRecurseAfterForwarding = serverinfo->fRecurseAfterForwarding;
			r->ServerInfoDotNet->fForwardDelegations = serverinfo->fForwardDelegations;
			r->ServerInfoDotNet->fNoRecursion = serverinfo->fNoRecursion;
			r->ServerInfoDotNet->fSecureResponses = serverinfo->fSecureResponses;
			r->ServerInfoDotNet->fRoundRobin = serverinfo->fRoundRobin;
			r->ServerInfoDotNet->fLocalNetPriority = serverinfo->fLocalNetPriority;
			r->ServerInfoDotNet->fBindSecondaries = serverinfo->fBindSecondaries;
			r->ServerInfoDotNet->fWriteAuthorityNs = serverinfo->fWriteAuthorityNs;
			r->ServerInfoDotNet->fStrictFileParsing = serverinfo->fStrictFileParsing;
			r->ServerInfoDotNet->fLooseWildcarding = serverinfo->fLooseWildcarding;
			r->ServerInfoDotNet->fDefaultAgingState = serverinfo->fDefaultAgingState;

		} else if (client_version == DNS_CLIENT_VERSION_LONGHORN) {
			*typeid = DNSSRV_TYPEID_SERVER_INFO;
			r->ServerInfo = talloc_zero(mem_ctx, struct DNS_RPC_SERVER_INFO_LONGHORN);

			r->ServerInfo->dwRpcStructureVersion = 0x02;
			r->ServerInfo->dwVersion = serverinfo->dwVersion;
			r->ServerInfo->fBootMethod = serverinfo->fBootMethod;
			r->ServerInfo->fAdminConfigured = serverinfo->fAdminConfigured;
			r->ServerInfo->fAllowUpdate = serverinfo->fAllowUpdate;
			r->ServerInfo->fDsAvailable = serverinfo->fDsAvailable;
			r->ServerInfo->pszServerName = talloc_strdup(mem_ctx, serverinfo->pszServerName);
			r->ServerInfo->pszDsContainer = talloc_strdup(mem_ctx, serverinfo->pszDsContainer);
			r->ServerInfo->aipServerAddrs = serverinfo->aipServerAddrs;
			r->ServerInfo->aipListenAddrs = serverinfo->aipListenAddrs;
			r->ServerInfo->aipForwarders = ip4_array_to_dns_addr_array(mem_ctx, serverinfo->aipForwarders);
			r->ServerInfo->aipLogFilter = ip4_array_to_dns_addr_array(mem_ctx, serverinfo->aipLogFilter);
			r->ServerInfo->pwszLogFilePath = talloc_strdup(mem_ctx, serverinfo->pwszLogFilePath);
			r->ServerInfo->pszDomainName = talloc_strdup(mem_ctx, serverinfo->pszDomainName);
			r->ServerInfo->pszForestName = talloc_strdup(mem_ctx, serverinfo->pszForestName);
			r->ServerInfo->pszDomainDirectoryPartition = talloc_strdup(mem_ctx, serverinfo->pszDomainDirectoryPartition);
			r->ServerInfo->pszForestDirectoryPartition = talloc_strdup(mem_ctx, serverinfo->pszForestDirectoryPartition);
			r->ServerInfo->dwLogLevel = serverinfo->dwLogLevel;
			r->ServerInfo->dwDebugLevel = serverinfo->dwDebugLevel;
			r->ServerInfo->dwForwardTimeout = serverinfo->dwForwardTimeout;
			r->ServerInfo->dwRpcProtocol = serverinfo->dwRpcProtocol;
			r->ServerInfo->dwNameCheckFlag = serverinfo->dwNameCheckFlag;
			r->ServerInfo->cAddressAnswerLimit = serverinfo->cAddressAnswerLimit;
			r->ServerInfo->dwRecursionRetry = serverinfo->dwRecursionRetry;
			r->ServerInfo->dwRecursionTimeout = serverinfo->dwRecursionTimeout;
			r->ServerInfo->dwMaxCacheTtl = serverinfo->dwMaxCacheTtl;
			r->ServerInfo->dwDsPollingInterval = serverinfo->dwDsPollingInterval;
			r->ServerInfo->dwLocalNetPriorityNetMask = serverinfo->dwLocalNetPriorityNetMask;
			r->ServerInfo->dwScavengingInterval = serverinfo->dwScavengingInterval;
			r->ServerInfo->dwDefaultRefreshInterval = serverinfo->dwDefaultRefreshInterval;
			r->ServerInfo->dwDefaultNoRefreshInterval = serverinfo->dwDefaultNoRefreshInterval;
			r->ServerInfo->dwLastScavengeTime = serverinfo->dwLastScavengeTime;
			r->ServerInfo->dwEventLogLevel = serverinfo->dwEventLogLevel;
			r->ServerInfo->dwLogFileMaxSize = serverinfo->dwLogFileMaxSize;
			r->ServerInfo->dwDsForestVersion = serverinfo->dwDsForestVersion;
			r->ServerInfo->dwDsDomainVersion = serverinfo->dwDsDomainVersion;
			r->ServerInfo->dwDsDsaVersion = serverinfo->dwDsDsaVersion;
			r->ServerInfo->fReadOnlyDC = serverinfo->fReadOnlyDC;
			r->ServerInfo->fAutoReverseZones = serverinfo->fAutoReverseZones;
			r->ServerInfo->fAutoCacheUpdate = serverinfo->fAutoCacheUpdate;
			r->ServerInfo->fRecurseAfterForwarding = serverinfo->fRecurseAfterForwarding;
			r->ServerInfo->fForwardDelegations = serverinfo->fForwardDelegations;
			r->ServerInfo->fNoRecursion = serverinfo->fNoRecursion;
			r->ServerInfo->fSecureResponses = serverinfo->fSecureResponses;
			r->ServerInfo->fRoundRobin = serverinfo->fRoundRobin;
			r->ServerInfo->fLocalNetPriority = serverinfo->fLocalNetPriority;
			r->ServerInfo->fBindSecondaries = serverinfo->fBindSecondaries;
			r->ServerInfo->fWriteAuthorityNs = serverinfo->fWriteAuthorityNs;
			r->ServerInfo->fStrictFileParsing = serverinfo->fStrictFileParsing;
			r->ServerInfo->fLooseWildcarding = serverinfo->fLooseWildcarding;
			r->ServerInfo->fDefaultAgingState = serverinfo->fDefaultAgingState;
		}
		return WERR_OK;
	}

	is_integer = 0;
	answer_integer = 0;

	if (strcasecmp(operation, "AddressAnswerLimit") == 0) {
		answer_integer = serverinfo->cAddressAnswerLimit;
		is_integer = 1;
	} else if (strcasecmp(operation, "AdminConfigured") == 0) {
		answer_integer = serverinfo->fAdminConfigured;
		is_integer = 1;
	} else if (strcasecmp(operation, "AllowCNAMEAtNS") == 0) {
		answer_integer = 0;
		is_integer = 1;
	} else if (strcasecmp(operation, "AllowUpdate") == 0) {
		answer_integer = serverinfo->fAllowUpdate;
		is_integer = 1;
	} else if (strcasecmp(operation, "AutoCacheUpdate") == 0) {
		answer_integer = serverinfo->fAutoCacheUpdate;
		is_integer = 1;
	} else if (strcasecmp(operation, "AutoConfigFileZones") == 0) {
		answer_integer = 1;
		is_integer = 1;
	} else if (strcasecmp(operation, "BindSecondaries") == 0) {
		answer_integer = serverinfo->fBindSecondaries;
		is_integer = 1;
	} else if (strcasecmp(operation, "BootMethod") == 0) {
		answer_integer = serverinfo->fBootMethod;
		is_integer = 1;
	} else if (strcasecmp(operation, "DebugLevel") == 0) {
		answer_integer = serverinfo->dwDebugLevel;
		is_integer = 1;
	} else if (strcasecmp(operation, "DefaultAgingState") == 0) {
		answer_integer = serverinfo->fDefaultAgingState;
		is_integer = 1;
	} else if (strcasecmp(operation, "DefaultNoRefreshInterval") == 0) {
		answer_integer = serverinfo->dwDefaultNoRefreshInterval;
		is_integer = 1;
	} else if (strcasecmp(operation, "DefaultRefreshInterval") == 0) {
		answer_integer = serverinfo->dwDefaultRefreshInterval;
		is_integer = 1;
	} else if (strcasecmp(operation, "DeleteOutsideGlue") == 0) {
		answer_integer = 0;
		is_integer = 1;
	} else if (strcasecmp(operation, "DisjointNets") == 0) {
		answer_integer = 0;
		is_integer = 1;
	} else if (strcasecmp(operation, "DsLazyUpdateInterval") == 0) {
		answer_integer = 3; /* seconds */
		is_integer = 1;
	} else if (strcasecmp(operation, "DsPollingInterval") == 0) {
		answer_integer = serverinfo->dwDsPollingInterval;
		is_integer = 1;
	} else if (strcasecmp(operation, "DsTombstoneInterval") == 0) {
		answer_integer = 0x00127500; /* 14 days */
		is_integer = 1;
	} else if (strcasecmp(operation, "EnableRegistryBoot") == 0) {
		answer_integer = 0;
		is_integer = 1;
	} else if (strcasecmp(operation, "EventLogLevel") == 0) {
		answer_integer = serverinfo->dwEventLogLevel;
		is_integer = 1;
	} else if (strcasecmp(operation, "ForceSoaSerial") == 0) {
		answer_integer = 0;
		is_integer = 1;
	} else if (strcasecmp(operation, "ForceSaoRetry") == 0) {
		answer_integer = 0;
		is_integer = 1;
	} else if (strcasecmp(operation, "ForceSoaRefresh") == 0) {
		answer_integer = 0;
		is_integer = 1;
	} else if (strcasecmp(operation, "ForceSoaMinimumTtl") == 0) {
		answer_integer = 0;
		is_integer = 1;
	} else if (strcasecmp(operation, "ForwardDelegations") == 0) {
		answer_integer = 1;
		is_integer = 1;
	} else if (strcasecmp(operation, "ForwardingTimeout") == 0) {
		answer_integer = serverinfo->dwForwardTimeout;
		is_integer = 1;
	} else if (strcasecmp(operation, "IsSlave") == 0) {
		answer_integer = 0;
		is_integer = 1;
	} else if (strcasecmp(operation, "LocalNetPriority") == 0) {
		answer_integer = serverinfo->fLocalNetPriority;
		is_integer = 1;
	} else if (strcasecmp(operation, "LogFileMaxSize") == 0) {
		answer_integer = serverinfo->dwLogFileMaxSize;
		is_integer = 1;
	} else if (strcasecmp(operation, "LogLevel") == 0) {
		answer_integer = serverinfo->dwLogLevel;
		is_integer = 1;
	} else if (strcasecmp(operation, "LooseWildcarding") == 0) {
		answer_integer = serverinfo->fLooseWildcarding;
		is_integer = 1;
	} else if (strcasecmp(operation, "MaxCacheTtl") == 0) {
		answer_integer = serverinfo->dwMaxCacheTtl;
		is_integer = 1;
	} else if (strcasecmp(operation, "MaxNegativeCacheTtl") == 0) {
		answer_integer = 0x00000384; /* 15 minutes */
		is_integer = 1;
	} else if (strcasecmp(operation, "NameCheckFlag") == 0) {
		answer_integer = serverinfo->dwNameCheckFlag;
		is_integer = 1;
	} else if (strcasecmp(operation, "NoRecursion") == 0) {
		answer_integer = serverinfo->fNoRecursion;
		is_integer = 1;
	} else if (strcasecmp(operation, "NoUpdateDelegations") == 0) {
		answer_integer = 1;
		is_integer = 1;
	} else if (strcasecmp(operation, "PublishAutonet") == 0) {
		answer_integer = 0;
		is_integer = 1;
	} else if (strcasecmp(operation, "QuietRecvFaultInterval") == 0) {
		answer_integer = 0;
		is_integer = 1;
	} else if (strcasecmp(operation, "QuietRecvLogInterval") == 0) {
		answer_integer = 0;
		is_integer = 1;
	} else if (strcasecmp(operation, "RecursionRetry") == 0) {
		answer_integer = serverinfo->dwRecursionRetry;
		is_integer = 1;
	} else if (strcasecmp(operation, "RecursionTimeout") == 0) {
		answer_integer = serverinfo->dwRecursionTimeout;
		is_integer = 1;
	} else if (strcasecmp(operation, "ReloadException") == 0) {
		answer_integer = 0;
		is_integer = 1;
	} else if (strcasecmp(operation, "RoundRobin") == 0) {
		answer_integer = serverinfo->fRoundRobin;
		is_integer = 1;
	} else if (strcasecmp(operation, "RpcProtocol") == 0) {
		answer_integer = serverinfo->dwRpcProtocol;
		is_integer = 1;
	} else if (strcasecmp(operation, "SecureResponses") == 0) {
		answer_integer = serverinfo->fSecureResponses;
		is_integer = 1;
	} else if (strcasecmp(operation, "SendPort") == 0) {
		answer_integer = 0;
		is_integer = 1;
	} else if (strcasecmp(operation, "ScavengingInterval") == 0) {
		answer_integer = serverinfo->dwScavengingInterval;
		is_integer = 1;
	} else if (strcasecmp(operation, "SocketPoolSize") == 0) {
		answer_integer = 0x000009C4;
		is_integer = 1;
	} else if (strcasecmp(operation, "StrictFileParsing") == 0) {
		answer_integer = serverinfo->fStrictFileParsing;
		is_integer = 1;
	} else if (strcasecmp(operation, "SyncDnsZoneSerial") == 0) {
		answer_integer = 2; /* ZONE_SERIAL_SYNC_XFER */
		is_integer = 1;
	} else if (strcasecmp(operation, "UpdateOptions") == 0) {
		answer_integer = 0x0000030F; /* DNS_DEFAULT_UPDATE_OPTIONS */
		is_integer = 1;
	} else if (strcasecmp(operation, "UseSystemEvengLog") == 0) {
		answer_integer = 0;
		is_integer = 1;
	} else if (strcasecmp(operation, "Version") == 0) {
		answer_integer = serverinfo->dwVersion;
		is_integer = 1;
	} else if (strcasecmp(operation, "XfrConnectTimeout") == 0) {
		answer_integer = 0x0000001E;
		is_integer = 1;
	} else if (strcasecmp(operation, "WriteAuthorityNs") == 0) {
		answer_integer = serverinfo->fWriteAuthorityNs;
		is_integer = 1;
	} else if (strcasecmp(operation, "AdditionalRecursionTimeout") == 0) {
		answer_integer = 0x00000004;
		is_integer = 1;
	} else if (strcasecmp(operation, "AppendMsZoneTransferFlag") == 0) {
		answer_integer = 0;
		is_integer = 1;
	} else if (strcasecmp(operation, "AutoCreateDelegations") == 0) {
		answer_integer = 0; /* DNS_ACD_DONT_CREATE */
		is_integer = 1;
	} else if (strcasecmp(operation, "BreakOnAscFailure") == 0) {
		answer_integer = 0;
		is_integer = 1;
	} else if (strcasecmp(operation, "CacheEmptyAuthResponses") == 0) {
		answer_integer = 0;
		is_integer = 1;
	} else if (strcasecmp(operation, "DirectoryPartitionAutoEnlistInterval") == 0) {
		answer_integer = 0x00015180; /* 1 day */
		is_integer = 1;
	} else if (strcasecmp(operation, "DisableAutoReverseZones") == 0) {
		answer_integer = ~serverinfo->fAutoReverseZones;
		is_integer = 1;
	} else if (strcasecmp(operation, "EDnsCacheTimeout") == 0) {
		answer_integer = 0x00000384; /* 15 minutes */
		is_integer = 1;
	} else if (strcasecmp(operation, "EnableDirectoryPartitions") == 0) {
		answer_integer = serverinfo->fDsAvailable;
		is_integer = 1;
	} else if (strcasecmp(operation, "EnableDnsSec") == 0) {
		answer_integer = 0;
		is_integer = 1;
	} else if (strcasecmp(operation, "EnableEDnsProbes") == 0) {
		answer_integer = 0;
		is_integer = 1;
	} else if (strcasecmp(operation, "EnableEDnsReception") == 0) {
		answer_integer = 0;
		is_integer = 1;
	} else if (strcasecmp(operation, "EnableIPv6") == 0) {
		answer_integer = 0;
		is_integer = 1;
	} else if (strcasecmp(operation, "EnableIQueryResponseGeneration") == 0) {
		answer_integer = 0;
		is_integer = 1;
	} else if (strcasecmp(operation, "EnableSendErrorSuppression") == 0) {
		answer_integer = 0;
		is_integer = 1;
	} else if (strcasecmp(operation, "EnableUpdateForwarding") == 0) {
		answer_integer = 0;
		is_integer = 1;
	} else if (strcasecmp(operation, "EnableWinsR") == 0) {
		answer_integer = 0;
		is_integer = 1;
	} else if (strcasecmp(operation, "ForceDsaBehaviorVersion") == 0) {
		answer_integer = serverinfo->dwDsDsaVersion;
		is_integer = 1;
	} else if (strcasecmp(operation, "ForceDomainBehaviorVersion") == 0) {
		answer_integer = serverinfo->dwDsDsaVersion;
		is_integer = 1;
	} else if (strcasecmp(operation, "ForceForestBehaviorVersion") == 0) {
		answer_integer = serverinfo->dwDsDsaVersion;
		is_integer = 1;
	} else if (strcasecmp(operation, "HeapDebug") == 0) {
		answer_integer = 0;
		is_integer = 1;
	} else if (strcasecmp(operation, "LameDelegationTtl") == 0) {
		answer_integer = 0; /* seconds */
		is_integer = 1;
	} else if (strcasecmp(operation, "LocalNetPriorityNetMask") == 0) {
		answer_integer = serverinfo->dwLocalNetPriorityNetMask;
		is_integer = 1;
	} else if (strcasecmp(operation, "MaxCacheSize") == 0) {
		answer_integer = 0;
		is_integer = 1;
	} else if (strcasecmp(operation, "MaxResourceRecordsInNonSecureUpdate") == 0) {
		answer_integer = 0x0000001E;
		is_integer = 1;
	} else if (strcasecmp(operation, "OperationsLogLevel") == 0) {
		answer_integer = 0;
		is_integer = 1;
	} else if (strcasecmp(operation, "OperationsLogLevel2") == 0) {
		answer_integer = 0;
		is_integer = 1;
	} else if (strcasecmp(operation, "MaximumUdpPacketSize") == 0) {
		answer_integer = 0x00004000; /* maximum possible */
		is_integer = 1;
	} else if (strcasecmp(operation, "RecurseToInternetRootMask") == 0) {
		answer_integer = 0;
		is_integer = 1;
	} else if (strcasecmp(operation, "SelfTest") == 0) {
		answer_integer = 0;
		is_integer = 1;
	} else if (strcasecmp(operation, "SilentlyIgnoreCNameUpdateConflicts") == 0) {
		answer_integer = 1;
		is_integer = 1;
	} else if (strcasecmp(operation, "TcpReceivePacketSize") == 0) {
		answer_integer = 0x00010000;
		is_integer = 1;
	} else if (strcasecmp(operation, "XfrThrottleMultiplier") == 0) {
		answer_integer = 0x0000000A;
		is_integer = 1;
	} else if (strcasecmp(operation, "AllowMsdcsLookupRetry") == 0) {
		answer_integer = 1;
		is_integer = 1;
	} else if (strcasecmp(operation, "AllowReadOnlyZoneTransfer") == 0) {
		answer_integer = 0;
		is_integer = 1;
	} else if (strcasecmp(operation, "DsBackGroundLoadPaused") == 0) {
		answer_integer = 0;
		is_integer = 1;
	} else if (strcasecmp(operation, "DsMinimumBackgroundLoadThreads") == 0) {
		answer_integer = 0;
		is_integer = 1;
	} else if (strcasecmp(operation, "DsRemoteReplicationDelay") == 0) {
		answer_integer = 0x0000001E; /* 30 seconds */
		is_integer = 1;
	} else if (strcasecmp(operation, "EnableDuplicateQuerySuppresion") == 0) {
		answer_integer = 0;
		is_integer = 1;
	} else if (strcasecmp(operation, "EnableGlobalNamesSupport") == 0) {
		answer_integer = 0;
		is_integer = 1;
	} else if (strcasecmp(operation, "EnableVersionQuery") == 0) {
		answer_integer = 1; /* DNS_VERSION_QUERY_FULL */
		is_integer = 1;
	} else if (strcasecmp(operation, "EnableRsoForRodc") == 0) {
		answer_integer = 0;
		is_integer = 1;
	} else if (strcasecmp(operation, "ForceRODCMode") == 0) {
		answer_integer = 0;
		is_integer = 1;
	} else if (strcasecmp(operation, "GlobalNamesAlwaysQuerySrv") == 0) {
		answer_integer = 1;
		is_integer = 1;
	} else if (strcasecmp(operation, "GlobalNamesBlockUpdates") == 0) {
		answer_integer = 0;
		is_integer = 1;
	} else if (strcasecmp(operation, "GlobalNamesEnableEDnsProbes") == 0) {
		answer_integer = 0;
		is_integer = 1;
	} else if (strcasecmp(operation, "GlobalNamesPreferAAAA") == 0) {
		answer_integer = 0;
		is_integer = 1;
	} else if (strcasecmp(operation, "GlobalNamesQueryOrder") == 0) {
		answer_integer = 1;
		is_integer = 1;
	} else if (strcasecmp(operation, "GlobalNamesSendTimeout") == 0) {
		answer_integer = 3; /* seconds */
		is_integer = 1;
	} else if (strcasecmp(operation, "GlobalNamesServerQueryInterval") == 0) {
		answer_integer = 0x00005460; /* 6 hours */
		is_integer = 1;
	} else if (strcasecmp(operation, "RemoteIPv4RankBoost") == 0) {
		answer_integer = 0;
		is_integer = 1;
	} else if (strcasecmp(operation, "RemoteIPv6RankBoost") == 0) {
		answer_integer = 0;
		is_integer = 1;
	} else if (strcasecmp(operation, "MaximumRodcRsoAttemptsPerCycle") == 0) {
		answer_integer = 0x00000064;
		is_integer = 1;
	} else if (strcasecmp(operation, "MaximumRodcRsoQueueLength") == 0) {
		answer_integer = 0x0000012C;
		is_integer = 1;
	} else if (strcasecmp(operation, "EnableGlobalQueryBlockList") == 0) {
		answer_integer = 0;
		is_integer = 1;
	} else if (strcasecmp(operation, "OpenACLOnProxyUpdates") == 0) {
		answer_integer = 0;
		is_integer = 1;
	} else if (strcasecmp(operation, "CacheLockingPercent") == 0) {
		answer_integer = 0x00000064;
		is_integer = 1;
	}

	if (is_integer == 1) {
		*typeid = DNSSRV_TYPEID_DWORD;
		r->Dword = answer_integer;
		return WERR_OK;
	}

	is_addresses = 0;

	if (strcasecmp(operation, "Forwarders") == 0) {
		if (client_version == DNS_CLIENT_VERSION_LONGHORN) {
			answer_addrarray = ip4_array_to_dns_addr_array(mem_ctx, serverinfo->aipForwarders);
		} else {
			answer_iparray = ip4_array_copy(mem_ctx, serverinfo->aipForwarders);
		}
		is_addresses = 1;
	} else if (strcasecmp(operation, "ListenAddresses") == 0) {
		if (client_version == DNS_CLIENT_VERSION_LONGHORN) {
			answer_addrarray = serverinfo->aipListenAddrs;
		} else {
			answer_iparray = dns_addr_array_to_ip4_array(mem_ctx, serverinfo->aipListenAddrs);
		}
		is_addresses = 1;
	} else if (strcasecmp(operation, "BreakOnReceiveFrom") == 0) {
		if (client_version == DNS_CLIENT_VERSION_LONGHORN) {
			answer_addrarray = NULL;
		} else {
			answer_iparray = NULL;
		}
		is_addresses = 1;
	} else if (strcasecmp(operation, "BreakOnUpdateFrom") == 0) {
		if (client_version == DNS_CLIENT_VERSION_LONGHORN) {
			answer_addrarray = NULL;
		} else {
			answer_iparray = NULL;
		}
		is_addresses = 1;
	} else if (strcasecmp(operation, "LogIPFilterList") == 0) {
		if (client_version == DNS_CLIENT_VERSION_LONGHORN) {
			answer_addrarray = ip4_array_to_dns_addr_array(mem_ctx, serverinfo->aipLogFilter);
		} else {
			answer_iparray = ip4_array_copy(mem_ctx, serverinfo->aipLogFilter);
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

	is_string = is_wstring = 0;

	if (strcasecmp(operation, "DomainDirectoryPartitionBaseName") == 0) {
		answer_string = talloc_strdup(mem_ctx, "DomainDnsZones");
		if (! answer_string) {
			return WERR_OUTOFMEMORY;
		}
		is_string = 1;
	} else if (strcasecmp(operation, "ForestDirectoryPartitionBaseName") == 0) {
		answer_string = talloc_strdup(mem_ctx, "ForestDnsZones");
		if (! answer_string) {
			return WERR_OUTOFMEMORY;
		}
		is_string = 1;
	} else if (strcasecmp(operation, "LogFilePath") == 0) {
		answer_string = talloc_strdup(mem_ctx, serverinfo->pwszLogFilePath);
		is_wstring = 1;
	} else if (strcasecmp(operation, "ServerLevelPluginDll") == 0) {
		answer_string = NULL;
		is_wstring = 1;
	} else if (strcasecmp(operation, "DsBackgroundPauseName") == 0) {
		answer_string = NULL;
		is_string = 1;
	} else if (strcasecmp(operation, "DsNotRoundRobinTypes") == 0) {
		answer_string = NULL;
		is_string = 1;
	}

	if (is_string == 1) {
		*typeid = DNSSRV_TYPEID_LPSTR;
		r->String = answer_string;
		return WERR_OK;
	} else if (is_wstring == 1) {
		*typeid = DNSSRV_TYPEID_LPWSTR;
		r->WideString = answer_string;
		return WERR_OK;
	}

	is_stringlist = 0;

	if (strcasecmp(operation, "GlobalQueryBlockList") == 0) {
		answer_stringlist = NULL;
		is_stringlist = 1;
	} else if (strcasecmp(operation, "SocketPoolExcludedPortRanges") == 0) {
		answer_stringlist = NULL;
		is_stringlist = 1;
	}

	if (is_stringlist == 1) {
		*typeid = DNSSRV_TYPEID_UTF8_STRING_LIST;
		r->Utf8StringList = answer_stringlist;
		return WERR_OK;
	}

	DEBUG(0,("dnsserver: Invalid server operation %s", operation));
	return WERR_DNS_ERROR_INVALID_PROPERTY;
}