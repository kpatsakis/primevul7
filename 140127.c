static void emulator_put_fpu(struct x86_emulate_ctxt *ctxt)
{
	preempt_enable();
}