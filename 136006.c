static int __init init_msr_all_available(void)
{
#ifdef CONFIG_PPC_FPU
	msr_all_available |= MSR_FP;
#endif
#ifdef CONFIG_ALTIVEC
	if (cpu_has_feature(CPU_FTR_ALTIVEC))
		msr_all_available |= MSR_VEC;
#endif
#ifdef CONFIG_VSX
	if (cpu_has_feature(CPU_FTR_VSX))
		msr_all_available |= MSR_VSX;
#endif
#ifdef CONFIG_SPE
	if (cpu_has_feature(CPU_FTR_SPE))
		msr_all_available |= MSR_SPE;
#endif

	return 0;
}