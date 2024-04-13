static void spl_array_set_pos(spl_array_object* intern, HashPosition pos) /* {{{ */
{
	intern->pos = pos;
	spl_array_update_pos(intern);
} /* }}} */