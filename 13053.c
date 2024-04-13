fill_assert_error(
    garray_T	*gap,
    typval_T	*opt_msg_tv,
    char_u      *exp_str,
    typval_T	*exp_tv_arg,
    typval_T	*got_tv_arg,
    assert_type_T atype)
{
    char_u	numbuf[NUMBUFLEN];
    char_u	*tofree;
    typval_T	*exp_tv = exp_tv_arg;
    typval_T	*got_tv = got_tv_arg;
    int		did_copy = FALSE;
    int		omitted = 0;

    if (opt_msg_tv->v_type != VAR_UNKNOWN
	    && !(opt_msg_tv->v_type == VAR_STRING
		&& (opt_msg_tv->vval.v_string == NULL
		    || *opt_msg_tv->vval.v_string == NUL)))
    {
	ga_concat(gap, echo_string(opt_msg_tv, &tofree, numbuf, 0));
	vim_free(tofree);
	ga_concat(gap, (char_u *)": ");
    }

    if (atype == ASSERT_MATCH || atype == ASSERT_NOTMATCH)
	ga_concat(gap, (char_u *)"Pattern ");
    else if (atype == ASSERT_NOTEQUAL)
	ga_concat(gap, (char_u *)"Expected not equal to ");
    else
	ga_concat(gap, (char_u *)"Expected ");
    if (exp_str == NULL)
    {
	// When comparing dictionaries, drop the items that are equal, so that
	// it's a lot easier to see what differs.
	if (atype != ASSERT_NOTEQUAL
		&& exp_tv->v_type == VAR_DICT && got_tv->v_type == VAR_DICT
		&& exp_tv->vval.v_dict != NULL && got_tv->vval.v_dict != NULL)
	{
	    dict_T	*exp_d = exp_tv->vval.v_dict;
	    dict_T	*got_d = got_tv->vval.v_dict;
	    hashitem_T	*hi;
	    dictitem_T	*item2;
	    int		todo;

	    did_copy = TRUE;
	    exp_tv->vval.v_dict = dict_alloc();
	    got_tv->vval.v_dict = dict_alloc();
	    if (exp_tv->vval.v_dict == NULL || got_tv->vval.v_dict == NULL)
		return;

	    todo = (int)exp_d->dv_hashtab.ht_used;
	    for (hi = exp_d->dv_hashtab.ht_array; todo > 0; ++hi)
	    {
		if (!HASHITEM_EMPTY(hi))
		{
		    item2 = dict_find(got_d, hi->hi_key, -1);
		    if (item2 == NULL || !tv_equal(&HI2DI(hi)->di_tv,
						  &item2->di_tv, FALSE, FALSE))
		    {
			// item of exp_d not present in got_d or values differ.
			dict_add_tv(exp_tv->vval.v_dict,
					(char *)hi->hi_key, &HI2DI(hi)->di_tv);
			if (item2 != NULL)
			    dict_add_tv(got_tv->vval.v_dict,
					    (char *)hi->hi_key, &item2->di_tv);
		    }
		    else
			++omitted;
		    --todo;
		}
	    }

	    // Add items only present in got_d.
	    todo = (int)got_d->dv_hashtab.ht_used;
	    for (hi = got_d->dv_hashtab.ht_array; todo > 0; ++hi)
	    {
		if (!HASHITEM_EMPTY(hi))
		{
		    item2 = dict_find(exp_d, hi->hi_key, -1);
		    if (item2 == NULL)
			// item of got_d not present in exp_d
			dict_add_tv(got_tv->vval.v_dict,
					(char *)hi->hi_key, &HI2DI(hi)->di_tv);
		    --todo;
		}
	    }
	}

	ga_concat_shorten_esc(gap, tv2string(exp_tv, &tofree, numbuf, 0));
	vim_free(tofree);
    }
    else
    {
	ga_concat(gap, (char_u *)"'");
	ga_concat_shorten_esc(gap, exp_str);
	ga_concat(gap, (char_u *)"'");
    }
    if (atype != ASSERT_NOTEQUAL)
    {
	if (atype == ASSERT_MATCH)
	    ga_concat(gap, (char_u *)" does not match ");
	else if (atype == ASSERT_NOTMATCH)
	    ga_concat(gap, (char_u *)" does match ");
	else
	    ga_concat(gap, (char_u *)" but got ");
	ga_concat_shorten_esc(gap, tv2string(got_tv, &tofree, numbuf, 0));
	vim_free(tofree);

	if (omitted != 0)
	{
	    char buf[100];

	    vim_snprintf(buf, 100, " - %d equal item%s omitted",
					     omitted, omitted == 1 ? "" : "s");
	    ga_concat(gap, (char_u *)buf);
	}
    }

    if (did_copy)
    {
	clear_tv(exp_tv);
	clear_tv(got_tv);
    }
}