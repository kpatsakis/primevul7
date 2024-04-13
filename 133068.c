ospf_check_md5_digest (struct ospf_interface *oi, struct ospf_header *ospfh)
{
  MD5_CTX ctx;
  unsigned char digest[OSPF_AUTH_MD5_SIZE];
  struct crypt_key *ck;
  struct ospf_neighbor *nbr;
  u_int16_t length = ntohs (ospfh->length);
  
  /* Get secret key. */
  ck = ospf_crypt_key_lookup (OSPF_IF_PARAM (oi, auth_crypt),
			      ospfh->u.crypt.key_id);
  if (ck == NULL)
    {
      zlog_warn ("interface %s: ospf_check_md5 no key %d",
		 IF_NAME (oi), ospfh->u.crypt.key_id);
      return 0;
    }

  /* check crypto seqnum. */
  nbr = ospf_nbr_lookup_by_routerid (oi->nbrs, &ospfh->router_id);

  if (nbr && ntohl(nbr->crypt_seqnum) > ntohl(ospfh->u.crypt.crypt_seqnum))
    {
      zlog_warn ("interface %s: ospf_check_md5 bad sequence %d (expect %d)",
		 IF_NAME (oi),
		 ntohl(ospfh->u.crypt.crypt_seqnum),
		 ntohl(nbr->crypt_seqnum));
      return 0;
    }
      
  /* Generate a digest for the ospf packet - their digest + our digest. */
  memset(&ctx, 0, sizeof(ctx));
  MD5Init(&ctx);
  MD5Update(&ctx, ospfh, length);
  MD5Update(&ctx, ck->auth_key, OSPF_AUTH_MD5_SIZE);
  MD5Final(digest, &ctx);

  /* compare the two */
  if (memcmp ((caddr_t)ospfh + length, digest, OSPF_AUTH_MD5_SIZE))
    {
      zlog_warn ("interface %s: ospf_check_md5 checksum mismatch",
		 IF_NAME (oi));
      return 0;
    }

  /* save neighbor's crypt_seqnum */
  if (nbr)
    nbr->crypt_seqnum = ospfh->u.crypt.crypt_seqnum;
  return 1;
}