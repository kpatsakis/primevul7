static inline void php_register_server_variables(TSRMLS_D)
{
	zval *array_ptr = NULL;

	ALLOC_ZVAL(array_ptr);
	array_init(array_ptr);
	INIT_PZVAL(array_ptr);
	if (PG(http_globals)[TRACK_VARS_SERVER]) {
		zval_ptr_dtor(&PG(http_globals)[TRACK_VARS_SERVER]);
	}
	PG(http_globals)[TRACK_VARS_SERVER] = array_ptr;

	/* Server variables */
	if (sapi_module.register_server_variables) {
		sapi_module.register_server_variables(array_ptr TSRMLS_CC);
	}

	/* PHP Authentication support */
	if (SG(request_info).auth_user) {
		php_register_variable("PHP_AUTH_USER", SG(request_info).auth_user, array_ptr TSRMLS_CC);
	}
	if (SG(request_info).auth_password) {
		php_register_variable("PHP_AUTH_PW", SG(request_info).auth_password, array_ptr TSRMLS_CC);
	}
	if (SG(request_info).auth_digest) {
		php_register_variable("PHP_AUTH_DIGEST", SG(request_info).auth_digest, array_ptr TSRMLS_CC);
	}
	/* store request init time */
	{
		zval request_time_float, request_time_long;
		Z_TYPE(request_time_float) = IS_DOUBLE;
		Z_DVAL(request_time_float) = sapi_get_request_time(TSRMLS_C);
		php_register_variable_ex("REQUEST_TIME_FLOAT", &request_time_float, array_ptr TSRMLS_CC);
		Z_TYPE(request_time_long) = IS_LONG;
		Z_LVAL(request_time_long) = zend_dval_to_lval(Z_DVAL(request_time_float));
		php_register_variable_ex("REQUEST_TIME", &request_time_long, array_ptr TSRMLS_CC);
	}

}