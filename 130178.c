is_valid_branch_instruction (MonoMethodHeader *header, guint offset, guint target)
{
	int i;
	MonoExceptionClause *clause;

	for (i = 0; i < header->num_clauses; ++i) {
		clause = &header->clauses [i];
		/*branching into a finally block is invalid*/
		if ((clause->flags == MONO_EXCEPTION_CLAUSE_FINALLY || clause->flags == MONO_EXCEPTION_CLAUSE_FAULT) &&
			!MONO_OFFSET_IN_HANDLER (clause, offset) &&
			MONO_OFFSET_IN_HANDLER (clause, target))
			return 2;

		if (clause->try_offset != target && (MONO_OFFSET_IN_CLAUSE (clause, offset) ^ MONO_OFFSET_IN_CLAUSE (clause, target)))
			return 1;
		if (MONO_OFFSET_IN_HANDLER (clause, offset) ^ MONO_OFFSET_IN_HANDLER (clause, target))
			return 1;
		if (MONO_OFFSET_IN_FILTER (clause, offset) ^ MONO_OFFSET_IN_FILTER (clause, target))
			return 1;
	}
	return 0;
}