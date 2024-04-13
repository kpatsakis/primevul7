CURLcode Curl_connected_proxy(struct connectdata *conn,
                              int sockindex)
{
  if(!conn->bits.proxy || sockindex)
    /* this magic only works for the primary socket as the secondary is used
       for FTP only and it has FTP specific magic in ftp.c */
    return CURLE_OK;

  switch(conn->proxytype) {
#ifndef CURL_DISABLE_PROXY
  case CURLPROXY_SOCKS5:
  case CURLPROXY_SOCKS5_HOSTNAME:
    return Curl_SOCKS5(conn->proxyuser, conn->proxypasswd,
                       conn->host.name, conn->remote_port,
                       FIRSTSOCKET, conn);

  case CURLPROXY_SOCKS4:
    return Curl_SOCKS4(conn->proxyuser, conn->host.name,
                       conn->remote_port, FIRSTSOCKET, conn, FALSE);

  case CURLPROXY_SOCKS4A:
    return Curl_SOCKS4(conn->proxyuser, conn->host.name,
                       conn->remote_port, FIRSTSOCKET, conn, TRUE);

#endif /* CURL_DISABLE_PROXY */
  case CURLPROXY_HTTP:
  case CURLPROXY_HTTP_1_0:
    /* do nothing here. handled later. */
    break;
  default:
    break;
  } /* switch proxytype */

  return CURLE_OK;
}