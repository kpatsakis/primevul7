ospf_lsa_examin (struct lsa_header * lsah, const u_int16_t lsalen, const u_char headeronly)
{
  unsigned ret;
  struct router_lsa * rlsa;
  if
  (
    lsah->type < OSPF_MAX_LSA &&
    ospf_lsa_minlen[lsah->type] &&
    lsalen < OSPF_LSA_HEADER_SIZE + ospf_lsa_minlen[lsah->type]
  )
  {
    if (IS_DEBUG_OSPF_PACKET (0, RECV))
      zlog_debug ("%s: undersized (%u B) %s",
                  __func__, lsalen, LOOKUP (ospf_lsa_type_msg, lsah->type));
    return MSG_NG;
  }
  switch (lsah->type)
  {
  case OSPF_ROUTER_LSA:
    /* RFC2328 A.4.2, LSA header + 4 bytes followed by N>=1 (12+)-byte link blocks */
    if (headeronly)
    {
      ret = (lsalen - OSPF_LSA_HEADER_SIZE - OSPF_ROUTER_LSA_MIN_SIZE) % 4 ? MSG_NG : MSG_OK;
      break;
    }
    rlsa = (struct router_lsa *) lsah;
    ret = ospf_router_lsa_links_examin
    (
      (struct router_lsa_link *) rlsa->link,
      lsalen - OSPF_LSA_HEADER_SIZE - 4, /* skip: basic header, "flags", 0, "# links" */
      ntohs (rlsa->links) /* 16 bits */
    );
    break;
  case OSPF_AS_EXTERNAL_LSA:
    /* RFC2328 A.4.5, LSA header + 4 bytes followed by N>=1 12-bytes long blocks */
  case OSPF_AS_NSSA_LSA:
    /* RFC3101 C, idem */
    ret = (lsalen - OSPF_LSA_HEADER_SIZE - OSPF_AS_EXTERNAL_LSA_MIN_SIZE) % 12 ? MSG_NG : MSG_OK;
    break;
  /* Following LSA types are considered OK length-wise as soon as their minimum
   * length constraint is met and length of the whole LSA is a multiple of 4
   * (basic LSA header size is already a multiple of 4). */
  case OSPF_NETWORK_LSA:
    /* RFC2328 A.4.3, LSA header + 4 bytes followed by N>=1 router-IDs */
  case OSPF_SUMMARY_LSA:
  case OSPF_ASBR_SUMMARY_LSA:
    /* RFC2328 A.4.4, LSA header + 4 bytes followed by N>=1 4-bytes TOS blocks */
#ifdef HAVE_OPAQUE_LSA
  case OSPF_OPAQUE_LINK_LSA:
  case OSPF_OPAQUE_AREA_LSA:
  case OSPF_OPAQUE_AS_LSA:
    /* RFC5250 A.2, "some number of octets (of application-specific
     * data) padded to 32-bit alignment." This is considered equivalent
     * to 4-byte alignment of all other LSA types, see OSPF-ALIGNMENT.txt
     * file for the detailed analysis of this passage. */
#endif
    ret = lsalen % 4 ? MSG_NG : MSG_OK;
    break;
  default:
    if (IS_DEBUG_OSPF_PACKET (0, RECV))
      zlog_debug ("%s: unsupported LSA type 0x%02x", __func__, lsah->type);
    return MSG_NG;
  }
  if (ret != MSG_OK && IS_DEBUG_OSPF_PACKET (0, RECV))
    zlog_debug ("%s: alignment error in %s",
                __func__, LOOKUP (ospf_lsa_type_msg, lsah->type));
  return ret;
}