void Curl_getoff_all_pipelines(struct SessionHandle *data,
                               struct connectdata *conn)
{
  bool recv_head = (conn->readchannel_inuse &&
    (gethandleathead(conn->recv_pipe) == data)) ? TRUE : FALSE;

  bool send_head = (conn->writechannel_inuse &&
    (gethandleathead(conn->send_pipe) == data)) ? TRUE : FALSE;

  if(Curl_removeHandleFromPipeline(data, conn->recv_pipe) && recv_head)
    conn->readchannel_inuse = FALSE;
  if(Curl_removeHandleFromPipeline(data, conn->send_pipe) && send_head)
    conn->writechannel_inuse = FALSE;
}