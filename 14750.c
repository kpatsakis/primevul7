sourcing_a_script(exarg_T *eap)
{
    return (getline_equal(eap->getline, eap->cookie, getsourceline));
}