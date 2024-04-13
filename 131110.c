int Curl_doing_getsock(struct connectdata *conn,
                       curl_socket_t *socks,
                       int numsocks)
{
  if(conn && conn->handler->doing_getsock)
    return conn->handler->doing_getsock(conn, socks, numsocks);
  return GETSOCK_BLANK;
}