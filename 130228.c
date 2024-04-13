is_clause_inside_range (MonoExceptionClause *clause, guint32 start, guint32 end)
{
	if (clause->try_offset < start || (clause->try_offset + clause->try_len) > end)
		return FALSE;
	if (HANDLER_START (clause) < start || (clause->handler_offset + clause->handler_len) > end)
		return FALSE;
	return TRUE;
}