op_function(oparg_T *oap UNUSED)
{
#ifdef FEAT_EVAL
    typval_T	argv[2];
    int		save_virtual_op = virtual_op;
    int		save_finish_op = finish_op;
    pos_T	orig_start = curbuf->b_op_start;
    pos_T	orig_end = curbuf->b_op_end;
    typval_T	rettv;

    if (*p_opfunc == NUL)
	emsg(_(e_operatorfunc_is_empty));
    else
    {
	// Set '[ and '] marks to text to be operated on.
	curbuf->b_op_start = oap->start;
	curbuf->b_op_end = oap->end;
	if (oap->motion_type != MLINE && !oap->inclusive)
	    // Exclude the end position.
	    decl(&curbuf->b_op_end);

	argv[0].v_type = VAR_STRING;
	if (oap->block_mode)
	    argv[0].vval.v_string = (char_u *)"block";
	else if (oap->motion_type == MLINE)
	    argv[0].vval.v_string = (char_u *)"line";
	else
	    argv[0].vval.v_string = (char_u *)"char";
	argv[1].v_type = VAR_UNKNOWN;

	// Reset virtual_op so that 'virtualedit' can be changed in the
	// function.
	virtual_op = MAYBE;

	// Reset finish_op so that mode() returns the right value.
	finish_op = FALSE;

	if (call_callback(&opfunc_cb, 0, &rettv, 1, argv) != FAIL)
	    clear_tv(&rettv);

	virtual_op = save_virtual_op;
	finish_op = save_finish_op;
	if (cmdmod.cmod_flags & CMOD_LOCKMARKS)
	{
	    curbuf->b_op_start = orig_start;
	    curbuf->b_op_end = orig_end;
	}
    }
#else
    emsg(_(e_eval_feature_not_available));
#endif
}