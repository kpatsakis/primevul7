static void prune_dead_connections(struct SessionHandle *data)
{
  struct timeval now = Curl_tvnow();
  long elapsed = Curl_tvdiff(now, data->state.conn_cache->last_cleanup);

  if(elapsed >= 1000L) {
    Curl_conncache_foreach(data->state.conn_cache, data,
                           call_disconnect_if_dead);
    data->state.conn_cache->last_cleanup = now;
  }
}