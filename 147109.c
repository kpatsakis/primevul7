url_decode2(const char *url, const Buffer *buf)
{
    wc_ces url_charset;

    if (!DecodeURL)
	return (char *)url;
    url_charset = buf ?
	url_to_charset(url, baseURL((Buffer *)buf), buf->document_charset) :
	url_to_charset(url, NULL, 0);
    return url_unquote_conv((char *)url, url_charset);
}