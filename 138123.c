/* {{{ date_interval_write_property */
void date_interval_write_property(zval *object, zval *member, zval *value, const zend_literal *key TSRMLS_DC)
{
	php_interval_obj *obj;
	zval tmp_member, tmp_value;

 	if (member->type != IS_STRING) {
		tmp_member = *member;
		zval_copy_ctor(&tmp_member);
		convert_to_string(&tmp_member);
		member = &tmp_member;
		key = NULL;
	}

	obj = (php_interval_obj *)zend_objects_get_address(object TSRMLS_CC);

	if (!obj->initialized) {
		(zend_get_std_object_handlers())->write_property(object, member, value, key TSRMLS_CC);
		if (member == &tmp_member) {
			zval_dtor(member);
		}
		return;
	}

#define SET_VALUE_FROM_STRUCT(n,m)            \
	if (strcmp(Z_STRVAL_P(member), m) == 0) { \
		if (value->type != IS_LONG) {         \
			tmp_value = *value;               \
			zval_copy_ctor(&tmp_value);       \
			convert_to_long(&tmp_value);      \
			value = &tmp_value;               \
		}                                     \
		obj->diff->n = Z_LVAL_P(value);       \
		if (value == &tmp_value) {            \
			zval_dtor(value);                 \
		}                                     \
		break;								  \
	}

	do {
		SET_VALUE_FROM_STRUCT(y, "y");
		SET_VALUE_FROM_STRUCT(m, "m");
		SET_VALUE_FROM_STRUCT(d, "d");
		SET_VALUE_FROM_STRUCT(h, "h");
		SET_VALUE_FROM_STRUCT(i, "i");
		SET_VALUE_FROM_STRUCT(s, "s");
		SET_VALUE_FROM_STRUCT(invert, "invert");
		/* didn't find any */
		(zend_get_std_object_handlers())->write_property(object, member, value, key TSRMLS_CC);
	} while(0);

	if (member == &tmp_member) {
		zval_dtor(member);
	}