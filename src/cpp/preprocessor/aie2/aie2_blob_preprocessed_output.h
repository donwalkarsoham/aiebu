// SPDX-License-Identifier: MIT
// Copyright (C) 2024-2025, Advanced Micro Devices, Inc. All rights reserved.

#ifndef _AIEBU_PREPROCESSOR_AIE2_BLOB_PREPROCESSED_OUTPUT_H_
#define _AIEBU_PREPROCESSOR_AIE2_BLOB_PREPROCESSED_OUTPUT_H_

#include <string>
#include <map>
#include <unordered_map>
#include "symbol.h"
#include "preprocessed_output.h"

namespace aiebu {

class aie2_blob_preprocessed_output : public preprocessed_output
{

  // Use std::map which forces the same order for keys across all platforms
  // std::unordered_map was leading to different order of ELF sections causing
  // ELF binaries produced on Linux and Windows to look different .
  std::map<std::string, std::vector<uint8_t>> m_data;
  std::vector<symbol> m_sym;
  std::shared_ptr<const partition_info> m_partition;
public:
  explicit aie2_blob_preprocessed_output(std::shared_ptr<const partition_info> partition): m_partition(std::move(partition)) {}

  std::shared_ptr<const partition_info> get_partition_info() const { return m_partition; }

  std::vector<symbol>& get_symbols()
  {
    return m_sym;
  }

  void add_data(const std::string& name, const std::vector<uint8_t>& buf)
  {
    m_data[name] = buf;
  }

  void add_symbol(const symbol buf)
  {
    m_sym.emplace_back(buf);
  }

  void add_symbols(std::vector<symbol>& syms)
  {
    m_sym = std::move(syms);
  }

  const std::vector<std::string> get_keys()
  {
    std::vector<std::string> keys(m_data.size());
    auto key_selector = [](auto pair){return pair.first;};
    transform(m_data.begin(), m_data.end(), keys.begin(), key_selector);
    return keys;
  }

  std::vector<uint8_t>& get_data(std::string& key)
  {
    auto it = m_data.find(key);
    if (it == m_data.end())
      throw error(error::error_code::internal_error, "Key (" + key + ") not found!!!");
    return m_data[key];
  }
};

class instance_output
{
  std::map<std::string, std::shared_ptr<aie2_blob_preprocessed_output>> m_instance;
  std::map<std::string, std::vector<uint8_t>> m_data_common;
public:

  const std::map<std::string, std::shared_ptr<aie2_blob_preprocessed_output>>& get_instance_map() const
  {
    return m_instance;
  }

  const std::map<std::string, std::vector<uint8_t>>& get_common() const
  {
    return m_data_common;
  }

  void add_instance(const std::string& instance, std::shared_ptr<aie2_blob_preprocessed_output> val)
  {
    m_instance[instance] = val;
  }

  void add_common_data(const std::string& dname, std::vector<uint8_t> val)
  {
    m_data_common[dname] = std::move(val);
  }
};

class aie2_config_preprocessed_output: public preprocessed_output
{
  std::map<std::string, instance_output> m_output;
public:

  const std::map<std::string, instance_output>&
  get_kernel_map() const { return m_output; }

  void add_kernel_map(const std::string& kernel, const std::string& instance, std::shared_ptr<aie2_blob_preprocessed_output> val) {
    m_output[kernel].add_instance(instance, val);
  }

  void add_kernel_common_data(const std::string& kernel, const std::string& dname, std::vector<uint8_t> val) {
    m_output[kernel].add_common_data(dname, std::move(val));
  }
};

}
#endif //_AIEBU_PREPROCESSOR_AIE2_BLOB_PREPROCESSED_OUTPUT_H_
