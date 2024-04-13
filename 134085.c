static void pp_add_magic_stdmac(void)
{
    const struct magic_macros *m;
    SMacro tmpl;

    nasm_zero(tmpl);

    for (m = magic_macros; m->name; m++) {
        tmpl.nparam = m->nparam;
        tmpl.expand = m->func;
        define_smacro(m->name, true, NULL, &tmpl);
    }
}