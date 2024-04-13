ref_stack_pop(ref_stack_t *pstack, uint count)
{
    uint used;

    while ((used = pstack->p + 1 - pstack->bot) < count) {
        count -= used;
        pstack->p = pstack->bot - 1;
        ref_stack_pop_block(pstack);
    }
    pstack->p -= count;
}