static void do_complete(struct connectdata *conn)
{
  conn->data->req.chunk=FALSE;
  conn->data->req.maxfd = (conn->sockfd>conn->writesockfd?
                           conn->sockfd:conn->writesockfd)+1;
  Curl_pgrsTime(conn->data, TIMER_PRETRANSFER);
}