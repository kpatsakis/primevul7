Status ConstantFolding::MaterializeReductionIndices(
    NodeDef* node, const GraphProperties& properties) {
  if (node->input_size() < 2) {
    return Status::OK();
  }
  const NodeDef* indices = node_map_->GetNode(node->input(1));
  if (!indices || IsReallyConstant(*indices)) {
    // The reduction indices are already constant, there's nothing to do.
    return Status::OK();
  }

  const std::vector<OpInfo::TensorProperties>& input_props =
      properties.GetInputProperties(node->name());
  if (input_props.size() != 2) {
    return Status::OK();
  }
  const OpInfo::TensorProperties& input_prop = input_props[0];
  if (input_prop.shape().unknown_rank()) {
    // We can't do anything if we don't know the rank of the input.
    return Status::OK();
  }
  const int input_rank = input_prop.shape().dim_size();
  if (input_rank < 1) {
    // Unexpected graph, don't try to change it.
    return Status::OK();
  }
  const OpInfo::TensorProperties& reduction_indices_prop = input_props[1];
  DataType dtype = reduction_indices_prop.dtype();
  if (dtype != DT_INT32 && dtype != DT_INT64) {
    return Status::OK();
  }
  PartialTensorShape reduction_indices_shape(reduction_indices_prop.shape());
  const int num_reduction_indices = reduction_indices_shape.num_elements();

  const std::vector<OpInfo::TensorProperties>& output_props =
      properties.GetOutputProperties(node->name());
  if (output_props.size() != 1) {
    return Status::OK();
  }
  const OpInfo::TensorProperties& output_prop = output_props[0];
  const int output_rank =
      output_prop.shape().unknown_rank() ? -1 : output_prop.shape().dim_size();

  bool full_reduction = output_rank == 0 || num_reduction_indices == input_rank;
  if (!full_reduction) {
    // A full reduction will generate a tensor of one of the shapes
    // [], [1], [1, 1], [1, 1, ...]. Even if we do not know the number of
    // elements in the output of the reduction, we may deduce it from reshape
    // nodes following it.
    for (const NodeDef* fanout : node_map_->GetOutputs(node->name())) {
      full_reduction = false;
      if (!IsReshape(*fanout)) {
        return Status::OK();
      }
      const std::vector<OpInfo::TensorProperties>& reshape_props =
          properties.GetOutputProperties(fanout->name());
      if (reshape_props.size() != 1) {
        return Status::OK();
      }
      const OpInfo::TensorProperties& reshape_prop = reshape_props[0];
      PartialTensorShape shape(reshape_prop.shape());
      if (shape.num_elements() != 1) {
        return Status::OK();
      } else {
        full_reduction = true;
      }
    }
    if (!full_reduction) {
      return Status::OK();
    }
  }

  // We know it's a full reduction. We can generate the full set of indices to
  // reduce as a constant node.
  string const_name = OptimizedNodeName(*node, "-reduction_indices");
  if (node_map_->GetNode(const_name)) {
    return Status::OK();
  }
  NodeDef* reduction_indices = graph_->add_node();
  Tensor value(dtype, TensorShape({input_rank}));
  for (int i = 0; i < input_rank; ++i) {
    if (dtype == DT_INT32) {
      value.vec<int32>()(i) = i;
    } else {
      value.vec<int64_t>()(i) = i;
    }
  }
  TF_RETURN_IF_ERROR(
      CreateNodeDef(const_name, TensorValue(&value), reduction_indices));

  reduction_indices->set_device(node->device());
  string ctrl_dep =
      AddControlDependency(node->input(1), graph_, node_map_.get());
  *reduction_indices->add_input() = ctrl_dep;
  node_map_->AddNode(const_name, reduction_indices);
  node_map_->AddOutput(NodeName(ctrl_dep), const_name);

  node->set_input(1, reduction_indices->name());
  node_map_->UpdateInput(node->name(), indices->name(),
                         reduction_indices->name());

  return Status::OK();
}