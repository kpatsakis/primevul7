static uchar* check_get_key(ACL_USER *buff, size_t *length,
                            my_bool not_used __attribute__((unused)))
{
  *length=buff->host.get_host_len();
  return (uchar*) buff->host.get_host();
}