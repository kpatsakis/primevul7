url_encode(const char *url, const ParsedURL *base, wc_ces doc_charset)
{
    return url_quote_conv((char *)url,
			  url_to_charset(url, base, doc_charset));
}