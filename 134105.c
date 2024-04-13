static void free_smacro_members(SMacro *s)
{
    if (s->params) {
        int i;
        for (i = 0; i < s->nparam; i++) {
	    if (s->params[i].name.len > INLINE_TEXT)
		nasm_free(s->params[i].name.text.p.ptr);
	}
        nasm_free(s->params);
    }
    nasm_free(s->name);
    free_tlist(s->expansion);
}