static CURLcode setstropt(char **charp, char *s)
{
  /* Release the previous storage at `charp' and replace by a dynamic storage
     copy of `s'. Return CURLE_OK or CURLE_OUT_OF_MEMORY. */

  Curl_safefree(*charp);

  if(s) {
    s = strdup(s);

    if(!s)
      return CURLE_OUT_OF_MEMORY;

    *charp = s;
  }

  return CURLE_OK;
}