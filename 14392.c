static int esp_output_tail_tcp(struct xfrm_state *x, struct sk_buff *skb)
{
	int err;

	local_bh_disable();
	err = xfrm_trans_queue_net(xs_net(x), skb, esp_output_tcp_encap_cb);
	local_bh_enable();

	/* EINPROGRESS just happens to do the right thing.  It
	 * actually means that the skb has been consumed and
	 * isn't coming back.
	 */
	return err ?: -EINPROGRESS;
}