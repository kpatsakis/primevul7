static void pp_cleanup_session(void)
{
    nasm_free(use_loaded);
    free_llist(predef);
    predef = NULL;
    delete_Blocks();
    ipath_list = NULL;
}