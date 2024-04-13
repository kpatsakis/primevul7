ospf_lsaseq_examin
(
  struct lsa_header *lsah, /* start of buffered data */
  size_t length,
  const u_char headeronly,
  /* When declared_num_lsas is not 0, compare it to the real number of LSAs
     and treat the difference as an error. */
  const u_int32_t declared_num_lsas
)
{
  u_int32_t counted_lsas = 0;

  while (length)
  {
    u_int16_t lsalen;
    if (length < OSPF_LSA_HEADER_SIZE)
    {
      if (IS_DEBUG_OSPF_PACKET (0, RECV))
        zlog_debug ("%s: undersized (%zu B) trailing (#%u) LSA header",
                    __func__, length, counted_lsas);
      return MSG_NG;
    }
    /* save on ntohs() calls here and in the LSA validator */
    lsalen = ntohs (lsah->length);
    if (lsalen < OSPF_LSA_HEADER_SIZE)
    {
      if (IS_DEBUG_OSPF_PACKET (0, RECV))
        zlog_debug ("%s: malformed LSA header #%u, declared length is %u B",
                    __func__, counted_lsas, lsalen);
      return MSG_NG;
    }
    if (headeronly)
    {
      /* less checks here and in ospf_lsa_examin() */
      if (MSG_OK != ospf_lsa_examin (lsah, lsalen, 1))
      {
        if (IS_DEBUG_OSPF_PACKET (0, RECV))
          zlog_debug ("%s: malformed header-only LSA #%u", __func__, counted_lsas);
        return MSG_NG;
      }
      lsah = (struct lsa_header *) ((caddr_t) lsah + OSPF_LSA_HEADER_SIZE);
      length -= OSPF_LSA_HEADER_SIZE;
    }
    else
    {
      /* make sure the input buffer is deep enough before further checks */
      if (lsalen > length)
      {
        if (IS_DEBUG_OSPF_PACKET (0, RECV))
          zlog_debug ("%s: anomaly in LSA #%u: declared length is %u B, buffered length is %zu B",
                      __func__, counted_lsas, lsalen, length);
        return MSG_NG;
      }
      if (MSG_OK != ospf_lsa_examin (lsah, lsalen, 0))
      {
        if (IS_DEBUG_OSPF_PACKET (0, RECV))
          zlog_debug ("%s: malformed LSA #%u", __func__, counted_lsas);
        return MSG_NG;
      }
      lsah = (struct lsa_header *) ((caddr_t) lsah + lsalen);
      length -= lsalen;
    }
    counted_lsas++;
  }

  if (declared_num_lsas && counted_lsas != declared_num_lsas)
  {
    if (IS_DEBUG_OSPF_PACKET (0, RECV))
      zlog_debug ("%s: #LSAs declared (%u) does not match actual (%u)",
                  __func__, declared_num_lsas, counted_lsas);
    return MSG_NG;
  }
  return MSG_OK;
}