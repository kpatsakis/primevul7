ConstantFolding::ConstantFolding(RewriterConfig::Toggle opt_level,
                                 DeviceBase* cpu_device,
                                 bool disable_compressed_tensor_optimization,
                                 bool fold_quantization_emulation)
    : opt_level_(opt_level),
      cpu_device_(cpu_device),
      disable_compressed_tensor_optimization_(
          disable_compressed_tensor_optimization),
      fold_quantization_emulation_(fold_quantization_emulation) {
  resource_mgr_.reset(new ResourceMgr());
}