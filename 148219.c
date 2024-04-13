static int sco_connect(struct hci_dev *hdev, struct sock *sk)
{
	struct sco_conn *conn;
	struct hci_conn *hcon;
	int err, type;

	BT_DBG("%pMR -> %pMR", &sco_pi(sk)->src, &sco_pi(sk)->dst);

	if (lmp_esco_capable(hdev) && !disable_esco)
		type = ESCO_LINK;
	else
		type = SCO_LINK;

	if (sco_pi(sk)->setting == BT_VOICE_TRANSPARENT &&
	    (!lmp_transp_capable(hdev) || !lmp_esco_capable(hdev)))
		return -EOPNOTSUPP;

	hcon = hci_connect_sco(hdev, type, &sco_pi(sk)->dst,
			       sco_pi(sk)->setting, &sco_pi(sk)->codec);
	if (IS_ERR(hcon))
		return PTR_ERR(hcon);

	conn = sco_conn_add(hcon);
	if (!conn) {
		hci_conn_drop(hcon);
		return -ENOMEM;
	}

	/* Update source addr of the socket */
	bacpy(&sco_pi(sk)->src, &hcon->src);

	err = sco_chan_add(conn, sk, NULL);
	if (err)
		return err;

	if (hcon->state == BT_CONNECTED) {
		sco_sock_clear_timer(sk);
		sk->sk_state = BT_CONNECTED;
	} else {
		sk->sk_state = BT_CONNECT;
		sco_sock_set_timer(sk, sk->sk_sndtimeo);
	}

	return err;
}