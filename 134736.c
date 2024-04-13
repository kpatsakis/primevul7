CLEAR_MARKS_PROC(ref_stack_clear_marks)
{
    ref_stack_t *const sptr = vptr;

    r_clear_attrs(&sptr->current, l_mark);
}