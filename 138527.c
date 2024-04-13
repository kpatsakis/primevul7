static void prb_thaw_queue(struct tpacket_kbdq_core *pkc)
{
	pkc->reset_pending_on_curr_blk = 0;
}