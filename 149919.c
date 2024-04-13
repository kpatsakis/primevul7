reg_operand(VALUE s, int check)
{
    if (SYMBOL_P(s)) {
	return rb_sym2str(s);
    }
    else {
	return (check ? rb_str_to_str : rb_check_string_type)(s);
    }
}