f_test_autochdir(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
#if defined(FEAT_AUTOCHDIR)
    test_autochdir = TRUE;
#endif
}