ospf_packet_authspace (struct ospf_interface *oi)
{
  int auth = 0;

  if ( ospf_auth_type (oi) == OSPF_AUTH_CRYPTOGRAPHIC)
    auth = OSPF_AUTH_MD5_SIZE;

  return auth;
}