/* {{{ date_period_it_dtor */
static void date_period_it_dtor(zend_object_iterator *iter TSRMLS_DC)
{
	date_period_it *iterator = (date_period_it *)iter;

	date_period_it_invalidate_current(iter TSRMLS_CC);

	zval_ptr_dtor(&iterator->date_period_zval);

	efree(iterator);