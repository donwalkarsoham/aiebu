// SPDX-License-Identifier: MIT
// Copyright (C) 2024-2025, Advanced Micro Devices, Inc. All rights reserved.

#ifndef _AIEBU_PREPROCESSOR_AIE2_BLOB_PREPROCESSOR_H_
#define _AIEBU_PREPROCESSOR_AIE2_BLOB_PREPROCESSOR_H_

#include "preprocessor.h"
#include "aie2_blob_preprocessor_input.h"
#include "aie2_blob_preprocessed_output.h"

namespace aiebu {

class aie2_blob_preprocessor: public preprocessor
{  

public:
  aie2_blob_preprocessor() {}

  std::vector<uint8_t> transform(const std::vector<char>& in)
  {
    // transform vector<char> to vector<uint8_t>
    std::vector<uint8_t> out;
    out.resize(in.size());
    std::transform(in.begin(), in.end(), out.begin(), [](char c) {return static_cast<uint8_t>(c);});
    return out;
  }

  virtual std::shared_ptr<preprocessed_output>
  process(std::shared_ptr<preprocessor_input> input) override
  {
    // preporcess : nothing to be done.
    auto rinput = std::static_pointer_cast<aie2_blob_preprocessor_input>(input);
    auto routput = std::make_shared<aie2_blob_preprocessed_output>(rinput->get_partition_info());

    for(auto key : rinput->get_keys())
      routput->add_data(key, transform(rinput->get_data(key)));

    routput->add_symbols(rinput->get_symbols());
    return routput;
  }
};


class aie2_config_preprocessor: public preprocessor
{

public:
  aie2_config_preprocessor() = default;

  std::vector<uint8_t> transform(const std::vector<char>& in)
  {
    // transform vector<char> to vector<uint8_t>
    std::vector<uint8_t> out;
    out.resize(in.size());
    std::transform(in.begin(), in.end(), out.begin(), [](char c) {return static_cast<uint8_t>(c);});
    return out;
  }

  std::shared_ptr<preprocessed_output>
  process(std::shared_ptr<preprocessor_input> input) override
  {
    // preporcess : nothing to be done.
    auto rinput = std::static_pointer_cast<aie2_config_preprocessor_input>(input);
    auto routput = std::make_shared<aie2_config_preprocessed_output>();
    aie2_blob_preprocessor preprocessor;

    for (auto& [kernel, instances] : rinput->get_kernel_map()) {
      //copy instances->m_data_common
      for(auto& [dname, data] : instances.get_common())
        routput->add_kernel_common_data(kernel, dname, transform(data));

      for(auto& [iname, instance] : instances.get_instance_map())
      {
        auto val = std::dynamic_pointer_cast<aie2_blob_preprocessed_output>(preprocessor.process(instance));
        routput->add_kernel_map(kernel, iname, val);
      }
    }
    return routput;
  }
};
}
#endif //_AIEBU_PREPROCESSOR_AIE2_BLOB_PREPROCESSOR_H_
