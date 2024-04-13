static inline void vpid_sync_context(struct vcpu_vmx *vmx)
{
	if (cpu_has_vmx_invvpid_single())
		vpid_sync_vcpu_single(vmx);
	else
		vpid_sync_vcpu_global();
}