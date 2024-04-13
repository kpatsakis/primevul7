static void intf_free(struct kref *ref)
{
	struct ipmi_smi *intf = container_of(ref, struct ipmi_smi, refcount);

	clean_up_interface_data(intf);
	kfree(intf);
}