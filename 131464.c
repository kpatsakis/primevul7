static void spl_array_update_pos(spl_array_object* intern) /* {{{ */
{
	Bucket *pos = intern->pos;
	if (pos != NULL) {
		intern->pos_h = pos->h;
	}
} /* }}} */