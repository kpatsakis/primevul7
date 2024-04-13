ospf_make_header (int type, struct ospf_interface *oi, struct stream *s)
{
  struct ospf_header *ospfh;

  ospfh = (struct ospf_header *) STREAM_DATA (s);

  ospfh->version = (u_char) OSPF_VERSION;
  ospfh->type = (u_char) type;

  ospfh->router_id = oi->ospf->router_id;

  ospfh->checksum = 0;
  ospfh->area_id = oi->area->area_id;
  ospfh->auth_type = htons (ospf_auth_type (oi));

  memset (ospfh->u.auth_data, 0, OSPF_AUTH_SIMPLE_SIZE);

  stream_forward_endp (s, OSPF_HEADER_SIZE);
}