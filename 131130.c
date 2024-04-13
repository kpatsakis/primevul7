static CURLcode ConnectionStore(struct SessionHandle *data,
                                struct connectdata *conn)
{
  return Curl_conncache_add_conn(data->state.conn_cache, conn);
}