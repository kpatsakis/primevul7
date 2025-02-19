static int vcpu_mmio_read(struct kvm_vcpu *vcpu, gpa_t addr, int len, void *v)
{
	int handled = 0;
	int n;

	do {
		n = min(len, 8);
		if (!(vcpu->arch.apic &&
		      !kvm_iodevice_read(&vcpu->arch.apic->dev, addr, n, v))
		    && kvm_io_bus_read(vcpu->kvm, KVM_MMIO_BUS, addr, n, v))
			break;
		trace_kvm_mmio(KVM_TRACE_MMIO_READ, n, addr, *(u64 *)v);
		handled += n;
		addr += n;
		len -= n;
		v += n;
	} while (len);

	return handled;
}