ref_stack_set_error_codes(ref_stack_t *pstack, int underflow_error,
                          int overflow_error)
{
    pstack->params->underflow_error = underflow_error;
    pstack->params->overflow_error = overflow_error;
}