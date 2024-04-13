addSelectOption(FormSelectOption *fso, Str value, Str label, int chk)
{
    FormSelectOptionItem *o;
    o = New(FormSelectOptionItem);
    if (value == NULL)
	value = label;
    o->value = value;
    Strremovefirstspaces(label);
    Strremovetrailingspaces(label);
    o->label = label;
    o->checked = chk;
    o->next = NULL;
    if (fso->first == NULL)
	fso->first = fso->last = o;
    else {
	fso->last->next = o;
	fso->last = o;
    }
}