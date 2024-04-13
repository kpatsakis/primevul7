static void sctp_chunk_destroy(struct sctp_chunk *chunk)
{
	BUG_ON(!list_empty(&chunk->list));
	list_del_init(&chunk->transmitted_list);

	consume_skb(chunk->skb);
	consume_skb(chunk->auth_chunk);

	SCTP_DBG_OBJCNT_DEC(chunk);
	kmem_cache_free(sctp_chunk_cachep, chunk);
}