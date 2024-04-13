/* Helper function used to add an associative array of warnings and errors to a zval */
static void zval_from_error_container(zval *z, timelib_error_container *error)
{
	int   i;
	zval *element;

	add_assoc_long(z, "warning_count", error->warning_count);
	MAKE_STD_ZVAL(element);
	array_init(element);
	for (i = 0; i < error->warning_count; i++) {
		add_index_string(element, error->warning_messages[i].position, error->warning_messages[i].message, 1);
	}
	add_assoc_zval(z, "warnings", element);

	add_assoc_long(z, "error_count", error->error_count);
	MAKE_STD_ZVAL(element);
	array_init(element);
	for (i = 0; i < error->error_count; i++) {
		add_index_string(element, error->error_messages[i].position, error->error_messages[i].message, 1);
	}
	add_assoc_zval(z, "errors", element);