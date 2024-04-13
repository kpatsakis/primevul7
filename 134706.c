ref_stack_enum_begin(ref_stack_enum_t *prse, const ref_stack_t *pstack)
{
    prse->block = (ref_stack_block *)pstack->current.value.refs;
    prse->ptr = pstack->bot;
    prse->size = pstack->p + 1 - pstack->bot;
}