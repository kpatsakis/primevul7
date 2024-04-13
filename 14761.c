source_finished(
    char_u	*(*fgetline)(int, void *, int, getline_opt_T),
    void	*cookie)
{
    return (getline_equal(fgetline, cookie, getsourceline)
	    && ((source_cookie_T *)getline_cookie(
						fgetline, cookie))->finished);
}