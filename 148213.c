static int sco_send_frame(struct sock *sk, struct sk_buff *skb)
{
	struct sco_conn *conn = sco_pi(sk)->conn;

	/* Check outgoing MTU */
	if (skb->len > conn->mtu)
		return -EINVAL;

	BT_DBG("sk %p len %d", sk, skb->len);

	hci_send_sco(conn->hcon, skb);

	return skb->len;
}