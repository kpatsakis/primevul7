aspath_snmp_pathseg (struct aspath *as, size_t *varlen)
{
#define SNMP_PATHSEG_MAX 1024

  if (!snmp_stream)
    snmp_stream = stream_new (SNMP_PATHSEG_MAX);
  else
    stream_reset (snmp_stream);
  
  if (!as)
    {
      *varlen = 0;
      return NULL;
    }
  aspath_put (snmp_stream, as, 0); /* use 16 bit for now here */
  
  *varlen = stream_get_endp (snmp_stream);
  return stream_pnt(snmp_stream);
}