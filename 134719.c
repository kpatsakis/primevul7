static RELOC_PTRS_WITH(ref_stack_reloc_ptrs, ref_stack_t *sptr)
{
    /* Note that the relocation must be a multiple of sizeof(ref_packed) */
    /* * align_packed_per_ref, but it need not be a multiple of */
    /* sizeof(ref).  Therefore, we must do the adjustments using */
    /* ref_packed pointers rather than ref pointers. */
    ref_packed *bot = (ref_packed *) sptr->current.value.refs;
    long reloc;

    RELOC_REF_VAR(sptr->current);
    r_clear_attrs(&sptr->current, l_mark);
    reloc = bot - (ref_packed *) sptr->current.value.refs;
#define RELOC_P(p)\
  sptr->p = (ref *)((ref_packed *)sptr->p - reloc);
    RELOC_P(p);
    RELOC_P(bot);
    RELOC_P(top);
#undef RELOC_P
    RELOC_OBJ_VAR(sptr->params);
} RELOC_PTRS_END