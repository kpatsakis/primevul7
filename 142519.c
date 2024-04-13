socket_write(struct connectdata *conn, curl_socket_t fd, const void *to,
             size_t len)
{
  const char *to_p = to;
  CURLcode result;
  ssize_t written;

  while(len > 0) {
    result = Curl_write_plain(conn, fd, to_p, len, &written);
    if(!result) {
      len -= written;
      to_p += written;
    }
    else {
      if(result == CURLE_AGAIN)
        continue;
      return result;
    }
  }
  return CURLE_OK;
}