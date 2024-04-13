ospf_auth_type (struct ospf_interface *oi)
{
  int auth_type;

  if (OSPF_IF_PARAM (oi, auth_type) == OSPF_AUTH_NOTSET)
    auth_type = oi->area->auth_type;
  else
    auth_type = OSPF_IF_PARAM (oi, auth_type);

  /* Handle case where MD5 key list is not configured aka Cisco */
  if (auth_type == OSPF_AUTH_CRYPTOGRAPHIC &&
      list_isempty (OSPF_IF_PARAM (oi, auth_crypt)))
    return OSPF_AUTH_NULL;
  
  return auth_type;

}