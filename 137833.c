 Return all registered __autoload() functionns */
PHP_FUNCTION(spl_autoload_functions)
{
	zend_function *fptr;
	HashPosition function_pos;
	autoload_func_info *alfi;

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}
	
	if (!EG(autoload_func)) {
		if (zend_hash_find(EG(function_table), ZEND_AUTOLOAD_FUNC_NAME, sizeof(ZEND_AUTOLOAD_FUNC_NAME), (void **) &fptr) == SUCCESS) {
			array_init(return_value);
			add_next_index_stringl(return_value, ZEND_AUTOLOAD_FUNC_NAME, sizeof(ZEND_AUTOLOAD_FUNC_NAME)-1, 1);
			return;
		}
		RETURN_FALSE;
	}

	zend_hash_find(EG(function_table), "spl_autoload_call", sizeof("spl_autoload_call"), (void **) &fptr);

	if (EG(autoload_func) == fptr) {
		array_init(return_value);
		zend_hash_internal_pointer_reset_ex(SPL_G(autoload_functions), &function_pos);
		while(zend_hash_has_more_elements_ex(SPL_G(autoload_functions), &function_pos) == SUCCESS) {
			zend_hash_get_current_data_ex(SPL_G(autoload_functions), (void **) &alfi, &function_pos);
			if (alfi->closure) {
				Z_ADDREF_P(alfi->closure);
				add_next_index_zval(return_value, alfi->closure);
			} else if (alfi->func_ptr->common.scope) {
				zval *tmp;
				MAKE_STD_ZVAL(tmp);
				array_init(tmp);

				if (alfi->obj) {
					Z_ADDREF_P(alfi->obj);
					add_next_index_zval(tmp, alfi->obj);
				} else {
					add_next_index_string(tmp, alfi->ce->name, 1);
				}
				add_next_index_string(tmp, alfi->func_ptr->common.function_name, 1);
				add_next_index_zval(return_value, tmp);
			} else {
				if (strncmp(alfi->func_ptr->common.function_name, "__lambda_func", sizeof("__lambda_func") - 1)) {
					add_next_index_string(return_value, alfi->func_ptr->common.function_name, 1);
				} else {
				   char *key;
				   uint len;
				   long dummy;
				   zend_hash_get_current_key_ex(SPL_G(autoload_functions), &key, &len, &dummy, 0, &function_pos); 
				   add_next_index_stringl(return_value, key, len - 1, 1);
				}
			}

			zend_hash_move_forward_ex(SPL_G(autoload_functions), &function_pos);
		}
		return;
	}

	array_init(return_value);
	add_next_index_string(return_value, EG(autoload_func)->common.function_name, 1);