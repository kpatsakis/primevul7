static int sco_sock_getsockopt(struct socket *sock, int level, int optname,
			       char __user *optval, int __user *optlen)
{
	struct sock *sk = sock->sk;
	int len, err = 0;
	struct bt_voice voice;
	u32 phys;
	int pkt_status;
	int buf_len;
	struct codec_list *c;
	u8 num_codecs, i, __user *ptr;
	struct hci_dev *hdev;
	struct hci_codec_caps *caps;
	struct bt_codec codec;

	BT_DBG("sk %p", sk);

	if (level == SOL_SCO)
		return sco_sock_getsockopt_old(sock, optname, optval, optlen);

	if (get_user(len, optlen))
		return -EFAULT;

	lock_sock(sk);

	switch (optname) {

	case BT_DEFER_SETUP:
		if (sk->sk_state != BT_BOUND && sk->sk_state != BT_LISTEN) {
			err = -EINVAL;
			break;
		}

		if (put_user(test_bit(BT_SK_DEFER_SETUP, &bt_sk(sk)->flags),
			     (u32 __user *)optval))
			err = -EFAULT;

		break;

	case BT_VOICE:
		voice.setting = sco_pi(sk)->setting;

		len = min_t(unsigned int, len, sizeof(voice));
		if (copy_to_user(optval, (char *)&voice, len))
			err = -EFAULT;

		break;

	case BT_PHY:
		if (sk->sk_state != BT_CONNECTED) {
			err = -ENOTCONN;
			break;
		}

		phys = hci_conn_get_phy(sco_pi(sk)->conn->hcon);

		if (put_user(phys, (u32 __user *) optval))
			err = -EFAULT;
		break;

	case BT_PKT_STATUS:
		pkt_status = (sco_pi(sk)->cmsg_mask & SCO_CMSG_PKT_STATUS);

		if (put_user(pkt_status, (int __user *)optval))
			err = -EFAULT;
		break;

	case BT_SNDMTU:
	case BT_RCVMTU:
		if (sk->sk_state != BT_CONNECTED) {
			err = -ENOTCONN;
			break;
		}

		if (put_user(sco_pi(sk)->conn->mtu, (u32 __user *)optval))
			err = -EFAULT;
		break;

	case BT_CODEC:
		num_codecs = 0;
		buf_len = 0;

		hdev = hci_get_route(&sco_pi(sk)->dst, &sco_pi(sk)->src, BDADDR_BREDR);
		if (!hdev) {
			err = -EBADFD;
			break;
		}

		if (!hci_dev_test_flag(hdev, HCI_OFFLOAD_CODECS_ENABLED)) {
			hci_dev_put(hdev);
			err = -EOPNOTSUPP;
			break;
		}

		if (!hdev->get_data_path_id) {
			hci_dev_put(hdev);
			err = -EOPNOTSUPP;
			break;
		}

		/* find total buffer size required to copy codec + caps */
		hci_dev_lock(hdev);
		list_for_each_entry(c, &hdev->local_codecs, list) {
			if (c->transport != HCI_TRANSPORT_SCO_ESCO)
				continue;
			num_codecs++;
			for (i = 0, caps = c->caps; i < c->num_caps; i++) {
				buf_len += 1 + caps->len;
				caps = (void *)&caps->data[caps->len];
			}
			buf_len += sizeof(struct bt_codec);
		}
		hci_dev_unlock(hdev);

		buf_len += sizeof(struct bt_codecs);
		if (buf_len > len) {
			hci_dev_put(hdev);
			err = -ENOBUFS;
			break;
		}
		ptr = optval;

		if (put_user(num_codecs, ptr)) {
			hci_dev_put(hdev);
			err = -EFAULT;
			break;
		}
		ptr += sizeof(num_codecs);

		/* Iterate all the codecs supported over SCO and populate
		 * codec data
		 */
		hci_dev_lock(hdev);
		list_for_each_entry(c, &hdev->local_codecs, list) {
			if (c->transport != HCI_TRANSPORT_SCO_ESCO)
				continue;

			codec.id = c->id;
			codec.cid = c->cid;
			codec.vid = c->vid;
			err = hdev->get_data_path_id(hdev, &codec.data_path);
			if (err < 0)
				break;
			codec.num_caps = c->num_caps;
			if (copy_to_user(ptr, &codec, sizeof(codec))) {
				err = -EFAULT;
				break;
			}
			ptr += sizeof(codec);

			/* find codec capabilities data length */
			len = 0;
			for (i = 0, caps = c->caps; i < c->num_caps; i++) {
				len += 1 + caps->len;
				caps = (void *)&caps->data[caps->len];
			}

			/* copy codec capabilities data */
			if (len && copy_to_user(ptr, c->caps, len)) {
				err = -EFAULT;
				break;
			}
			ptr += len;
		}

		if (!err && put_user(buf_len, optlen))
			err = -EFAULT;

		hci_dev_unlock(hdev);
		hci_dev_put(hdev);

		break;

	default:
		err = -ENOPROTOOPT;
		break;
	}

	release_sock(sk);
	return err;
}