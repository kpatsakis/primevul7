 Unregister given function as __autoload() implementation */
PHP_FUNCTION(spl_autoload_unregister)
{
	char *func_name, *error = NULL;
	int func_name_len;
	char *lc_name = NULL;
	zval *zcallable;
	int success = FAILURE;
	zend_function *spl_func_ptr;
	zval *obj_ptr;
	zend_fcall_info_cache fcc;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &zcallable) == FAILURE) {
		return;
	}

	if (!zend_is_callable_ex(zcallable, NULL, IS_CALLABLE_CHECK_SYNTAX_ONLY, &func_name, &func_name_len, &fcc, &error TSRMLS_CC)) {
		zend_throw_exception_ex(spl_ce_LogicException, 0 TSRMLS_CC, "Unable to unregister invalid function (%s)", error);
		if (error) {
			efree(error);
		}
		if (func_name) {
			efree(func_name);
		}
		RETURN_FALSE;
	}
	obj_ptr = fcc.object_ptr;
	if (error) {
		efree(error);
	}

	lc_name = safe_emalloc(func_name_len, 1, sizeof(long) + 1);
	zend_str_tolower_copy(lc_name, func_name, func_name_len);
	efree(func_name);

	if (Z_TYPE_P(zcallable) == IS_OBJECT) {
		lc_name = erealloc(lc_name, func_name_len + 2 + sizeof(zend_object_handle));
		memcpy(lc_name + func_name_len, &Z_OBJ_HANDLE_P(zcallable),
			sizeof(zend_object_handle));
		func_name_len += sizeof(zend_object_handle);
		lc_name[func_name_len] = '\0';
	}

	if (SPL_G(autoload_functions)) {
		if (func_name_len == sizeof("spl_autoload_call")-1 && !strcmp(lc_name, "spl_autoload_call")) {
			/* remove all */
			if (!SPL_G(autoload_running)) {
				zend_hash_destroy(SPL_G(autoload_functions));
				FREE_HASHTABLE(SPL_G(autoload_functions));
				SPL_G(autoload_functions) = NULL;
				EG(autoload_func) = NULL;
			} else {
				zend_hash_clean(SPL_G(autoload_functions));
			}
			success = SUCCESS;
		} else {
			/* remove specific */
			success = zend_hash_del(SPL_G(autoload_functions), lc_name, func_name_len+1);
			if (success != SUCCESS && obj_ptr) {
				lc_name = erealloc(lc_name, func_name_len + 2 + sizeof(zend_object_handle));
				memcpy(lc_name + func_name_len, &Z_OBJ_HANDLE_P(obj_ptr), sizeof(zend_object_handle));
				func_name_len += sizeof(zend_object_handle);
				lc_name[func_name_len] = '\0';
				success = zend_hash_del(SPL_G(autoload_functions), lc_name, func_name_len+1);
			}
		}
	} else if (func_name_len == sizeof("spl_autoload")-1 && !strcmp(lc_name, "spl_autoload")) {
		/* register single spl_autoload() */
		zend_hash_find(EG(function_table), "spl_autoload", sizeof("spl_autoload"), (void **) &spl_func_ptr);

		if (EG(autoload_func) == spl_func_ptr) {
			success = SUCCESS;
			EG(autoload_func) = NULL;
		}
	}

	efree(lc_name);
	RETURN_BOOL(success == SUCCESS);