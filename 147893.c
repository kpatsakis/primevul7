snmpInit(void)
{
    debugs(49, 5, "snmpInit: Building SNMP mib tree structure");

    snmplib_debug_hook = snmpSnmplibDebug;

    /*
     * This following bit of evil is to get the final node in the "squid" mib
     * without having a "search" function. A search function should be written
     * to make this and the other code much less evil.
     */
    mib_tree_head = snmpAddNode(snmpCreateOid(1, 1), 1, NULL, NULL, atNone, 0);

    assert(mib_tree_head);
    debugs(49, 5, "snmpInit: root is " << mib_tree_head);
    snmpAddNodeStr("1", 3, NULL, NULL);

    snmpAddNodeStr("1.3", 6, NULL, NULL);

    snmpAddNodeStr("1.3.6", 1, NULL, NULL);
    snmpAddNodeStr("1.3.6.1", 4, NULL, NULL);
    snmpAddNodeStr("1.3.6.1.4", 1, NULL, NULL);
    snmpAddNodeStr("1.3.6.1.4.1", 3495, NULL, NULL);
    mib_tree_entry *m2 = snmpAddNodeStr("1.3.6.1.4.1.3495", 1, NULL, NULL);

    mib_tree_entry *n = snmpLookupNodeStr(NULL, "1.3.6.1.4.1.3495.1");
    assert(m2 == n);

    /* SQ_SYS - 1.3.6.1.4.1.3495.1.1 */
    snmpAddNodeStr("1.3.6.1.4.1.3495.1", 1, NULL, NULL);
    snmpAddNodeStr("1.3.6.1.4.1.3495.1.1", SYSVMSIZ, snmp_sysFn, static_Inst, atSum);
    snmpAddNodeStr("1.3.6.1.4.1.3495.1.1", SYSSTOR, snmp_sysFn, static_Inst, atSum);
    snmpAddNodeStr("1.3.6.1.4.1.3495.1.1", SYS_UPTIME, snmp_sysFn, static_Inst, atMax);

    /* SQ_CONF - 1.3.6.1.4.1.3495.1.2 */
    snmpAddNodeStr("1.3.6.1.4.1.3495.1", 2, NULL, NULL);
    snmpAddNodeStr("1.3.6.1.4.1.3495.1.2", CONF_ADMIN, snmp_confFn, static_Inst);
    snmpAddNodeStr("1.3.6.1.4.1.3495.1.2", CONF_VERSION, snmp_confFn, static_Inst);
    snmpAddNodeStr("1.3.6.1.4.1.3495.1.2", CONF_VERSION_ID, snmp_confFn, static_Inst);
    snmpAddNodeStr("1.3.6.1.4.1.3495.1.2", CONF_LOG_FAC, snmp_confFn, static_Inst);

    /* SQ_CONF + CONF_STORAGE - 1.3.6.1.4.1.3495.1.5 */
    snmpAddNodeStr("1.3.6.1.4.1.3495.1.2", CONF_STORAGE, NULL, NULL);
    snmpAddNodeStr("1.3.6.1.4.1.3495.1.2.5", CONF_ST_MMAXSZ, snmp_confFn, static_Inst, atSum);
    snmpAddNodeStr("1.3.6.1.4.1.3495.1.2.5", CONF_ST_SWMAXSZ, snmp_confFn, static_Inst, atSum);
    snmpAddNodeStr("1.3.6.1.4.1.3495.1.2.5", CONF_ST_SWHIWM, snmp_confFn, static_Inst, atMin);
    snmpAddNodeStr("1.3.6.1.4.1.3495.1.2.5", CONF_ST_SWLOWM, snmp_confFn, static_Inst, atMin);

    snmpAddNodeStr("1.3.6.1.4.1.3495.1.2", CONF_UNIQNAME, snmp_confFn, static_Inst);

    /* SQ_PRF - 1.3.6.1.4.1.3495.1.3 */
    snmpAddNodeStr("1.3.6.1.4.1.3495.1", 3, NULL, NULL);                    /* SQ_PRF */

    /* PERF_SYS - 1.3.6.1.4.1.3495.1.3.1 */
    snmpAddNodeStr("1.3.6.1.4.1.3495.1.3", PERF_SYS, NULL, NULL);
    snmpAddNodeStr("1.3.6.1.4.1.3495.1.3.1", PERF_SYS_PF, snmp_prfSysFn, static_Inst, atSum);
    snmpAddNodeStr("1.3.6.1.4.1.3495.1.3.1", PERF_SYS_NUMR, snmp_prfSysFn, static_Inst, atSum);
    snmpAddNodeStr("1.3.6.1.4.1.3495.1.3.1", PERF_SYS_MEMUSAGE, snmp_prfSysFn, static_Inst, atSum);
    snmpAddNodeStr("1.3.6.1.4.1.3495.1.3.1", PERF_SYS_CPUTIME, snmp_prfSysFn, static_Inst, atSum);
    snmpAddNodeStr("1.3.6.1.4.1.3495.1.3.1", PERF_SYS_CPUUSAGE, snmp_prfSysFn, static_Inst, atSum);
    snmpAddNodeStr("1.3.6.1.4.1.3495.1.3.1", PERF_SYS_MAXRESSZ, snmp_prfSysFn, static_Inst, atSum);
    snmpAddNodeStr("1.3.6.1.4.1.3495.1.3.1", PERF_SYS_NUMOBJCNT, snmp_prfSysFn, static_Inst, atSum);
    /*
      Amos comments:
      The meaning of LRU is "oldest timestamped object in cache,  if LRU algorithm is
      used"...
      What this SMP support needs to do is aggregate via a special filter equivalent to
      min() to retain the semantic oldest-object meaning. A special one is needed that
      works as unsigned and ignores '0' values.
     */
    snmpAddNodeStr("1.3.6.1.4.1.3495.1.3.1", PERF_SYS_CURLRUEXP, snmp_prfSysFn, static_Inst);
    snmpAddNodeStr("1.3.6.1.4.1.3495.1.3.1", PERF_SYS_CURUNLREQ, snmp_prfSysFn, static_Inst, atSum);
    snmpAddNodeStr("1.3.6.1.4.1.3495.1.3.1", PERF_SYS_CURUNUSED_FD, snmp_prfSysFn, static_Inst, atSum);
    snmpAddNodeStr("1.3.6.1.4.1.3495.1.3.1", PERF_SYS_CURRESERVED_FD, snmp_prfSysFn, static_Inst, atSum);
    snmpAddNodeStr("1.3.6.1.4.1.3495.1.3.1", PERF_SYS_CURUSED_FD, snmp_prfSysFn, static_Inst, atSum);
    snmpAddNodeStr("1.3.6.1.4.1.3495.1.3.1", PERF_SYS_CURMAX_FD, snmp_prfSysFn, static_Inst, atMax);

    /* PERF_PROTO - 1.3.6.1.4.1.3495.1.3.2 */
    snmpAddNodeStr("1.3.6.1.4.1.3495.1.3", PERF_PROTO, NULL, NULL);
    snmpAddNodeStr("1.3.6.1.4.1.3495.1.3.2", PERF_PROTOSTAT_AGGR, NULL, NULL);
    snmpAddNodeStr("1.3.6.1.4.1.3495.1.3.2.1", PERF_PROTOSTAT_AGGR_HTTP_REQ, snmp_prfProtoFn, static_Inst, atSum);
    snmpAddNodeStr("1.3.6.1.4.1.3495.1.3.2.1", PERF_PROTOSTAT_AGGR_HTTP_HITS, snmp_prfProtoFn, static_Inst, atSum);
    snmpAddNodeStr("1.3.6.1.4.1.3495.1.3.2.1", PERF_PROTOSTAT_AGGR_HTTP_ERRORS, snmp_prfProtoFn, static_Inst, atSum);
    snmpAddNodeStr("1.3.6.1.4.1.3495.1.3.2.1", PERF_PROTOSTAT_AGGR_HTTP_KBYTES_IN, snmp_prfProtoFn, static_Inst, atSum);
    snmpAddNodeStr("1.3.6.1.4.1.3495.1.3.2.1", PERF_PROTOSTAT_AGGR_HTTP_KBYTES_OUT, snmp_prfProtoFn, static_Inst, atSum);
    snmpAddNodeStr("1.3.6.1.4.1.3495.1.3.2.1", PERF_PROTOSTAT_AGGR_ICP_S, snmp_prfProtoFn, static_Inst, atSum);
    snmpAddNodeStr("1.3.6.1.4.1.3495.1.3.2.1", PERF_PROTOSTAT_AGGR_ICP_R, snmp_prfProtoFn, static_Inst, atSum);
    snmpAddNodeStr("1.3.6.1.4.1.3495.1.3.2.1", PERF_PROTOSTAT_AGGR_ICP_SKB, snmp_prfProtoFn, static_Inst, atSum);
    snmpAddNodeStr("1.3.6.1.4.1.3495.1.3.2.1", PERF_PROTOSTAT_AGGR_ICP_RKB, snmp_prfProtoFn, static_Inst, atSum);
    snmpAddNodeStr("1.3.6.1.4.1.3495.1.3.2.1", PERF_PROTOSTAT_AGGR_REQ, snmp_prfProtoFn, static_Inst, atSum);
    snmpAddNodeStr("1.3.6.1.4.1.3495.1.3.2.1", PERF_PROTOSTAT_AGGR_ERRORS, snmp_prfProtoFn, static_Inst, atSum);
    snmpAddNodeStr("1.3.6.1.4.1.3495.1.3.2.1", PERF_PROTOSTAT_AGGR_KBYTES_IN, snmp_prfProtoFn, static_Inst, atSum);
    snmpAddNodeStr("1.3.6.1.4.1.3495.1.3.2.1", PERF_PROTOSTAT_AGGR_KBYTES_OUT, snmp_prfProtoFn, static_Inst, atSum);
    snmpAddNodeStr("1.3.6.1.4.1.3495.1.3.2.1", PERF_PROTOSTAT_AGGR_CURSWAP, snmp_prfProtoFn, static_Inst, atSum);
    snmpAddNodeStr("1.3.6.1.4.1.3495.1.3.2.1", PERF_PROTOSTAT_AGGR_CLIENTS, snmp_prfProtoFn, static_Inst, atSum);

    /* Note this is time-series rather than 'static' */
    /* cacheMedianSvcTable */
    snmpAddNodeStr("1.3.6.1.4.1.3495.1.3.2", PERF_PROTOSTAT_MEDIAN, NULL, NULL);

    /* cacheMedianSvcEntry */
    snmpAddNodeStr("1.3.6.1.4.1.3495.1.3.2.2", 1, NULL, NULL);
    snmpAddNodeStr("1.3.6.1.4.1.3495.1.3.2.2.1", PERF_MEDIAN_TIME, snmp_prfProtoFn, time_Inst, atAverage);
    snmpAddNodeStr("1.3.6.1.4.1.3495.1.3.2.2.1", PERF_MEDIAN_HTTP_ALL, snmp_prfProtoFn, time_Inst, atAverage);
    snmpAddNodeStr("1.3.6.1.4.1.3495.1.3.2.2.1", PERF_MEDIAN_HTTP_MISS, snmp_prfProtoFn, time_Inst, atAverage);
    snmpAddNodeStr("1.3.6.1.4.1.3495.1.3.2.2.1", PERF_MEDIAN_HTTP_NM, snmp_prfProtoFn, time_Inst, atAverage);
    snmpAddNodeStr("1.3.6.1.4.1.3495.1.3.2.2.1", PERF_MEDIAN_HTTP_HIT, snmp_prfProtoFn, time_Inst, atAverage);
    snmpAddNodeStr("1.3.6.1.4.1.3495.1.3.2.2.1", PERF_MEDIAN_ICP_QUERY, snmp_prfProtoFn, time_Inst, atAverage);
    snmpAddNodeStr("1.3.6.1.4.1.3495.1.3.2.2.1", PERF_MEDIAN_ICP_REPLY, snmp_prfProtoFn, time_Inst, atAverage);
    snmpAddNodeStr("1.3.6.1.4.1.3495.1.3.2.2.1", PERF_MEDIAN_DNS, snmp_prfProtoFn, time_Inst, atAverage);
    snmpAddNodeStr("1.3.6.1.4.1.3495.1.3.2.2.1", PERF_MEDIAN_RHR, snmp_prfProtoFn, time_Inst, atAverage);
    snmpAddNodeStr("1.3.6.1.4.1.3495.1.3.2.2.1", PERF_MEDIAN_BHR, snmp_prfProtoFn, time_Inst, atAverage);
    snmpAddNodeStr("1.3.6.1.4.1.3495.1.3.2.2.1", PERF_MEDIAN_HTTP_NH, snmp_prfProtoFn, time_Inst, atAverage);

    /* SQ_NET - 1.3.6.1.4.1.3495.1.4 */
    snmpAddNodeStr("1.3.6.1.4.1.3495.1", 4, NULL, NULL);

    snmpAddNodeStr("1.3.6.1.4.1.3495.1.4", NET_IP_CACHE, NULL, NULL);
    snmpAddNodeStr("1.3.6.1.4.1.3495.1.4.1", IP_ENT, snmp_netIpFn, static_Inst, atSum);
    snmpAddNodeStr("1.3.6.1.4.1.3495.1.4.1", IP_REQ, snmp_netIpFn, static_Inst, atSum);
    snmpAddNodeStr("1.3.6.1.4.1.3495.1.4.1", IP_HITS, snmp_netIpFn, static_Inst, atSum);
    snmpAddNodeStr("1.3.6.1.4.1.3495.1.4.1", IP_PENDHIT, snmp_netIpFn, static_Inst, atSum);
    snmpAddNodeStr("1.3.6.1.4.1.3495.1.4.1", IP_NEGHIT, snmp_netIpFn, static_Inst, atSum);
    snmpAddNodeStr("1.3.6.1.4.1.3495.1.4.1", IP_MISS, snmp_netIpFn, static_Inst, atSum);
    snmpAddNodeStr("1.3.6.1.4.1.3495.1.4.1", IP_GHBN, snmp_netIpFn, static_Inst, atSum);
    snmpAddNodeStr("1.3.6.1.4.1.3495.1.4.1", IP_LOC, snmp_netIpFn, static_Inst, atSum);

    snmpAddNodeStr("1.3.6.1.4.1.3495.1.4", NET_FQDN_CACHE, NULL, NULL);
    snmpAddNodeStr("1.3.6.1.4.1.3495.1.4.2", FQDN_ENT, snmp_netFqdnFn, static_Inst, atSum);
    snmpAddNodeStr("1.3.6.1.4.1.3495.1.4.2", FQDN_REQ, snmp_netFqdnFn, static_Inst, atSum);
    snmpAddNodeStr("1.3.6.1.4.1.3495.1.4.2", FQDN_HITS, snmp_netFqdnFn, static_Inst, atSum);
    snmpAddNodeStr("1.3.6.1.4.1.3495.1.4.2", FQDN_PENDHIT, snmp_netFqdnFn, static_Inst, atSum);
    snmpAddNodeStr("1.3.6.1.4.1.3495.1.4.2", FQDN_NEGHIT, snmp_netFqdnFn, static_Inst, atSum);
    snmpAddNodeStr("1.3.6.1.4.1.3495.1.4.2", FQDN_MISS, snmp_netFqdnFn, static_Inst, atSum);
    snmpAddNodeStr("1.3.6.1.4.1.3495.1.4.2", FQDN_GHBN, snmp_netFqdnFn, static_Inst, atSum);

    snmpAddNodeStr("1.3.6.1.4.1.3495.1.4", NET_DNS_CACHE, NULL, NULL);
    snmpAddNodeStr("1.3.6.1.4.1.3495.1.4.3", DNS_REQ, snmp_netDnsFn, static_Inst, atSum);
    snmpAddNodeStr("1.3.6.1.4.1.3495.1.4.3", DNS_REP, snmp_netDnsFn, static_Inst, atSum);
    snmpAddNodeStr("1.3.6.1.4.1.3495.1.4.3", DNS_SERVERS, snmp_netDnsFn, static_Inst, atSum);

    /* SQ_MESH - 1.3.6.1.4.1.3495.1.5 */
    snmpAddNodeStr("1.3.6.1.4.1.3495.1", 5, NULL, NULL);

    /* cachePeerTable - 1.3.6.1.4.1.3495.1.5.1 */
    snmpAddNodeStr("1.3.6.1.4.1.3495.1.5", MESH_PTBL, NULL, NULL);

    /* CachePeerTableEntry (version 3) - 1.3.6.1.4.1.3495.1.5.1.3 */
    snmpAddNodeStr("1.3.6.1.4.1.3495.1.5.1", 3, NULL, NULL);
    snmpAddNodeStr("1.3.6.1.4.1.3495.1.5.1.3", MESH_PTBL_INDEX, snmp_meshPtblFn, peer_Inst);
    snmpAddNodeStr("1.3.6.1.4.1.3495.1.5.1.3", MESH_PTBL_NAME, snmp_meshPtblFn, peer_Inst);
    snmpAddNodeStr("1.3.6.1.4.1.3495.1.5.1.3", MESH_PTBL_ADDR_TYPE, snmp_meshPtblFn, peer_Inst);
    snmpAddNodeStr("1.3.6.1.4.1.3495.1.5.1.3", MESH_PTBL_ADDR, snmp_meshPtblFn, peer_Inst);
    snmpAddNodeStr("1.3.6.1.4.1.3495.1.5.1.3", MESH_PTBL_HTTP, snmp_meshPtblFn, peer_Inst);
    snmpAddNodeStr("1.3.6.1.4.1.3495.1.5.1.3", MESH_PTBL_ICP, snmp_meshPtblFn, peer_Inst);
    snmpAddNodeStr("1.3.6.1.4.1.3495.1.5.1.3", MESH_PTBL_TYPE, snmp_meshPtblFn, peer_Inst);
    snmpAddNodeStr("1.3.6.1.4.1.3495.1.5.1.3", MESH_PTBL_STATE, snmp_meshPtblFn, peer_Inst);
    snmpAddNodeStr("1.3.6.1.4.1.3495.1.5.1.3", MESH_PTBL_SENT, snmp_meshPtblFn, peer_Inst);
    snmpAddNodeStr("1.3.6.1.4.1.3495.1.5.1.3", MESH_PTBL_PACKED, snmp_meshPtblFn, peer_Inst);
    snmpAddNodeStr("1.3.6.1.4.1.3495.1.5.1.3", MESH_PTBL_FETCHES, snmp_meshPtblFn, peer_Inst);
    snmpAddNodeStr("1.3.6.1.4.1.3495.1.5.1.3", MESH_PTBL_RTT, snmp_meshPtblFn, peer_Inst);
    snmpAddNodeStr("1.3.6.1.4.1.3495.1.5.1.3", MESH_PTBL_IGN, snmp_meshPtblFn, peer_Inst);
    snmpAddNodeStr("1.3.6.1.4.1.3495.1.5.1.3", MESH_PTBL_KEEPAL_S, snmp_meshPtblFn, peer_Inst);
    snmpAddNodeStr("1.3.6.1.4.1.3495.1.5.1.3", MESH_PTBL_KEEPAL_R, snmp_meshPtblFn, peer_Inst);

    /* cacheClientTable - 1.3.6.1.4.1.3495.1.5.2 */
    snmpAddNodeStr("1.3.6.1.4.1.3495.1.5", MESH_CTBL, NULL, NULL);

    /* BUG 2811: we NEED to create a reliable index for the client DB and make version 3 of the table. */
    /* for now we have version 2 table with OID capable of mixed IPv4 / IPv6 clients and upgraded address text format. */

    /* cacheClientEntry - 1.3.6.1.4.1.3495.1.5.2.2 */
    snmpAddNodeStr("1.3.6.1.4.1.3495.1.5.2", 2, NULL, NULL);
    snmpAddNodeStr("1.3.6.1.4.1.3495.1.5.2.2", MESH_CTBL_ADDR_TYPE, snmp_meshCtblFn, client_Inst);
    snmpAddNodeStr("1.3.6.1.4.1.3495.1.5.2.2", MESH_CTBL_ADDR, snmp_meshCtblFn, client_Inst);
    snmpAddNodeStr("1.3.6.1.4.1.3495.1.5.2.2", MESH_CTBL_HTREQ, snmp_meshCtblFn, client_Inst);
    snmpAddNodeStr("1.3.6.1.4.1.3495.1.5.2.2", MESH_CTBL_HTBYTES, snmp_meshCtblFn, client_Inst);
    snmpAddNodeStr("1.3.6.1.4.1.3495.1.5.2.2", MESH_CTBL_HTHITS, snmp_meshCtblFn, client_Inst);
    snmpAddNodeStr("1.3.6.1.4.1.3495.1.5.2.2", MESH_CTBL_HTHITBYTES, snmp_meshCtblFn, client_Inst);
    snmpAddNodeStr("1.3.6.1.4.1.3495.1.5.2.2", MESH_CTBL_ICPREQ, snmp_meshCtblFn, client_Inst);
    snmpAddNodeStr("1.3.6.1.4.1.3495.1.5.2.2", MESH_CTBL_ICPBYTES, snmp_meshCtblFn, client_Inst);
    snmpAddNodeStr("1.3.6.1.4.1.3495.1.5.2.2", MESH_CTBL_ICPHITS, snmp_meshCtblFn, client_Inst);
    mib_tree_last = snmpAddNodeStr("1.3.6.1.4.1.3495.1.5.2.2", MESH_CTBL_ICPHITBYTES, snmp_meshCtblFn, client_Inst);

    debugs(49, 9, "snmpInit: Completed SNMP mib tree structure");
}