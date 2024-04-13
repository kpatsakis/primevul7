static bool IsPipeliningPossible(const struct SessionHandle *handle,
                                 const struct connectdata *conn)
{
  if((conn->handler->protocol & PROTO_FAMILY_HTTP) &&
     Curl_multi_pipeline_enabled(handle->multi) &&
     (handle->set.httpreq == HTTPREQ_GET ||
      handle->set.httpreq == HTTPREQ_HEAD) &&
     handle->set.httpversion != CURL_HTTP_VERSION_1_0)
    return TRUE;

  return FALSE;
}