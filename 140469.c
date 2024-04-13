static void kvm_on_user_return(struct user_return_notifier *urn)
{
	unsigned slot;
	struct kvm_shared_msrs *locals
		= container_of(urn, struct kvm_shared_msrs, urn);
	struct kvm_shared_msr_values *values;

	for (slot = 0; slot < shared_msrs_global.nr; ++slot) {
		values = &locals->values[slot];
		if (values->host != values->curr) {
			wrmsrl(shared_msrs_global.msrs[slot], values->host);
			values->curr = values->host;
		}
	}
	locals->registered = false;
	user_return_notifier_unregister(urn);
}