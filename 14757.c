add_pack_start_dirs(void)
{
    do_in_path(p_pp, (char_u *)"pack/*/start/*", DIP_ALL + DIP_DIR,
					       add_pack_plugin, &APP_ADD_DIR);
}