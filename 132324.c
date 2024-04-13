form2str(FormItemList *fi)
{
    Str tmp = Strnew();

    if (fi->type != FORM_SELECT && fi->type != FORM_TEXTAREA)
	Strcat_charp(tmp, "input type=");
    Strcat_charp(tmp, _formtypetbl[fi->type]);
    if (fi->name && fi->name->length)
	Strcat_m_charp(tmp, " name=\"", fi->name->ptr, "\"", NULL);
    if ((fi->type == FORM_INPUT_RADIO || fi->type == FORM_INPUT_CHECKBOX ||
	 fi->type == FORM_SELECT) && fi->value)
	Strcat_m_charp(tmp, " value=\"", fi->value->ptr, "\"", NULL);
    Strcat_m_charp(tmp, " (", _formmethodtbl[fi->parent->method], " ",
		   fi->parent->action->ptr, ")", NULL);
    return tmp->ptr;
}