void Curl_verboseconnect(struct connectdata *conn)
{
  if(conn->data->set.verbose)
    infof(conn->data, "Connected to %s (%s) port %ld (#%ld)\n",
          conn->bits.proxy ? conn->proxy.dispname : conn->host.dispname,
          conn->ip_addr_str, conn->port, conn->connection_id);
}