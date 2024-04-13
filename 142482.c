SAPI_API int sapi_register_input_filter(unsigned int (*input_filter)(int arg, char *var, char **val, unsigned int val_len, unsigned int *new_val_len TSRMLS_DC), unsigned int (*input_filter_init)(TSRMLS_D) TSRMLS_DC)
{
	if (SG(sapi_started) && EG(in_execution)) {
		return FAILURE;
	}
	sapi_module.input_filter = input_filter;
	sapi_module.input_filter_init = input_filter_init;
	return SUCCESS;
}