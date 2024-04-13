url_to_charset(const char *url, const ParsedURL *base, wc_ces doc_charset)
{
    const ParsedURL *pu;
    ParsedURL pu_buf;
    const wc_ces *csptr;

    if (url && *url && *url != '#') {
	parseURL2((char *)url, &pu_buf, (ParsedURL *)base);
	pu = &pu_buf;
    } else {
	pu = base;
    }
    if (pu && (pu->scheme == SCM_LOCAL || pu->scheme == SCM_LOCAL_CGI))
	return SystemCharset;
    csptr = query_SCONF_URL_CHARSET(pu);
    return (csptr && *csptr) ? *csptr :
	doc_charset ? doc_charset : DocumentCharset;
}