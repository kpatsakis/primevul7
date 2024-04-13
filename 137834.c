
static void autoload_func_info_dtor(autoload_func_info *alfi)
{
	if (alfi->obj) {
		zval_ptr_dtor(&alfi->obj);
	}
	if (alfi->closure) {
		zval_ptr_dtor(&alfi->closure);
	}