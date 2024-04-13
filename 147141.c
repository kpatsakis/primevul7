url_decode0(const char *url)
{
    if (!DecodeURL)
	return (char *)url;
    return url_unquote_conv((char *)url, 0);
}