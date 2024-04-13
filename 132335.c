chooseSelectOption(FormItemList *fi, FormSelectOptionItem *item)
{
    FormSelectOptionItem *opt;
    int i;

    fi->selected = 0;
    if (item == NULL) {
	fi->value = Strnew_size(0);
	fi->label = Strnew_size(0);
	return;
    }
    fi->value = item->value;
    fi->label = item->label;
    for (i = 0, opt = item; opt != NULL; i++, opt = opt->next) {
	if (opt->checked) {
	    fi->value = opt->value;
	    fi->label = opt->label;
	    fi->selected = i;
	    break;
	}
    }
    updateSelectOption(fi, item);
}