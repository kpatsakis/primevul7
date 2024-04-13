static int emulator_intercept(struct x86_emulate_ctxt *ctxt,
			      struct x86_instruction_info *info,
			      enum x86_intercept_stage stage)
{
	return kvm_x86_ops->check_intercept(emul_to_vcpu(ctxt), info, stage);
}