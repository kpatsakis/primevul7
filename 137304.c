static void bpf_get_prog_name(const struct bpf_prog *prog, char *sym)
{
	const char *end = sym + KSYM_NAME_LEN;

	BUILD_BUG_ON(sizeof("bpf_prog_") +
		     sizeof(prog->tag) * 2 +
		     /* name has been null terminated.
		      * We should need +1 for the '_' preceding
		      * the name.  However, the null character
		      * is double counted between the name and the
		      * sizeof("bpf_prog_") above, so we omit
		      * the +1 here.
		      */
		     sizeof(prog->aux->name) > KSYM_NAME_LEN);

	sym += snprintf(sym, KSYM_NAME_LEN, "bpf_prog_");
	sym  = bin2hex(sym, prog->tag, sizeof(prog->tag));
	if (prog->aux->name[0])
		snprintf(sym, (size_t)(end - sym), "_%s", prog->aux->name);
	else
		*sym = 0;
}