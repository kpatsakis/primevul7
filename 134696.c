ref_stack_free(ref_stack_t *pstack)
{
    gs_memory_t *mem = (gs_memory_t *)pstack->memory;

    ref_stack_release(pstack);
    gs_free_object(mem, pstack, "ref_stack_free");
}