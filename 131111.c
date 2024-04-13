static bool disconnect_if_dead(struct connectdata *conn,
                               struct SessionHandle *data)
{
  size_t pipeLen = conn->send_pipe->size + conn->recv_pipe->size;
  if(!pipeLen && !conn->inuse) {
    /* The check for a dead socket makes sense only if there are no
       handles in pipeline and the connection isn't already marked in
       use */
    bool dead;
    if(conn->handler->protocol & CURLPROTO_RTSP)
      /* RTSP is a special case due to RTP interleaving */
      dead = Curl_rtsp_connisdead(conn);
    else
      dead = SocketIsDead(conn->sock[FIRSTSOCKET]);

    if(dead) {
      conn->data = data;
      infof(data, "Connection %ld seems to be dead!\n", conn->connection_id);

      /* disconnect resources */
      Curl_disconnect(conn, /* dead_connection */TRUE);
      return TRUE;
    }
  }
  return FALSE;
}