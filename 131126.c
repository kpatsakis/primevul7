static bool SocketIsDead(curl_socket_t sock)
{
  int sval;
  bool ret_val = TRUE;

  sval = Curl_socket_ready(sock, CURL_SOCKET_BAD, 0);
  if(sval == 0)
    /* timeout */
    ret_val = FALSE;

  return ret_val;
}