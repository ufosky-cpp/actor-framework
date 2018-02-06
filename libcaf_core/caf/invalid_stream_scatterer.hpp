/******************************************************************************
 *                       ____    _    _____                                   *
 *                      / ___|  / \  |  ___|    C++                           *
 *                     | |     / _ \ | |_       Actor                         *
 *                     | |___ / ___ \|  _|      Framework                     *
 *                      \____/_/   \_|_|                                      *
 *                                                                            *
 * Copyright 2011-2018 Dominik Charousset                                     *
 *                                                                            *
 * Distributed under the terms and conditions of the BSD 3-Clause License or  *
 * (at your option) under the terms and conditions of the Boost Software      *
 * License 1.0. See accompanying files LICENSE and LICENSE_ALTERNATIVE.       *
 *                                                                            *
 * If you did not receive a copy of the license files, see                    *
 * http://opensource.org/licenses/BSD-3-Clause and                            *
 * http://www.boost.org/LICENSE_1_0.txt.                                      *
 ******************************************************************************/

#ifndef CAF_INVALID_STREAM_SCATTERER_HPP
#define CAF_INVALID_STREAM_SCATTERER_HPP

#include "caf/stream_scatterer.hpp"

namespace caf {

/// Type-erased policy for dispatching data to sinks.
class invalid_stream_scatterer : public stream_scatterer {
public:
  invalid_stream_scatterer(local_actor* self = nullptr);

  ~invalid_stream_scatterer() override;

  path_ptr add_path(stream_slot slot, strong_actor_ptr origin,
                    strong_actor_ptr sink_ptr,
                    mailbox_element::forwarding_stack stages,
                    message_id handshake_mid, message handshake_data,
                    stream_priority prio, bool redeployable) override;

  path_ptr confirm_path(stream_slot slot, const actor_addr& from,
                        strong_actor_ptr to, long initial_demand,
                        long desired_batch_size, bool redeployable) override;

  bool remove_path(stream_slot slot, const actor_addr& x,
                           error reason, bool silent) override;

  bool paths_clean() const override;

  void close() override;

  void abort(error reason) override;

  long num_paths() const override;

  bool closed() const override;

  bool continuous() const override;

  void continuous(bool value) override;

  path_type* path_at(size_t index) override;

  void emit_batches() override;

  path_type* find(stream_slot slot, const actor_addr& x) override;

  long credit() const override;

  long buffered() const override;

  long desired_batch_size() const override;

  long min_buffer_size() const override;

  duration max_batch_delay() const override;

  void max_batch_delay(duration x) override;
};

} // namespace caf

#endif // CAF_INVALID_STREAM_SCATTERER_HPP
