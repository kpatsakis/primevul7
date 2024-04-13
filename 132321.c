newFormList(char *action, char *method, char *charset, char *enctype,
	    char *target, char *name, struct form_list *_next)
{
    struct form_list *l;
    Str a = Strnew_charp(action);
    int m = FORM_METHOD_GET;
    int e = FORM_ENCTYPE_URLENCODED;
#ifdef USE_M17N
    wc_ces c = 0;
#endif

    if (method == NULL || !strcasecmp(method, "get"))
	m = FORM_METHOD_GET;
    else if (!strcasecmp(method, "post"))
	m = FORM_METHOD_POST;
    else if (!strcasecmp(method, "internal"))
	m = FORM_METHOD_INTERNAL;
    /* unknown method is regarded as 'get' */

    if (enctype != NULL && !strcasecmp(enctype, "multipart/form-data")) {
	e = FORM_ENCTYPE_MULTIPART;
	if (m == FORM_METHOD_GET)
	    m = FORM_METHOD_POST;
    }

#ifdef USE_M17N
    if (charset != NULL)
	c = wc_guess_charset(charset, 0);
#endif

    l = New(struct form_list);
    l->item = l->lastitem = NULL;
    l->action = a;
    l->method = m;
#ifdef USE_M17N
    l->charset = c;
#endif
    l->enctype = e;
    l->target = target;
    l->name = name;
    l->next = _next;
    l->nitems = 0;
    l->body = NULL;
    l->length = 0;
    return l;
}