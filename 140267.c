int kvm_set_msr_common(struct kvm_vcpu *vcpu, struct msr_data *msr_info)
{
	bool pr = false;
	u32 msr = msr_info->index;
	u64 data = msr_info->data;

	switch (msr) {
	case MSR_AMD64_NB_CFG:
	case MSR_IA32_UCODE_REV:
	case MSR_IA32_UCODE_WRITE:
	case MSR_VM_HSAVE_PA:
	case MSR_AMD64_PATCH_LOADER:
	case MSR_AMD64_BU_CFG2:
		break;

	case MSR_EFER:
		return set_efer(vcpu, data);
	case MSR_K7_HWCR:
		data &= ~(u64)0x40;	/* ignore flush filter disable */
		data &= ~(u64)0x100;	/* ignore ignne emulation enable */
		data &= ~(u64)0x8;	/* ignore TLB cache disable */
		data &= ~(u64)0x40000;  /* ignore Mc status write enable */
		if (data != 0) {
			vcpu_unimpl(vcpu, "unimplemented HWCR wrmsr: 0x%llx\n",
				    data);
			return 1;
		}
		break;
	case MSR_FAM10H_MMIO_CONF_BASE:
		if (data != 0) {
			vcpu_unimpl(vcpu, "unimplemented MMIO_CONF_BASE wrmsr: "
				    "0x%llx\n", data);
			return 1;
		}
		break;
	case MSR_IA32_DEBUGCTLMSR:
		if (!data) {
			/* We support the non-activated case already */
			break;
		} else if (data & ~(DEBUGCTLMSR_LBR | DEBUGCTLMSR_BTF)) {
			/* Values other than LBR and BTF are vendor-specific,
			   thus reserved and should throw a #GP */
			return 1;
		}
		vcpu_unimpl(vcpu, "%s: MSR_IA32_DEBUGCTLMSR 0x%llx, nop\n",
			    __func__, data);
		break;
	case 0x200 ... 0x2ff:
		return set_msr_mtrr(vcpu, msr, data);
	case MSR_IA32_APICBASE:
		return kvm_set_apic_base(vcpu, msr_info);
	case APIC_BASE_MSR ... APIC_BASE_MSR + 0x3ff:
		return kvm_x2apic_msr_write(vcpu, msr, data);
	case MSR_IA32_TSCDEADLINE:
		kvm_set_lapic_tscdeadline_msr(vcpu, data);
		break;
	case MSR_IA32_TSC_ADJUST:
		if (guest_cpuid_has_tsc_adjust(vcpu)) {
			if (!msr_info->host_initiated) {
				u64 adj = data - vcpu->arch.ia32_tsc_adjust_msr;
				kvm_x86_ops->adjust_tsc_offset(vcpu, adj, true);
			}
			vcpu->arch.ia32_tsc_adjust_msr = data;
		}
		break;
	case MSR_IA32_MISC_ENABLE:
		vcpu->arch.ia32_misc_enable_msr = data;
		break;
	case MSR_KVM_WALL_CLOCK_NEW:
	case MSR_KVM_WALL_CLOCK:
		vcpu->kvm->arch.wall_clock = data;
		kvm_write_wall_clock(vcpu->kvm, data);
		break;
	case MSR_KVM_SYSTEM_TIME_NEW:
	case MSR_KVM_SYSTEM_TIME: {
		u64 gpa_offset;
		kvmclock_reset(vcpu);

		vcpu->arch.time = data;
		kvm_make_request(KVM_REQ_GLOBAL_CLOCK_UPDATE, vcpu);

		/* we verify if the enable bit is set... */
		if (!(data & 1))
			break;

		gpa_offset = data & ~(PAGE_MASK | 1);

		if (kvm_gfn_to_hva_cache_init(vcpu->kvm,
		     &vcpu->arch.pv_time, data & ~1ULL,
		     sizeof(struct pvclock_vcpu_time_info)))
			vcpu->arch.pv_time_enabled = false;
		else
			vcpu->arch.pv_time_enabled = true;

		break;
	}
	case MSR_KVM_ASYNC_PF_EN:
		if (kvm_pv_enable_async_pf(vcpu, data))
			return 1;
		break;
	case MSR_KVM_STEAL_TIME:

		if (unlikely(!sched_info_on()))
			return 1;

		if (data & KVM_STEAL_RESERVED_MASK)
			return 1;

		if (kvm_gfn_to_hva_cache_init(vcpu->kvm, &vcpu->arch.st.stime,
						data & KVM_STEAL_VALID_BITS,
						sizeof(struct kvm_steal_time)))
			return 1;

		vcpu->arch.st.msr_val = data;

		if (!(data & KVM_MSR_ENABLED))
			break;

		vcpu->arch.st.last_steal = current->sched_info.run_delay;

		preempt_disable();
		accumulate_steal_time(vcpu);
		preempt_enable();

		kvm_make_request(KVM_REQ_STEAL_UPDATE, vcpu);

		break;
	case MSR_KVM_PV_EOI_EN:
		if (kvm_lapic_enable_pv_eoi(vcpu, data))
			return 1;
		break;

	case MSR_IA32_MCG_CTL:
	case MSR_IA32_MCG_STATUS:
	case MSR_IA32_MC0_CTL ... MSR_IA32_MCx_CTL(KVM_MAX_MCE_BANKS) - 1:
		return set_msr_mce(vcpu, msr, data);

	/* Performance counters are not protected by a CPUID bit,
	 * so we should check all of them in the generic path for the sake of
	 * cross vendor migration.
	 * Writing a zero into the event select MSRs disables them,
	 * which we perfectly emulate ;-). Any other value should be at least
	 * reported, some guests depend on them.
	 */
	case MSR_K7_EVNTSEL0:
	case MSR_K7_EVNTSEL1:
	case MSR_K7_EVNTSEL2:
	case MSR_K7_EVNTSEL3:
		if (data != 0)
			vcpu_unimpl(vcpu, "unimplemented perfctr wrmsr: "
				    "0x%x data 0x%llx\n", msr, data);
		break;
	/* at least RHEL 4 unconditionally writes to the perfctr registers,
	 * so we ignore writes to make it happy.
	 */
	case MSR_K7_PERFCTR0:
	case MSR_K7_PERFCTR1:
	case MSR_K7_PERFCTR2:
	case MSR_K7_PERFCTR3:
		vcpu_unimpl(vcpu, "unimplemented perfctr wrmsr: "
			    "0x%x data 0x%llx\n", msr, data);
		break;
	case MSR_P6_PERFCTR0:
	case MSR_P6_PERFCTR1:
		pr = true;
	case MSR_P6_EVNTSEL0:
	case MSR_P6_EVNTSEL1:
		if (kvm_pmu_msr(vcpu, msr))
			return kvm_pmu_set_msr(vcpu, msr_info);

		if (pr || data != 0)
			vcpu_unimpl(vcpu, "disabled perfctr wrmsr: "
				    "0x%x data 0x%llx\n", msr, data);
		break;
	case MSR_K7_CLK_CTL:
		/*
		 * Ignore all writes to this no longer documented MSR.
		 * Writes are only relevant for old K7 processors,
		 * all pre-dating SVM, but a recommended workaround from
		 * AMD for these chips. It is possible to specify the
		 * affected processor models on the command line, hence
		 * the need to ignore the workaround.
		 */
		break;
	case HV_X64_MSR_GUEST_OS_ID ... HV_X64_MSR_SINT15:
		if (kvm_hv_msr_partition_wide(msr)) {
			int r;
			mutex_lock(&vcpu->kvm->lock);
			r = set_msr_hyperv_pw(vcpu, msr, data);
			mutex_unlock(&vcpu->kvm->lock);
			return r;
		} else
			return set_msr_hyperv(vcpu, msr, data);
		break;
	case MSR_IA32_BBL_CR_CTL3:
		/* Drop writes to this legacy MSR -- see rdmsr
		 * counterpart for further detail.
		 */
		vcpu_unimpl(vcpu, "ignored wrmsr: 0x%x data %llx\n", msr, data);
		break;
	case MSR_AMD64_OSVW_ID_LENGTH:
		if (!guest_cpuid_has_osvw(vcpu))
			return 1;
		vcpu->arch.osvw.length = data;
		break;
	case MSR_AMD64_OSVW_STATUS:
		if (!guest_cpuid_has_osvw(vcpu))
			return 1;
		vcpu->arch.osvw.status = data;
		break;
	default:
		if (msr && (msr == vcpu->kvm->arch.xen_hvm_config.msr))
			return xen_hvm_config(vcpu, data);
		if (kvm_pmu_msr(vcpu, msr))
			return kvm_pmu_set_msr(vcpu, msr_info);
		if (!ignore_msrs) {
			vcpu_unimpl(vcpu, "unhandled wrmsr: 0x%x data %llx\n",
				    msr, data);
			return 1;
		} else {
			vcpu_unimpl(vcpu, "ignored wrmsr: 0x%x data %llx\n",
				    msr, data);
			break;
		}
	}
	return 0;
}