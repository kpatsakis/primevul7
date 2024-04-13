add_pre_form_item(struct pre_form *pf, struct pre_form_item *prev, int type,
		  char *name, char *value, char *checked)
{
    struct pre_form_item *new;

    if (!pf)
	return NULL;
    if (prev)
	new = prev->next = New(struct pre_form_item);
    else
	new = pf->item = New(struct pre_form_item);
    new->type = type;
    new->name = name;
    new->value = value;
    if (checked && *checked && (!strcmp(checked, "0") ||
				!strcasecmp(checked, "off")
				|| !strcasecmp(checked, "no")))
	new->checked = 0;
    else
	new->checked = 1;
    new->next = NULL;
    return new;
}