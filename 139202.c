static my_bool my_realloc_str(NET *net, ulong length)
{
  ulong buf_length= (ulong) (net->write_pos - net->buff);
  my_bool res=0;
  DBUG_ENTER("my_realloc_str");
  if (buf_length + length > net->max_packet)
  {
    res= net_realloc(net, buf_length + length);
    if (res)
    {
      if (net->last_errno == ER_OUT_OF_RESOURCES)
        net->last_errno= CR_OUT_OF_MEMORY;
      else if (net->last_errno == ER_NET_PACKET_TOO_LARGE)
        net->last_errno= CR_NET_PACKET_TOO_LARGE;
      strmov(net->sqlstate, unknown_sqlstate);
      strmov(net->last_error, ER(net->last_errno));
    }
    net->write_pos= net->buff+ buf_length;
  }
  DBUG_RETURN(res);
}