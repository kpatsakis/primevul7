is_unverifiable_endfilter (VerifyContext *ctx, guint offset)
{
	int i;
	MonoExceptionClause *clause;

	for (i = 0; i < ctx->header->num_clauses; ++i) {
		clause = &ctx->header->clauses [i];
		if (MONO_OFFSET_IN_CLAUSE (clause, offset))
			return 1;
	}
	return 0;
}