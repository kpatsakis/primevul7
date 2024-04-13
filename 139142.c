void my_net_local_init(NET *net)
{
  net->max_packet=   (uint) net_buffer_length;
  my_net_set_read_timeout(net, CLIENT_NET_READ_TIMEOUT);
  my_net_set_write_timeout(net, CLIENT_NET_WRITE_TIMEOUT);
  net->retry_count=  1;
  net->max_packet_size= max(net_buffer_length, max_allowed_packet);
}