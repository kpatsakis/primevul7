static PHP_INI_MH(OnUpdateInternalEncoding)
{
	if (new_value) {
		OnUpdateString(entry, new_value, mh_arg1, mh_arg2, mh_arg3, stage);
	}
	return SUCCESS;
}