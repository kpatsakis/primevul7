ospf_make_md5_digest (struct ospf_interface *oi, struct ospf_packet *op)
{
  struct ospf_header *ospfh;
  unsigned char digest[OSPF_AUTH_MD5_SIZE];
  MD5_CTX ctx;
  void *ibuf;
  u_int32_t t;
  struct crypt_key *ck;
  const u_int8_t *auth_key;

  ibuf = STREAM_DATA (op->s);
  ospfh = (struct ospf_header *) ibuf;

  if (ntohs (ospfh->auth_type) != OSPF_AUTH_CRYPTOGRAPHIC)
    return 0;

  /* We do this here so when we dup a packet, we don't have to
     waste CPU rewriting other headers.
     
     Note that quagga_time /deliberately/ is not used here */
  t = (time(NULL) & 0xFFFFFFFF);
  if (t > oi->crypt_seqnum)
    oi->crypt_seqnum = t;
  else
    oi->crypt_seqnum++;
  
  ospfh->u.crypt.crypt_seqnum = htonl (oi->crypt_seqnum); 

  /* Get MD5 Authentication key from auth_key list. */
  if (list_isempty (OSPF_IF_PARAM (oi, auth_crypt)))
    auth_key = (const u_int8_t *) "";
  else
    {
      ck = listgetdata (listtail(OSPF_IF_PARAM (oi, auth_crypt)));
      auth_key = ck->auth_key;
    }

  /* Generate a digest for the entire packet + our secret key. */
  memset(&ctx, 0, sizeof(ctx));
  MD5Init(&ctx);
  MD5Update(&ctx, ibuf, ntohs (ospfh->length));
  MD5Update(&ctx, auth_key, OSPF_AUTH_MD5_SIZE);
  MD5Final(digest, &ctx);

  /* Append md5 digest to the end of the stream. */
  stream_put (op->s, digest, OSPF_AUTH_MD5_SIZE);

  /* We do *NOT* increment the OSPF header length. */
  op->length = ntohs (ospfh->length) + OSPF_AUTH_MD5_SIZE;

  if (stream_get_endp(op->s) != op->length)
    /* XXX size_t */
    zlog_warn("ospf_make_md5_digest: length mismatch stream %lu ospf_packet %u",
	      (u_long)stream_get_endp(op->s), op->length);

  return OSPF_AUTH_MD5_SIZE;
}