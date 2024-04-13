Status ConstantFolding::MaterializeBroadcastGradientArgs(
    const NodeDef& node, const GraphProperties& properties) {
  const NodeDef* shape_node1 = node_map_->GetNode(node.input(0));
  const NodeDef* shape_node2 = node_map_->GetNode(node.input(1));
  if (shape_node1 == nullptr ||
      (shape_node1->op() != "Shape" && !IsReallyConstant(*shape_node1)) ||
      shape_node2 == nullptr ||
      (shape_node2->op() != "Shape" && !IsReallyConstant(*shape_node2))) {
    return Status::OK();
  }

  // Don't optimize this again if it was already optimized and folded.
  if (OptimizedNodeExists(node, "-folded-1") ||
      OptimizedNodeExists(node, "-folded-2")) {
    return Status::OK();
  }
  int64_t min_id = 0;
  BCast::Vec shape1;
  if (!ExtractShape(*shape_node1, properties, &shape1, &min_id)) {
    return Status::OK();
  }
  BCast::Vec shape2;
  if (!ExtractShape(*shape_node2, properties, &shape2, &min_id)) {
    return Status::OK();
  }
  // A value of -1 means we don't known anything about the dimension. Replace
  // the -1 values with unique dimension ids since we don't want two '-1'
  // dimensions to be considered equal.
  for (auto& id : shape1) {
    if (id == -1) {
      id = --min_id;
    }
  }
  for (auto& id : shape2) {
    if (id == -1) {
      id = --min_id;
    }
  }

  // Beware: the reduction dimensions computed by the BCast class are valid iff
  // we assume that two distinct symbolic dimensions can't be equal and a
  // symbolic dimension can't be equal to 1. This is often but not always true,
  // so to make this optimization safe we filter out these cases.
  const int common_dims = std::min(shape1.size(), shape2.size());
  for (int i = 0; i < common_dims; ++i) {
    if (shape1[i] >= 0 && shape2[i] >= 0) {
      continue;
    }
    if (shape1[i] != shape2[i]) {
      // We're either dealing with 2 different symbolic dimensions or a symbolic
      // and a know dimensions. We can't be sure whether both are equal or not,
      // so we can't be sure whether we'll be broadcasting or not.
      return Status::OK();
    }
  }
  // These extra dims could be equal to 1, in which case there is no
  // broadcasting. It could also be greater than 1, in which case there would
  // be broadcasting. Since we don't know, we'll just punt.
  for (int i = common_dims, end = shape1.size(); i < end; ++i) {
    if (shape1[i] < 0) {
      return Status::OK();
    }
  }
  for (int i = common_dims, end = shape2.size(); i < end; ++i) {
    if (shape2[i] < 0) {
      return Status::OK();
    }
  }

  BCast bcast(shape1, shape2);
  if (!bcast.IsValid()) {
    return Status::OK();
  }

  BCast::Vec reduce_dims[2];
  reduce_dims[0] = bcast.grad_x_reduce_idx();
  reduce_dims[1] = bcast.grad_y_reduce_idx();

  TF_RETURN_IF_ERROR(CheckAttrExists(node, "T"));
  const DataType type = node.attr().at("T").type();
  NodeDef* out[2];
  for (int j = 0; j < 2; ++j) {
    int reduction_indices = reduce_dims[j].size();
    Tensor value(type, TensorShape({reduction_indices}));
    for (int i = 0; i < reduction_indices; ++i) {
      if (type == DT_INT32) {
        value.vec<int32>()(i) = reduce_dims[j][i];
      } else {
        value.vec<int64_t>()(i) = reduce_dims[j][i];
      }
    }
    string const_name =
        OptimizedNodeName(node, strings::StrCat("-bcastargs-", j));
    out[j] = node_map_->GetNode(const_name);
    if (out[j] == nullptr) {
      out[j] = graph_->add_node();
      TF_RETURN_IF_ERROR(
          CreateNodeDef(const_name, TensorValue(&value), out[j]));
      out[j]->set_device(node.device());
      node_map_->AddNode(const_name, out[j]);
      string ctrl_dep =
          AddControlDependency(node.name(), graph_, node_map_.get());
      *out[j]->add_input() = ctrl_dep;
      node_map_->AddOutput(NodeName(ctrl_dep), const_name);
    }
  }

  // We make a copy here since we might mutate the set.
  const auto outputs = node_map_->GetOutputs(node.name());
  for (NodeDef* output : outputs) {
    for (int k = 0; k < output->input_size(); ++k) {
      int port;
      string node_name = ParseNodeName(output->input(k), &port);
      if (node_name == node.name() && port >= 0 && port < 2 && out[port]) {
        *output->mutable_input(k) = out[port]->name();
        node_map_->UpdateInput(output->name(), node_name, out[port]->name());
      }
    }
  }

  return Status::OK();
}