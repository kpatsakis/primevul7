str_coderange(VALUE str)
{
    int cr = ENC_CODERANGE(str);
    if (cr == ENC_CODERANGE_UNKNOWN) {
	cr = rb_enc_str_coderange(str);
    }
    return cr;
}