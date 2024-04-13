find_start_of_next_routerstatus(const char *s)
{
  const char *eos, *footer, *sig;
  if ((eos = strstr(s, "\nr ")))
    ++eos;
  else
    eos = s + strlen(s);

  footer = tor_memstr(s, eos-s, "\ndirectory-footer");
  sig = tor_memstr(s, eos-s, "\ndirectory-signature");

  if (footer && sig)
    return MIN(footer, sig) + 1;
  else if (footer)
    return footer+1;
  else if (sig)
    return sig+1;
  else
    return eos;
}