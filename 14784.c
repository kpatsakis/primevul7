load_start_packages(void)
{
    did_source_packages = TRUE;
    do_in_path(p_pp, (char_u *)"pack/*/start/*", DIP_ALL + DIP_DIR,
						  add_pack_plugin, &APP_LOAD);
}