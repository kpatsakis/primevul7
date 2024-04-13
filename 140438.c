static void free_vpid(struct vcpu_vmx *vmx)
{
	if (!enable_vpid)
		return;
	spin_lock(&vmx_vpid_lock);
	if (vmx->vpid != 0)
		__clear_bit(vmx->vpid, vmx_vpid_bitmap);
	spin_unlock(&vmx_vpid_lock);
}