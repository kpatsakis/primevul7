static void free_ioctx(struct work_struct *work)
{
	struct kioctx *ctx = container_of(to_rcu_work(work), struct kioctx,
					  free_rwork);
	pr_debug("freeing %p\n", ctx);

	aio_free_ring(ctx);
	free_percpu(ctx->cpu);
	percpu_ref_exit(&ctx->reqs);
	percpu_ref_exit(&ctx->users);
	kmem_cache_free(kioctx_cachep, ctx);
}