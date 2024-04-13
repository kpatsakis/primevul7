firstComponentNormalize(
	slap_mask_t usage,
	Syntax *syntax,
	MatchingRule *mr,
	struct berval *val,
	struct berval *normalized,
	void *ctx )
{
	int rc;
	struct berval comp;
	ber_len_t len;

	if( SLAP_MR_IS_VALUE_OF_ASSERTION_SYNTAX( usage )) {
		ber_dupbv_x( normalized, val, ctx );
		return LDAP_SUCCESS;
	}

	if( val->bv_len < 3 ) return LDAP_INVALID_SYNTAX;

	if( ! ( val->bv_val[0] == '(' /*')'*/
			&& val->bv_val[val->bv_len - 1] == /*'('*/ ')' )
		&& ! ( val->bv_val[0] == '{' /*'}'*/
			&& val->bv_val[val->bv_len - 1] == /*'('*/ '}' ) )
	{
		return LDAP_INVALID_SYNTAX;
	}

	/* trim leading white space */
	for( len=1;
		len < val->bv_len && ASCII_SPACE(val->bv_val[len]);
		len++ )
	{
		/* empty */
	}

	/* grab next word */
	comp.bv_val = &val->bv_val[len];
	len = val->bv_len - len - STRLENOF(/*"{"*/ "}");
	for( comp.bv_len = 0;
		!ASCII_SPACE(comp.bv_val[comp.bv_len]) && comp.bv_len < len;
		comp.bv_len++ )
	{
		/* empty */
	}

	if( mr == slap_schema.si_mr_objectIdentifierFirstComponentMatch ) {
		rc = numericoidValidate( NULL, &comp );
	} else if( mr == slap_schema.si_mr_integerFirstComponentMatch ) {
		rc = integerValidate( NULL, &comp );
	} else {
		rc = LDAP_INVALID_SYNTAX;
	}
	

	if( rc == LDAP_SUCCESS ) {
		ber_dupbv_x( normalized, &comp, ctx );
	}

	return rc;
}