static __inline__ void scm_destroy_cred(struct scm_cookie *scm)
{
	put_pid(scm->pid);
	scm->pid  = NULL;

	if (scm->cred)
		put_cred(scm->cred);
	scm->cred = NULL;
}