is_correct_endfilter (VerifyContext *ctx, guint offset)
{
	int i;
	MonoExceptionClause *clause;

	for (i = 0; i < ctx->header->num_clauses; ++i) {
		clause = &ctx->header->clauses [i];
		if (clause->flags != MONO_EXCEPTION_CLAUSE_FILTER)
			continue;
		if (MONO_OFFSET_IN_FILTER (clause, offset))
			return clause;
		if (!IS_STRICT_MODE (ctx) && MONO_OFFSET_IN_HANDLER (clause, offset))
			return clause;
	}
	return NULL;
}