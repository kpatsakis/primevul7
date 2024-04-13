url_decode(const char *url, const ParsedURL *base, wc_ces doc_charset)
{
    if (!DecodeURL)
	return (char *)url;
    return url_unquote_conv((char *)url,
			    url_to_charset(url, base, doc_charset));
}