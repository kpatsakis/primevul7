static void aio_fill_event(struct io_event *ev, struct aio_kiocb *iocb,
			   long res, long res2)
{
	ev->obj = (u64)(unsigned long)iocb->ki_user_iocb;
	ev->data = iocb->ki_user_data;
	ev->res = res;
	ev->res2 = res2;
}