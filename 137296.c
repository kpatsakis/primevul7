static void bpf_prog_free_deferred(struct work_struct *work)
{
	struct bpf_prog_aux *aux;
	int i;

	aux = container_of(work, struct bpf_prog_aux, work);
	if (bpf_prog_is_dev_bound(aux))
		bpf_prog_offload_destroy(aux->prog);
	for (i = 0; i < aux->func_cnt; i++)
		bpf_jit_free(aux->func[i]);
	if (aux->func_cnt) {
		kfree(aux->func);
		bpf_prog_unlock_free(aux->prog);
	} else {
		bpf_jit_free(aux->prog);
	}
}