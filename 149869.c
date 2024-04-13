match_init_copy(VALUE obj, VALUE orig)
{
    struct rmatch *rm;

    if (!OBJ_INIT_COPY(obj, orig)) return obj;

    RMATCH(obj)->str = RMATCH(orig)->str;
    RMATCH(obj)->regexp = RMATCH(orig)->regexp;

    rm = RMATCH(obj)->rmatch;
    if (rb_reg_region_copy(&rm->regs, RMATCH_REGS(orig)))
	rb_memerror();

    if (!RMATCH(orig)->rmatch->char_offset_updated) {
        rm->char_offset_updated = 0;
    }
    else {
        if (rm->char_offset_num_allocated < rm->regs.num_regs) {
            REALLOC_N(rm->char_offset, struct rmatch_offset, rm->regs.num_regs);
            rm->char_offset_num_allocated = rm->regs.num_regs;
        }
        MEMCPY(rm->char_offset, RMATCH(orig)->rmatch->char_offset,
               struct rmatch_offset, rm->regs.num_regs);
        rm->char_offset_updated = 1;
	RB_GC_GUARD(orig);
    }

    return obj;
}