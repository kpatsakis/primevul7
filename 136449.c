assegment_header_put (struct stream *s, u_char type, int length)
{
  size_t lenp;
  assert (length <= AS_SEGMENT_MAX);
  stream_putc (s, type);
  lenp = stream_get_endp (s);
  stream_putc (s, length);
  return lenp;
}