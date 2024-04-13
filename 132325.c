add_pre_form(struct pre_form *prev, char *url, Regex *re_url, char *name, char *action)
{
    ParsedURL pu;
    struct pre_form *new;

    if (prev)
	new = prev->next = New(struct pre_form);
    else
	new = PreForm = New(struct pre_form);
    if (url && !re_url) {
	parseURL2(url, &pu, NULL);
	new->url = parsedURL2Str(&pu)->ptr;
    }
    else
	new->url = url;
    new->re_url = re_url;
    new->name = (name && *name) ? name : NULL;
    new->action = (action && *action) ? action : NULL;
    new->item = NULL;
    new->next = NULL;
    return new;
}