void dane_state_deinit(dane_state_t s)
{
	ub_ctx_delete(s->ctx);
	free(s);
}