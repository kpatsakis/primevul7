ref_stack_enum_next(ref_stack_enum_t *prse)
{
    ref_stack_block *block =
        prse->block = (ref_stack_block *)prse->block->next.value.refs;

    if (block == 0)
        return false;
    prse->ptr = block->used.value.refs;
    prse->size = r_size(&block->used);
    return true;
}