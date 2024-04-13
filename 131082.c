bool Curl_isPipeliningEnabled(const struct SessionHandle *handle)
{
  return Curl_multi_pipeline_enabled(handle->multi);
}