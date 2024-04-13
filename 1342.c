ConstantFolding::ConstantFolding(DeviceBase* cpu_device,
                                 bool disable_compressed_tensor_optimization,
                                 bool fold_quantization_ops)
    : ConstantFolding(RewriterConfig::ON, cpu_device,
                      disable_compressed_tensor_optimization,
                      fold_quantization_ops) {}