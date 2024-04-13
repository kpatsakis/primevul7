void bpf_prog_free(struct bpf_prog *fp)
{
	struct bpf_prog_aux *aux = fp->aux;

	INIT_WORK(&aux->work, bpf_prog_free_deferred);
	schedule_work(&aux->work);
}