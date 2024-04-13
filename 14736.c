find_script_in_rtp(char_u *name)
{
    int sid = -1;

    (void)do_in_path_and_pp(p_rtp, name, DIP_NOAFTER,
						   find_script_callback, &sid);
    return sid;
}