ospf_make_auth (struct ospf_interface *oi, struct ospf_header *ospfh)
{
  struct crypt_key *ck;

  switch (ospf_auth_type (oi))
    {
    case OSPF_AUTH_NULL:
      /* memset (ospfh->u.auth_data, 0, sizeof (ospfh->u.auth_data)); */
      break;
    case OSPF_AUTH_SIMPLE:
      memcpy (ospfh->u.auth_data, OSPF_IF_PARAM (oi, auth_simple),
	      OSPF_AUTH_SIMPLE_SIZE);
      break;
    case OSPF_AUTH_CRYPTOGRAPHIC:
      /* If key is not set, then set 0. */
      if (list_isempty (OSPF_IF_PARAM (oi, auth_crypt)))
	{
	  ospfh->u.crypt.zero = 0;
	  ospfh->u.crypt.key_id = 0;
	  ospfh->u.crypt.auth_data_len = OSPF_AUTH_MD5_SIZE;
	}
      else
	{
	  ck = listgetdata (listtail(OSPF_IF_PARAM (oi, auth_crypt)));
	  ospfh->u.crypt.zero = 0;
	  ospfh->u.crypt.key_id = ck->key_id;
	  ospfh->u.crypt.auth_data_len = OSPF_AUTH_MD5_SIZE;
	}
      /* note: the seq is done in ospf_make_md5_digest() */
      break;
    default:
      /* memset (ospfh->u.auth_data, 0, sizeof (ospfh->u.auth_data)); */
      break;
    }

  return 0;
}