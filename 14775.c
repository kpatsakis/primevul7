estack_pop(void)
{
    if (exestack.ga_len == 0)
	return NULL;
    --exestack.ga_len;
    return ((estack_T *)exestack.ga_data) + exestack.ga_len;
}