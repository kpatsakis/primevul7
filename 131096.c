static CURLcode do_init(struct connectdata *conn)
{
  struct SessionHandle *data = conn->data;
  struct SingleRequest *k = &data->req;

  conn->bits.done = FALSE; /* Curl_done() is not called yet */
  conn->bits.do_more = FALSE; /* by default there's no curl_do_more() to use */
  data->state.expect100header = FALSE;

  if(data->set.opt_no_body)
    /* in HTTP lingo, no body means using the HEAD request... */
    data->set.httpreq = HTTPREQ_HEAD;
  else if(HTTPREQ_HEAD == data->set.httpreq)
    /* ... but if unset there really is no perfect method that is the
       "opposite" of HEAD but in reality most people probably think GET
       then. The important thing is that we can't let it remain HEAD if the
       opt_no_body is set FALSE since then we'll behave wrong when getting
       HTTP. */
    data->set.httpreq = HTTPREQ_GET;

  k->start = Curl_tvnow(); /* start time */
  k->now = k->start;   /* current time is now */
  k->header = TRUE; /* assume header */

  k->bytecount = 0;

  k->buf = data->state.buffer;
  k->uploadbuf = data->state.uploadbuffer;
  k->hbufp = data->state.headerbuff;
  k->ignorebody=FALSE;

  Curl_speedinit(data);

  Curl_pgrsSetUploadCounter(data, 0);
  Curl_pgrsSetDownloadCounter(data, 0);

  return CURLE_OK;
}