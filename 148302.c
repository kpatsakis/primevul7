ZEND_API int ZEND_FASTCALL compare_function(zval *result, zval *op1, zval *op2) /* {{{ */
{
	int ret;
	int converted = 0;
	zval op1_copy, op2_copy;
	zval *op_free, tmp_free;

	while (1) {
		switch (TYPE_PAIR(Z_TYPE_P(op1), Z_TYPE_P(op2))) {
			case TYPE_PAIR(IS_LONG, IS_LONG):
				ZVAL_LONG(result, Z_LVAL_P(op1)>Z_LVAL_P(op2)?1:(Z_LVAL_P(op1)<Z_LVAL_P(op2)?-1:0));
				return SUCCESS;

			case TYPE_PAIR(IS_DOUBLE, IS_LONG):
				Z_DVAL_P(result) = Z_DVAL_P(op1) - (double)Z_LVAL_P(op2);
				ZVAL_LONG(result, ZEND_NORMALIZE_BOOL(Z_DVAL_P(result)));
				return SUCCESS;

			case TYPE_PAIR(IS_LONG, IS_DOUBLE):
				Z_DVAL_P(result) = (double)Z_LVAL_P(op1) - Z_DVAL_P(op2);
				ZVAL_LONG(result, ZEND_NORMALIZE_BOOL(Z_DVAL_P(result)));
				return SUCCESS;

			case TYPE_PAIR(IS_DOUBLE, IS_DOUBLE):
				if (Z_DVAL_P(op1) == Z_DVAL_P(op2)) {
					ZVAL_LONG(result, 0);
				} else {
					Z_DVAL_P(result) = Z_DVAL_P(op1) - Z_DVAL_P(op2);
					ZVAL_LONG(result, ZEND_NORMALIZE_BOOL(Z_DVAL_P(result)));
				}
				return SUCCESS;

			case TYPE_PAIR(IS_ARRAY, IS_ARRAY):
				ZVAL_LONG(result, zend_compare_arrays(op1, op2));
				return SUCCESS;

			case TYPE_PAIR(IS_NULL, IS_NULL):
			case TYPE_PAIR(IS_NULL, IS_FALSE):
			case TYPE_PAIR(IS_FALSE, IS_NULL):
			case TYPE_PAIR(IS_FALSE, IS_FALSE):
			case TYPE_PAIR(IS_TRUE, IS_TRUE):
				ZVAL_LONG(result, 0);
				return SUCCESS;

			case TYPE_PAIR(IS_NULL, IS_TRUE):
				ZVAL_LONG(result, -1);
				return SUCCESS;

			case TYPE_PAIR(IS_TRUE, IS_NULL):
				ZVAL_LONG(result, 1);
				return SUCCESS;

			case TYPE_PAIR(IS_STRING, IS_STRING):
				if (Z_STR_P(op1) == Z_STR_P(op2)) {
					ZVAL_LONG(result, 0);
					return SUCCESS;
				}
				ZVAL_LONG(result, zendi_smart_strcmp(Z_STR_P(op1), Z_STR_P(op2)));
				return SUCCESS;

			case TYPE_PAIR(IS_NULL, IS_STRING):
				ZVAL_LONG(result, Z_STRLEN_P(op2) == 0 ? 0 : -1);
				return SUCCESS;

			case TYPE_PAIR(IS_STRING, IS_NULL):
				ZVAL_LONG(result, Z_STRLEN_P(op1) == 0 ? 0 : 1);
				return SUCCESS;

			case TYPE_PAIR(IS_OBJECT, IS_NULL):
				ZVAL_LONG(result, 1);
				return SUCCESS;

			case TYPE_PAIR(IS_NULL, IS_OBJECT):
				ZVAL_LONG(result, -1);
				return SUCCESS;

			default:
				if (Z_ISREF_P(op1)) {
					op1 = Z_REFVAL_P(op1);
					continue;
				} else if (Z_ISREF_P(op2)) {
					op2 = Z_REFVAL_P(op2);
					continue;
				}

				if (Z_TYPE_P(op1) == IS_OBJECT && Z_OBJ_HANDLER_P(op1, compare)) {
					ret = Z_OBJ_HANDLER_P(op1, compare)(result, op1, op2);
					if (UNEXPECTED(Z_TYPE_P(result) != IS_LONG)) {
						convert_compare_result_to_long(result);
					}
					return ret;
				} else if (Z_TYPE_P(op2) == IS_OBJECT && Z_OBJ_HANDLER_P(op2, compare)) {
					ret = Z_OBJ_HANDLER_P(op2, compare)(result, op1, op2);
					if (UNEXPECTED(Z_TYPE_P(result) != IS_LONG)) {
						convert_compare_result_to_long(result);
					}
					return ret;
				}

				if (Z_TYPE_P(op1) == IS_OBJECT && Z_TYPE_P(op2) == IS_OBJECT) {
					if (Z_OBJ_P(op1) == Z_OBJ_P(op2)) {
						/* object handles are identical, apparently this is the same object */
						ZVAL_LONG(result, 0);
						return SUCCESS;
					}
					if (Z_OBJ_HANDLER_P(op1, compare_objects) == Z_OBJ_HANDLER_P(op2, compare_objects)) {
						ZVAL_LONG(result, Z_OBJ_HANDLER_P(op1, compare_objects)(op1, op2));
						return SUCCESS;
					}
				}
				if (Z_TYPE_P(op1) == IS_OBJECT) {
					if (Z_OBJ_HT_P(op1)->get) {
						zval rv;
						op_free = Z_OBJ_HT_P(op1)->get(op1, &rv);
						ret = compare_function(result, op_free, op2);
						zend_free_obj_get_result(op_free);
						return ret;
					} else if (Z_TYPE_P(op2) != IS_OBJECT && Z_OBJ_HT_P(op1)->cast_object) {
						ZVAL_UNDEF(&tmp_free);
						if (Z_OBJ_HT_P(op1)->cast_object(op1, &tmp_free, ((Z_TYPE_P(op2) == IS_FALSE || Z_TYPE_P(op2) == IS_TRUE) ? _IS_BOOL : Z_TYPE_P(op2))) == FAILURE) {
							ZVAL_LONG(result, 1);
							zend_free_obj_get_result(&tmp_free);
							return SUCCESS;
						}
						ret = compare_function(result, &tmp_free, op2);
						zend_free_obj_get_result(&tmp_free);
						return ret;
					}
				}
				if (Z_TYPE_P(op2) == IS_OBJECT) {
					if (Z_OBJ_HT_P(op2)->get) {
						zval rv;
						op_free = Z_OBJ_HT_P(op2)->get(op2, &rv);
						ret = compare_function(result, op1, op_free);
						zend_free_obj_get_result(op_free);
						return ret;
					} else if (Z_TYPE_P(op1) != IS_OBJECT && Z_OBJ_HT_P(op2)->cast_object) {
						ZVAL_UNDEF(&tmp_free);
						if (Z_OBJ_HT_P(op2)->cast_object(op2, &tmp_free, ((Z_TYPE_P(op1) == IS_FALSE || Z_TYPE_P(op1) == IS_TRUE) ? _IS_BOOL : Z_TYPE_P(op1))) == FAILURE) {
							ZVAL_LONG(result, -1);
							zend_free_obj_get_result(&tmp_free);
							return SUCCESS;
						}
						ret = compare_function(result, op1, &tmp_free);
						zend_free_obj_get_result(&tmp_free);
						return ret;
					} else if (Z_TYPE_P(op1) == IS_OBJECT) {
						ZVAL_LONG(result, 1);
						return SUCCESS;
					}
				}
				if (!converted) {
					if (Z_TYPE_P(op1) < IS_TRUE) {
						ZVAL_LONG(result, zval_is_true(op2) ? -1 : 0);
						return SUCCESS;
					} else if (Z_TYPE_P(op1) == IS_TRUE) {
						ZVAL_LONG(result, zval_is_true(op2) ? 0 : 1);
						return SUCCESS;
					} else if (Z_TYPE_P(op2) < IS_TRUE) {
						ZVAL_LONG(result, zval_is_true(op1) ? 1 : 0);
						return SUCCESS;
					} else if (Z_TYPE_P(op2) == IS_TRUE) {
						ZVAL_LONG(result, zval_is_true(op1) ? 0 : -1);
						return SUCCESS;
					} else {
						op1 = zendi_convert_scalar_to_number(op1, &op1_copy, result, 1);
						op2 = zendi_convert_scalar_to_number(op2, &op2_copy, result, 1);
						if (EG(exception)) {
							if (result != op1) {
								ZVAL_UNDEF(result);
							}
							return FAILURE;
						}
						converted = 1;
					}
				} else if (Z_TYPE_P(op1)==IS_ARRAY) {
					ZVAL_LONG(result, 1);
					return SUCCESS;
				} else if (Z_TYPE_P(op2)==IS_ARRAY) {
					ZVAL_LONG(result, -1);
					return SUCCESS;
				} else {
					ZEND_ASSERT(0);
					zend_throw_error(NULL, "Unsupported operand types");
					if (result != op1) {
						ZVAL_UNDEF(result);
					}
					return FAILURE;
				}
		}
	}
}