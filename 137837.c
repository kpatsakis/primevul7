 Try all registerd autoload function to load the requested class */
PHP_FUNCTION(spl_autoload_call)
{
	zval *class_name, *retval = NULL;
	int class_name_len;
	char *func_name, *lc_name;
	uint func_name_len;
	ulong dummy;
	HashPosition function_pos;
	autoload_func_info *alfi;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &class_name) == FAILURE || Z_TYPE_P(class_name) != IS_STRING) {
		return;
	}

	if (SPL_G(autoload_functions)) {
		int l_autoload_running = SPL_G(autoload_running);
		SPL_G(autoload_running) = 1;
		class_name_len = Z_STRLEN_P(class_name);
		lc_name = zend_str_tolower_dup(Z_STRVAL_P(class_name), class_name_len);
		zend_hash_internal_pointer_reset_ex(SPL_G(autoload_functions), &function_pos);
		while(zend_hash_has_more_elements_ex(SPL_G(autoload_functions), &function_pos) == SUCCESS) {
			zend_hash_get_current_key_ex(SPL_G(autoload_functions), &func_name, &func_name_len, &dummy, 0, &function_pos);
			zend_hash_get_current_data_ex(SPL_G(autoload_functions), (void **) &alfi, &function_pos);
			zend_call_method(alfi->obj ? &alfi->obj : NULL, alfi->ce, &alfi->func_ptr, func_name, func_name_len, &retval, 1, class_name, NULL TSRMLS_CC);
			zend_exception_save(TSRMLS_C);
			if (retval) {
				zval_ptr_dtor(&retval);
				retval = NULL;
			}
			if (zend_hash_exists(EG(class_table), lc_name, class_name_len + 1)) {
				break;
			}
			zend_hash_move_forward_ex(SPL_G(autoload_functions), &function_pos);
		}
		zend_exception_restore(TSRMLS_C);
		efree(lc_name);
		SPL_G(autoload_running) = l_autoload_running;
	} else {
		/* do not use or overwrite &EG(autoload_func) here */
		zend_call_method_with_1_params(NULL, NULL, NULL, "spl_autoload", NULL, class_name);
	}