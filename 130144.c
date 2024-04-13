is_correct_endfinally (MonoMethodHeader *header, guint offset)
{
	int i;
	MonoExceptionClause *clause;

	for (i = 0; i < header->num_clauses; ++i) {
		clause = &header->clauses [i];
		if (MONO_OFFSET_IN_HANDLER (clause, offset) && (clause->flags == MONO_EXCEPTION_CLAUSE_FAULT || clause->flags == MONO_EXCEPTION_CLAUSE_FINALLY))
			return 1;
	}
	return 0;
}