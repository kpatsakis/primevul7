static inline void vpid_sync_vcpu_single(struct vcpu_vmx *vmx)
{
	if (vmx->vpid == 0)
		return;

	if (cpu_has_vmx_invvpid_single())
		__invvpid(VMX_VPID_EXTENT_SINGLE_CONTEXT, vmx->vpid, 0);
}