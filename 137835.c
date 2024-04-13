 Register given function as __autoload() implementation */
PHP_FUNCTION(spl_autoload_register)
{
	char *func_name, *error = NULL;
	int  func_name_len;
	char *lc_name = NULL;
	zval *zcallable = NULL;
	zend_bool do_throw = 1;
	zend_bool prepend  = 0;
	zend_function *spl_func_ptr;
	autoload_func_info alfi;
	zval *obj_ptr;
	zend_fcall_info_cache fcc;

	if (zend_parse_parameters_ex(ZEND_PARSE_PARAMS_QUIET, ZEND_NUM_ARGS() TSRMLS_CC, "|zbb", &zcallable, &do_throw, &prepend) == FAILURE) {
		return;
	}

	if (ZEND_NUM_ARGS()) {
		if (Z_TYPE_P(zcallable) == IS_STRING) {
			if (Z_STRLEN_P(zcallable) == sizeof("spl_autoload_call") - 1) {
				if (!zend_binary_strcasecmp(Z_STRVAL_P(zcallable), sizeof("spl_autoload_call"), "spl_autoload_call", sizeof("spl_autoload_call"))) {
					if (do_throw) {
						zend_throw_exception_ex(spl_ce_LogicException, 0 TSRMLS_CC, "Function spl_autoload_call() cannot be registered");
					}
					RETURN_FALSE;
				}
			}
		}
	
		if (!zend_is_callable_ex(zcallable, NULL, IS_CALLABLE_STRICT, &func_name, &func_name_len, &fcc, &error TSRMLS_CC)) {
			alfi.ce = fcc.calling_scope;
			alfi.func_ptr = fcc.function_handler;
			obj_ptr = fcc.object_ptr;
			if (Z_TYPE_P(zcallable) == IS_ARRAY) {
				if (!obj_ptr && alfi.func_ptr && !(alfi.func_ptr->common.fn_flags & ZEND_ACC_STATIC)) {
					if (do_throw) {
						zend_throw_exception_ex(spl_ce_LogicException, 0 TSRMLS_CC, "Passed array specifies a non static method but no object (%s)", error);
					}
					if (error) {
						efree(error);
					}
					efree(func_name);
					RETURN_FALSE;
				}
				else if (do_throw) {
					zend_throw_exception_ex(spl_ce_LogicException, 0 TSRMLS_CC, "Passed array does not specify %s %smethod (%s)", alfi.func_ptr ? "a callable" : "an existing", !obj_ptr ? "static " : "", error);
				}
				if (error) {
					efree(error);
				}
				efree(func_name);
				RETURN_FALSE;
			} else if (Z_TYPE_P(zcallable) == IS_STRING) {
				if (do_throw) {
					zend_throw_exception_ex(spl_ce_LogicException, 0 TSRMLS_CC, "Function '%s' not %s (%s)", func_name, alfi.func_ptr ? "callable" : "found", error);
				}
				if (error) {
					efree(error);
				}
				efree(func_name);
				RETURN_FALSE;
			} else {
				if (do_throw) {
					zend_throw_exception_ex(spl_ce_LogicException, 0 TSRMLS_CC, "Illegal value passed (%s)", error);
				}
				if (error) {
					efree(error);
				}
				efree(func_name);
				RETURN_FALSE;
			}
		}
		alfi.closure = NULL;
		alfi.ce = fcc.calling_scope;
		alfi.func_ptr = fcc.function_handler;
		obj_ptr = fcc.object_ptr;
		if (error) {
			efree(error);
		}
	
		lc_name = safe_emalloc(func_name_len, 1, sizeof(long) + 1);
		zend_str_tolower_copy(lc_name, func_name, func_name_len);
		efree(func_name);

		if (Z_TYPE_P(zcallable) == IS_OBJECT) {
			alfi.closure = zcallable;
			Z_ADDREF_P(zcallable);

			lc_name = erealloc(lc_name, func_name_len + 2 + sizeof(zend_object_handle));
			memcpy(lc_name + func_name_len, &Z_OBJ_HANDLE_P(zcallable),
				sizeof(zend_object_handle));
			func_name_len += sizeof(zend_object_handle);
			lc_name[func_name_len] = '\0';
		}

		if (SPL_G(autoload_functions) && zend_hash_exists(SPL_G(autoload_functions), (char*)lc_name, func_name_len+1)) {
			if (alfi.closure) {
				Z_DELREF_P(zcallable);
			}
			goto skip;
		}

		if (obj_ptr && !(alfi.func_ptr->common.fn_flags & ZEND_ACC_STATIC)) {
			/* add object id to the hash to ensure uniqueness, for more reference look at bug #40091 */
			lc_name = erealloc(lc_name, func_name_len + 2 + sizeof(zend_object_handle));
			memcpy(lc_name + func_name_len, &Z_OBJ_HANDLE_P(obj_ptr), sizeof(zend_object_handle));
			func_name_len += sizeof(zend_object_handle);
			lc_name[func_name_len] = '\0';
			alfi.obj = obj_ptr;
			Z_ADDREF_P(alfi.obj);
		} else {
			alfi.obj = NULL;
		}

		if (!SPL_G(autoload_functions)) {
			ALLOC_HASHTABLE(SPL_G(autoload_functions));
			zend_hash_init(SPL_G(autoload_functions), 1, NULL, (dtor_func_t) autoload_func_info_dtor, 0);
		}

		zend_hash_find(EG(function_table), "spl_autoload", sizeof("spl_autoload"), (void **) &spl_func_ptr);

		if (EG(autoload_func) == spl_func_ptr) { /* registered already, so we insert that first */
			autoload_func_info spl_alfi;

			spl_alfi.func_ptr = spl_func_ptr;
			spl_alfi.obj = NULL;
			spl_alfi.ce = NULL;
			spl_alfi.closure = NULL;
			zend_hash_add(SPL_G(autoload_functions), "spl_autoload", sizeof("spl_autoload"), &spl_alfi, sizeof(autoload_func_info), NULL);
			if (prepend && SPL_G(autoload_functions)->nNumOfElements > 1) {
				/* Move the newly created element to the head of the hashtable */
				HT_MOVE_TAIL_TO_HEAD(SPL_G(autoload_functions));
			}
		}

		if (zend_hash_add(SPL_G(autoload_functions), lc_name, func_name_len+1, &alfi.func_ptr, sizeof(autoload_func_info), NULL) == FAILURE) {
			if (obj_ptr && !(alfi.func_ptr->common.fn_flags & ZEND_ACC_STATIC)) {
				Z_DELREF_P(alfi.obj);
			}				
			if (alfi.closure) {
				Z_DELREF_P(alfi.closure);
			}
		}
		if (prepend && SPL_G(autoload_functions)->nNumOfElements > 1) {
			/* Move the newly created element to the head of the hashtable */
			HT_MOVE_TAIL_TO_HEAD(SPL_G(autoload_functions));
		}
skip:
		efree(lc_name);
	}

	if (SPL_G(autoload_functions)) {
		zend_hash_find(EG(function_table), "spl_autoload_call", sizeof("spl_autoload_call"), (void **) &EG(autoload_func));
	} else {
		zend_hash_find(EG(function_table), "spl_autoload", sizeof("spl_autoload"), (void **) &EG(autoload_func));
	}
	RETURN_TRUE;