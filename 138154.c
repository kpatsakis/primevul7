/* {{{ date_interval_read_property */
zval *date_interval_read_property(zval *object, zval *member, int type, const zend_literal *key TSRMLS_DC)
{
	php_interval_obj *obj;
	zval *retval;
	zval tmp_member;
	timelib_sll value = -1;

 	if (member->type != IS_STRING) {
		tmp_member = *member;
		zval_copy_ctor(&tmp_member);
		convert_to_string(&tmp_member);
		member = &tmp_member;
		key = NULL;
	}

	obj = (php_interval_obj *)zend_objects_get_address(object TSRMLS_CC);

	if (!obj->initialized) {
		retval = (zend_get_std_object_handlers())->read_property(object, member, type, key TSRMLS_CC);
		if (member == &tmp_member) {
			zval_dtor(member);
		}
		return retval;
	}

#define GET_VALUE_FROM_STRUCT(n,m)            \
	if (strcmp(Z_STRVAL_P(member), m) == 0) { \
		value = obj->diff->n;                 \
		break;								  \
	}
	do {
		GET_VALUE_FROM_STRUCT(y, "y");
		GET_VALUE_FROM_STRUCT(m, "m");
		GET_VALUE_FROM_STRUCT(d, "d");
		GET_VALUE_FROM_STRUCT(h, "h");
		GET_VALUE_FROM_STRUCT(i, "i");
		GET_VALUE_FROM_STRUCT(s, "s");
		GET_VALUE_FROM_STRUCT(invert, "invert");
		GET_VALUE_FROM_STRUCT(days, "days");
		/* didn't find any */
		retval = (zend_get_std_object_handlers())->read_property(object, member, type, key TSRMLS_CC);

		if (member == &tmp_member) {
			zval_dtor(member);
		}

		return retval;
	} while(0);

	ALLOC_INIT_ZVAL(retval);
	Z_SET_REFCOUNT_P(retval, 0);

	if (value != -99999) {
		ZVAL_LONG(retval, value);
	} else {
		ZVAL_FALSE(retval);
	}

	if (member == &tmp_member) {
		zval_dtor(member);
	}

	return retval;