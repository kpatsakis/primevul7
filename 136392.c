aspath_finish (void)
{
  hash_clean (ashash, (void (*)(void *))aspath_free);
  hash_free (ashash);
  ashash = NULL;
  
  if (snmp_stream)
    stream_free (snmp_stream);
}