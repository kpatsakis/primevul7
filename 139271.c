static int maybe_init_creds(struct scm_cookie *scm,
			    struct socket *socket,
			    const struct sock *other)
{
	int err;
	struct msghdr msg = { .msg_controllen = 0 };

	err = scm_send(socket, &msg, scm, false);
	if (err)
		return err;

	if (unix_passcred_enabled(socket, other)) {
		scm->pid = get_pid(task_tgid(current));
		current_uid_gid(&scm->creds.uid, &scm->creds.gid);
	}
	return err;
}