namev_to_backref_number(struct re_registers *regs, VALUE re, VALUE name)
{
    int num;

    switch (TYPE(name)) {
      case T_SYMBOL:
	name = rb_sym2str(name);
	/* fall through */
      case T_STRING:
	if (NIL_P(re) || !rb_enc_compatible(RREGEXP_SRC(re), name) ||
		(num = name_to_backref_number(regs, re,
					      RSTRING_PTR(name), RSTRING_END(name))) < 1) {
	    name_to_backref_error(name);
	}
	return num;

      default:
	return -1;
    }
}