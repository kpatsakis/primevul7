static WERROR dnsserver_complex_operate_server(struct dnsserver_state *dsstate,
					TALLOC_CTX *mem_ctx,
					const char *operation,
					const unsigned int client_version,
					enum DNS_RPC_TYPEID typeid_in,
					union DNSSRV_RPC_UNION *rin,
					enum DNS_RPC_TYPEID *typeid_out,
					union DNSSRV_RPC_UNION *rout)
{
	int valid_operation = 0;
	struct dnsserver_zone *z, **zlist;
	size_t zcount;
	bool found1, found2, found3, found4;
	size_t i;

	if (strcasecmp(operation, "QueryDwordProperty") == 0) {
		if (typeid_in == DNSSRV_TYPEID_LPSTR) {
			return dnsserver_query_server(dsstate, mem_ctx,
							rin->String,
							client_version,
							typeid_out,
							rout);
		}
	} else if (strcasecmp(operation, "EnumZones") == 0) {
		if (typeid_in != DNSSRV_TYPEID_DWORD) {
			return WERR_DNS_ERROR_INVALID_PROPERTY;
		}

		zcount = 0;
		zlist = talloc_zero_array(mem_ctx, struct dnsserver_zone *, 0);
		for (z = dsstate->zones; z; z = z->next) {

			/* Match the flags in groups
			 *
			 * Group1 : PRIMARY, SECONDARY, CACHE, AUTO
			 * Group2 : FORWARD, REVERSE, FORWARDER, STUB
			 * Group3 : DS, NON_DS, DOMAIN_DP, FOREST_DP
			 * Group4 : CUSTOM_DP, LEGACY_DP
			 */
			
			/* Group 1 */
			found1 = false;
			if (rin->Dword & 0x0000000f) {
				if (rin->Dword & DNS_ZONE_REQUEST_PRIMARY) {
					if (z->zoneinfo->dwZoneType == DNS_ZONE_TYPE_PRIMARY) {
					found1 = true;
					}
				}
				if (rin->Dword & DNS_ZONE_REQUEST_SECONDARY) {
					if (z->zoneinfo->dwZoneType == DNS_ZONE_TYPE_SECONDARY) {
						found1 = true;
					}
				}
				if (rin->Dword & DNS_ZONE_REQUEST_CACHE) {
					if (z->zoneinfo->dwZoneType == DNS_ZONE_TYPE_CACHE) {
						found1 = true;
					}
				}
				if (rin->Dword & DNS_ZONE_REQUEST_AUTO) {
					if (z->zoneinfo->fAutoCreated 
						|| z->partition->dwDpFlags & DNS_DP_AUTOCREATED) {
						found1 = true;
					}
				}
			} else {
				found1 = true;
			}

			/* Group 2 */
			found2 = false;
			if (rin->Dword & 0x000000f0) {
				if (rin->Dword & DNS_ZONE_REQUEST_FORWARD) {
					if (!(z->zoneinfo->fReverse)) {
						found2 = true;
					}
				}
				if (rin->Dword & DNS_ZONE_REQUEST_REVERSE) {
					if (z->zoneinfo->fReverse) {
						found2 = true;
					}
				}
				if (rin->Dword & DNS_ZONE_REQUEST_FORWARDER) {
					if (z->zoneinfo->dwZoneType == DNS_ZONE_TYPE_FORWARDER) {
						found2 = true;
					}
				}
				if (rin->Dword & DNS_ZONE_REQUEST_STUB) {
					if (z->zoneinfo->dwZoneType == DNS_ZONE_TYPE_STUB) {
						found2 = true;
					}
				}
			} else {
				found2 = true;
			}

			/* Group 3 */
			found3 = false;
			if (rin->Dword & 0x00000f00) {
				if (rin->Dword & DNS_ZONE_REQUEST_DS) {
					if (z->zoneinfo->Flags & DNS_RPC_ZONE_DSINTEGRATED) {
						found3 = true;
					}
				}
				if (rin->Dword & DNS_ZONE_REQUEST_NON_DS) {
					if (!(z->zoneinfo->Flags & DNS_RPC_ZONE_DSINTEGRATED)) {
						found3 = true;
					}
				}
				if (rin->Dword & DNS_ZONE_REQUEST_DOMAIN_DP) {
					if (!(z->partition->dwDpFlags & DNS_DP_DOMAIN_DEFAULT)) {
						found3 = true;
					}
				}
				if (rin->Dword & DNS_ZONE_REQUEST_FOREST_DP) {
					if (!(z->partition->dwDpFlags & DNS_DP_FOREST_DEFAULT)) {
						found3 = true;
					}
				}
			} else {
				found3 = true;
			}
	
			/* Group 4 */
			if (rin->Dword & 0x0000f000) {
				found4 = false;
			} else {
				found4 = true;
			}

			if (found1 && found2 && found3 && found4) {
				zlist = talloc_realloc(mem_ctx, zlist, struct dnsserver_zone *, zcount+1);
				zlist[zcount] = z;
				zcount++;
			}
		}

		if (client_version == DNS_CLIENT_VERSION_W2K) {
			*typeid_out = DNSSRV_TYPEID_ZONE_LIST_W2K;
			rout->ZoneListW2K = talloc_zero(mem_ctx, struct DNS_RPC_ZONE_LIST_W2K);

			if (zcount == 0) {
				rout->ZoneListW2K->dwZoneCount = 0;
				rout->ZoneListW2K->ZoneArray = NULL;

				return WERR_OK;
			}

			rout->ZoneListW2K->ZoneArray = talloc_zero_array(mem_ctx, struct DNS_RPC_ZONE_W2K *, zcount);
			W_ERROR_HAVE_NO_MEMORY_AND_FREE(rout->ZoneListW2K->ZoneArray, zlist);

			for (i=0; i<zcount; i++) {
				rout->ZoneListW2K->ZoneArray[i] = talloc_zero(mem_ctx, struct DNS_RPC_ZONE_W2K);

				rout->ZoneListW2K->ZoneArray[i]->pszZoneName = talloc_strdup(mem_ctx, zlist[i]->name);
				rout->ZoneListW2K->ZoneArray[i]->Flags = zlist[i]->zoneinfo->Flags;
				rout->ZoneListW2K->ZoneArray[i]->ZoneType = zlist[i]->zoneinfo->dwZoneType;
				rout->ZoneListW2K->ZoneArray[i]->Version = zlist[i]->zoneinfo->Version;
			}
			rout->ZoneListW2K->dwZoneCount = zcount;

		} else {
			*typeid_out = DNSSRV_TYPEID_ZONE_LIST;
			rout->ZoneList = talloc_zero(mem_ctx, struct DNS_RPC_ZONE_LIST_DOTNET);

			if (zcount == 0) {
				rout->ZoneList->dwRpcStructureVersion = 1;
				rout->ZoneList->dwZoneCount = 0;
				rout->ZoneList->ZoneArray = NULL;

				return WERR_OK;
			}

			rout->ZoneList->ZoneArray = talloc_zero_array(mem_ctx, struct DNS_RPC_ZONE_DOTNET *, zcount);
			W_ERROR_HAVE_NO_MEMORY_AND_FREE(rout->ZoneList->ZoneArray, zlist);

			for (i=0; i<zcount; i++) {
				rout->ZoneList->ZoneArray[i] = talloc_zero(mem_ctx, struct DNS_RPC_ZONE_DOTNET);

				rout->ZoneList->ZoneArray[i]->dwRpcStructureVersion = 1;
				rout->ZoneList->ZoneArray[i]->pszZoneName = talloc_strdup(mem_ctx, zlist[i]->name);
				rout->ZoneList->ZoneArray[i]->Flags = zlist[i]->zoneinfo->Flags;
				rout->ZoneList->ZoneArray[i]->ZoneType = zlist[i]->zoneinfo->dwZoneType;
				rout->ZoneList->ZoneArray[i]->Version = zlist[i]->zoneinfo->Version;
				rout->ZoneList->ZoneArray[i]->dwDpFlags = zlist[i]->partition->dwDpFlags;
				rout->ZoneList->ZoneArray[i]->pszDpFqdn = talloc_strdup(mem_ctx, zlist[i]->partition->pszDpFqdn);
			}
			rout->ZoneList->dwRpcStructureVersion = 1;
			rout->ZoneList->dwZoneCount = zcount;
		}
		talloc_free(zlist);
		return WERR_OK;
	} else if (strcasecmp(operation, "EnumZones2") == 0) {
		valid_operation = true;
	} else if (strcasecmp(operation, "EnumDirectoryPartitions") == 0) {
		if (typeid_in != DNSSRV_TYPEID_DWORD) {
			return WERR_DNS_ERROR_INVALID_PROPERTY;
		}

		*typeid_out = DNSSRV_TYPEID_DP_LIST;
		rout->DirectoryPartitionList = talloc_zero(mem_ctx, struct DNS_RPC_DP_LIST);

		if (rin->Dword != 0) {
			rout->DirectoryPartitionList->dwDpCount = 0;
			rout->DirectoryPartitionList->DpArray = NULL;
		} else {
			struct DNS_RPC_DP_ENUM **dplist;
			struct dnsserver_partition *p;
			int pcount = 2;

			dplist = talloc_zero_array(mem_ctx, struct DNS_RPC_DP_ENUM *, pcount);
			W_ERROR_HAVE_NO_MEMORY(dplist);

			p = dsstate->partitions;
			for (i=0; i<pcount; i++) {
				dplist[i] = talloc_zero(dplist, struct DNS_RPC_DP_ENUM);

				dplist[i]->pszDpFqdn = talloc_strdup(mem_ctx, p->pszDpFqdn);
				dplist[i]->dwFlags = p->dwDpFlags;
				dplist[i]->dwZoneCount = p->zones_count;
				p = p->next;
			}

			rout->DirectoryPartitionList->dwDpCount = pcount;
			rout->DirectoryPartitionList->DpArray = dplist;
		}
		return WERR_OK;
	} else if (strcasecmp(operation, "DirectoryPartitionInfo") == 0) {
		struct dnsserver_partition *p;
		struct dnsserver_partition_info *partinfo;
		struct DNS_RPC_DP_INFO *dpinfo = NULL;

		if (typeid_in != DNSSRV_TYPEID_LPSTR) {
			return WERR_DNS_ERROR_INVALID_PROPERTY;
		}

		*typeid_out = DNSSRV_TYPEID_DP_INFO;

		for (p = dsstate->partitions; p; p = p->next) {
			if (strcasecmp(p->pszDpFqdn, rin->String) == 0) {
				dpinfo = talloc_zero(mem_ctx, struct DNS_RPC_DP_INFO);
				W_ERROR_HAVE_NO_MEMORY(dpinfo);

				partinfo = dnsserver_db_partition_info(mem_ctx, dsstate->samdb, p);
				W_ERROR_HAVE_NO_MEMORY(partinfo);

				dpinfo->pszDpFqdn = talloc_strdup(dpinfo, p->pszDpFqdn);
				dpinfo->pszDpDn = talloc_strdup(dpinfo, ldb_dn_get_linearized(p->partition_dn));
				dpinfo->pszCrDn = talloc_steal(dpinfo, partinfo->pszCrDn);
				dpinfo->dwFlags = p->dwDpFlags;
				dpinfo->dwZoneCount = p->zones_count;
				dpinfo->dwState = partinfo->dwState;
				dpinfo->dwReplicaCount = partinfo->dwReplicaCount;
				if (partinfo->dwReplicaCount > 0) {
					dpinfo->ReplicaArray = talloc_steal(dpinfo,
									    partinfo->ReplicaArray);
				} else {
					dpinfo->ReplicaArray = NULL;
				}
				break;
			}
		}

		if (dpinfo == NULL) {
			return WERR_DNS_ERROR_DP_DOES_NOT_EXIST;
		}

		rout->DirectoryPartition = dpinfo;
		return WERR_OK;
	} else if (strcasecmp(operation, "Statistics") == 0) {
		valid_operation = true;
	} else if (strcasecmp(operation, "IpValidate") == 0) {
		valid_operation = true;
	}

	if (valid_operation) {
		DEBUG(0, ("dnsserver: server complex operation '%s' not implemented", operation));
		return WERR_CALL_NOT_IMPLEMENTED;
	}

	DEBUG(0, ("dnsserver: invalid server complex operation '%s'", operation));
	return WERR_DNS_ERROR_INVALID_PROPERTY;
}