ex_behave(exarg_T *eap)
{
    if (STRCMP(eap->arg, "mswin") == 0)
    {
	set_option_value_give_err((char_u *)"selection",
						 0L, (char_u *)"exclusive", 0);
	set_option_value_give_err((char_u *)"selectmode",
						 0L, (char_u *)"mouse,key", 0);
	set_option_value_give_err((char_u *)"mousemodel",
						     0L, (char_u *)"popup", 0);
	set_option_value_give_err((char_u *)"keymodel",
					  0L, (char_u *)"startsel,stopsel", 0);
    }
    else if (STRCMP(eap->arg, "xterm") == 0)
    {
	set_option_value_give_err((char_u *)"selection",
						 0L, (char_u *)"inclusive", 0);
	set_option_value_give_err((char_u *)"selectmode", 0L, (char_u *)"", 0);
	set_option_value_give_err((char_u *)"mousemodel",
						    0L, (char_u *)"extend", 0);
	set_option_value_give_err((char_u *)"keymodel", 0L, (char_u *)"", 0);
    }
    else
	semsg(_(e_invalid_argument_str), eap->arg);
}