void
Perl_init_uniprops(pTHX)
{
    dVAR;

    PL_user_def_props = newHV();

#ifdef USE_ITHREADS

    HvSHAREKEYS_off(PL_user_def_props);
    PL_user_def_props_aTHX = aTHX;

#endif

    /* Set up the inversion list global variables */

    PL_XPosix_ptrs[_CC_ASCII] = _new_invlist_C_array(uni_prop_ptrs[UNI_ASCII]);
    PL_XPosix_ptrs[_CC_ALPHANUMERIC] = _new_invlist_C_array(uni_prop_ptrs[UNI_XPOSIXALNUM]);
    PL_XPosix_ptrs[_CC_ALPHA] = _new_invlist_C_array(uni_prop_ptrs[UNI_XPOSIXALPHA]);
    PL_XPosix_ptrs[_CC_BLANK] = _new_invlist_C_array(uni_prop_ptrs[UNI_XPOSIXBLANK]);
    PL_XPosix_ptrs[_CC_CASED] =  _new_invlist_C_array(uni_prop_ptrs[UNI_CASED]);
    PL_XPosix_ptrs[_CC_CNTRL] = _new_invlist_C_array(uni_prop_ptrs[UNI_XPOSIXCNTRL]);
    PL_XPosix_ptrs[_CC_DIGIT] = _new_invlist_C_array(uni_prop_ptrs[UNI_XPOSIXDIGIT]);
    PL_XPosix_ptrs[_CC_GRAPH] = _new_invlist_C_array(uni_prop_ptrs[UNI_XPOSIXGRAPH]);
    PL_XPosix_ptrs[_CC_LOWER] = _new_invlist_C_array(uni_prop_ptrs[UNI_XPOSIXLOWER]);
    PL_XPosix_ptrs[_CC_PRINT] = _new_invlist_C_array(uni_prop_ptrs[UNI_XPOSIXPRINT]);
    PL_XPosix_ptrs[_CC_PUNCT] = _new_invlist_C_array(uni_prop_ptrs[UNI_XPOSIXPUNCT]);
    PL_XPosix_ptrs[_CC_SPACE] = _new_invlist_C_array(uni_prop_ptrs[UNI_XPOSIXSPACE]);
    PL_XPosix_ptrs[_CC_UPPER] = _new_invlist_C_array(uni_prop_ptrs[UNI_XPOSIXUPPER]);
    PL_XPosix_ptrs[_CC_VERTSPACE] = _new_invlist_C_array(uni_prop_ptrs[UNI_VERTSPACE]);
    PL_XPosix_ptrs[_CC_WORDCHAR] = _new_invlist_C_array(uni_prop_ptrs[UNI_XPOSIXWORD]);
    PL_XPosix_ptrs[_CC_XDIGIT] = _new_invlist_C_array(uni_prop_ptrs[UNI_XPOSIXXDIGIT]);

    PL_Posix_ptrs[_CC_ASCII] = _new_invlist_C_array(uni_prop_ptrs[UNI_ASCII]);
    PL_Posix_ptrs[_CC_ALPHANUMERIC] = _new_invlist_C_array(uni_prop_ptrs[UNI_POSIXALNUM]);
    PL_Posix_ptrs[_CC_ALPHA] = _new_invlist_C_array(uni_prop_ptrs[UNI_POSIXALPHA]);
    PL_Posix_ptrs[_CC_BLANK] = _new_invlist_C_array(uni_prop_ptrs[UNI_POSIXBLANK]);
    PL_Posix_ptrs[_CC_CASED] = PL_Posix_ptrs[_CC_ALPHA];
    PL_Posix_ptrs[_CC_CNTRL] = _new_invlist_C_array(uni_prop_ptrs[UNI_POSIXCNTRL]);
    PL_Posix_ptrs[_CC_DIGIT] = _new_invlist_C_array(uni_prop_ptrs[UNI_POSIXDIGIT]);
    PL_Posix_ptrs[_CC_GRAPH] = _new_invlist_C_array(uni_prop_ptrs[UNI_POSIXGRAPH]);
    PL_Posix_ptrs[_CC_LOWER] = _new_invlist_C_array(uni_prop_ptrs[UNI_POSIXLOWER]);
    PL_Posix_ptrs[_CC_PRINT] = _new_invlist_C_array(uni_prop_ptrs[UNI_POSIXPRINT]);
    PL_Posix_ptrs[_CC_PUNCT] = _new_invlist_C_array(uni_prop_ptrs[UNI_POSIXPUNCT]);
    PL_Posix_ptrs[_CC_SPACE] = _new_invlist_C_array(uni_prop_ptrs[UNI_POSIXSPACE]);
    PL_Posix_ptrs[_CC_UPPER] = _new_invlist_C_array(uni_prop_ptrs[UNI_POSIXUPPER]);
    PL_Posix_ptrs[_CC_VERTSPACE] = NULL;
    PL_Posix_ptrs[_CC_WORDCHAR] = _new_invlist_C_array(uni_prop_ptrs[UNI_POSIXWORD]);
    PL_Posix_ptrs[_CC_XDIGIT] = _new_invlist_C_array(uni_prop_ptrs[UNI_POSIXXDIGIT]);

    PL_GCB_invlist = _new_invlist_C_array(_Perl_GCB_invlist);
    PL_SB_invlist = _new_invlist_C_array(_Perl_SB_invlist);
    PL_WB_invlist = _new_invlist_C_array(_Perl_WB_invlist);
    PL_LB_invlist = _new_invlist_C_array(_Perl_LB_invlist);
    PL_SCX_invlist = _new_invlist_C_array(_Perl_SCX_invlist);

    PL_AboveLatin1 = _new_invlist_C_array(AboveLatin1_invlist);
    PL_Latin1 = _new_invlist_C_array(Latin1_invlist);
    PL_UpperLatin1 = _new_invlist_C_array(UpperLatin1_invlist);

    PL_Assigned_invlist = _new_invlist_C_array(uni_prop_ptrs[UNI_ASSIGNED]);

    PL_utf8_perl_idstart = _new_invlist_C_array(uni_prop_ptrs[UNI__PERL_IDSTART]);
    PL_utf8_perl_idcont = _new_invlist_C_array(uni_prop_ptrs[UNI__PERL_IDCONT]);

    PL_utf8_charname_begin = _new_invlist_C_array(uni_prop_ptrs[UNI__PERL_CHARNAME_BEGIN]);
    PL_utf8_charname_continue = _new_invlist_C_array(uni_prop_ptrs[UNI__PERL_CHARNAME_CONTINUE]);

    PL_in_some_fold = _new_invlist_C_array(uni_prop_ptrs[UNI__PERL_ANY_FOLDS]);
    PL_HasMultiCharFold = _new_invlist_C_array(uni_prop_ptrs[
                                            UNI__PERL_FOLDS_TO_MULTI_CHAR]);
    PL_InMultiCharFold = _new_invlist_C_array(uni_prop_ptrs[
                                            UNI__PERL_IS_IN_MULTI_CHAR_FOLD]);
    PL_NonFinalFold = _new_invlist_C_array(uni_prop_ptrs[
                                            UNI__PERL_NON_FINAL_FOLDS]);

    PL_utf8_toupper = _new_invlist_C_array(Uppercase_Mapping_invlist);
    PL_utf8_tolower = _new_invlist_C_array(Lowercase_Mapping_invlist);
    PL_utf8_totitle = _new_invlist_C_array(Titlecase_Mapping_invlist);
    PL_utf8_tofold = _new_invlist_C_array(Case_Folding_invlist);
    PL_utf8_tosimplefold = _new_invlist_C_array(Simple_Case_Folding_invlist);
    PL_utf8_foldclosures = _new_invlist_C_array(_Perl_IVCF_invlist);
    PL_utf8_mark = _new_invlist_C_array(uni_prop_ptrs[UNI_M]);
    PL_CCC_non0_non230 = _new_invlist_C_array(_Perl_CCC_non0_non230_invlist);
    PL_Private_Use = _new_invlist_C_array(uni_prop_ptrs[UNI_CO]);

#ifdef UNI_XIDC
    /* The below are used only by deprecated functions.  They could be removed */
    PL_utf8_xidcont  = _new_invlist_C_array(uni_prop_ptrs[UNI_XIDC]);
    PL_utf8_idcont   = _new_invlist_C_array(uni_prop_ptrs[UNI_IDC]);
    PL_utf8_xidstart = _new_invlist_C_array(uni_prop_ptrs[UNI_XIDS]);
#endif