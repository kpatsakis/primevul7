mono_generic_param_is_constraint_compatible (VerifyContext *ctx, MonoGenericParam *target, MonoGenericParam *candidate, MonoClass *candidate_param_class, MonoGenericContext *context)
{
	MonoGenericParamInfo *tinfo = mono_generic_param_info (target);
	MonoGenericParamInfo *cinfo = mono_generic_param_info (candidate);

	int tmask = tinfo->flags & GENERIC_PARAMETER_ATTRIBUTE_SPECIAL_CONSTRAINTS_MASK;
	int cmask = cinfo->flags & GENERIC_PARAMETER_ATTRIBUTE_SPECIAL_CONSTRAINTS_MASK;
	if ((tmask & cmask) != tmask)
		return FALSE;

	if (tinfo->constraints) {
		MonoClass **target_class, **candidate_class;
		for (target_class = tinfo->constraints; *target_class; ++target_class) {
			MonoClass *tc;
			MonoType *inflated = verifier_inflate_type (ctx, &(*target_class)->byval_arg, context);
			if (!inflated)
				return FALSE;
			tc = mono_class_from_mono_type (inflated);
			mono_metadata_free_type (inflated);

			/*
			 * A constraint from @target might inflate into @candidate itself and in that case we don't need
			 * check it's constraints since it satisfy the constraint by itself.
			 */
			if (mono_metadata_type_equal (&tc->byval_arg, &candidate_param_class->byval_arg))
				continue;

			if (!cinfo->constraints)
				return FALSE;

			for (candidate_class = cinfo->constraints; *candidate_class; ++candidate_class) {
				MonoClass *cc;
				inflated = verifier_inflate_type (ctx, &(*candidate_class)->byval_arg, ctx->generic_context);
				if (!inflated)
					return FALSE;
				cc = mono_class_from_mono_type (inflated);
				mono_metadata_free_type (inflated);

				if (mono_class_is_assignable_from (tc, cc))
					break;
			}
			if (!*candidate_class)
				return FALSE;
		}
	}
	return TRUE;
}