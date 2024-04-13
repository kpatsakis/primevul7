match_backref_number(VALUE match, VALUE backref)
{
    const char *name;
    int num;

    struct re_registers *regs = RMATCH_REGS(match);
    VALUE regexp = RMATCH(match)->regexp;

    match_check(match);
    switch (TYPE(backref)) {
      default:
        return NUM2INT(backref);

      case T_SYMBOL:
	backref = rb_sym2str(backref);
	/* fall through */

      case T_STRING:
        name = StringValueCStr(backref);
        break;
    }

    num = onig_name_to_backref_number(RREGEXP_PTR(regexp),
              (const unsigned char*)name,
              (const unsigned char*)name + strlen(name),
              regs);

    if (num < 1) {
        rb_raise(rb_eIndexError, "undefined group name reference: %s", name);
    }

    return num;
}