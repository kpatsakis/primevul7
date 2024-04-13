formChooseOptionByMenu(struct form_item_list *fi, int x, int y)
{
    int i, n, selected = -1, init_select = fi->selected;
    FormSelectOptionItem *opt;
    char **label;

    for (n = 0, opt = fi->select_option; opt != NULL; n++, opt = opt->next) ;
    label = New_N(char *, n + 1);
    for (i = 0, opt = fi->select_option; opt != NULL; i++, opt = opt->next)
	label[i] = opt->label->ptr;
    label[n] = NULL;

    optionMenu(x, y, label, &selected, init_select, NULL);

    if (selected < 0)
	return 0;
    for (i = 0, opt = fi->select_option; opt != NULL; i++, opt = opt->next) {
	if (i == selected) {
	    fi->selected = selected;
	    fi->value = opt->value;
	    fi->label = opt->label;
	    break;
	}
    }
    updateSelectOption(fi, fi->select_option);
    return 1;
}