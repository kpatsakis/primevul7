mirror_wait_for_any_operation(MirrorBlockJob *s, bool active)
{
    MirrorOp *op;

    QTAILQ_FOREACH(op, &s->ops_in_flight, next) {
        /* Do not wait on pseudo ops, because it may in turn wait on
         * some other operation to start, which may in fact be the
         * caller of this function.  Since there is only one pseudo op
         * at any given time, we will always find some real operation
         * to wait on. */
        if (!op->is_pseudo_op && op->is_in_flight &&
            op->is_active_write == active)
        {
            qemu_co_queue_wait(&op->waiting_requests, NULL);
            return;
        }
    }
    abort();
}