/* {{{ date_period_it_move_forward */
static void date_period_it_move_forward(zend_object_iterator *iter TSRMLS_DC)
{
	date_period_it   *iterator = (date_period_it *)iter;

	iterator->current_index++;
	date_period_it_invalidate_current(iter TSRMLS_CC);