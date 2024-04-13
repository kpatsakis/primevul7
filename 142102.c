static bool kvm_hv_msr_partition_wide(u32 msr)
{
	bool r = false;
	switch (msr) {
	case HV_X64_MSR_GUEST_OS_ID:
	case HV_X64_MSR_HYPERCALL:
		r = true;
		break;
	}

	return r;
}