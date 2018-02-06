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

#ifndef CAF_INBOUND_PATH_HPP
#define CAF_INBOUND_PATH_HPP

#include <cstddef>
#include <cstdint>

#include "caf/actor_control_block.hpp"
#include "caf/stream_aborter.hpp"
#include "caf/stream_priority.hpp"
#include "caf/stream_slot.hpp"
#include "caf/upstream_msg.hpp"

#include "caf/meta/type_name.hpp"

namespace caf {

/// State for a path to an upstream actor (source).
class inbound_path {
public:
  /// Stream aborter flag to monitor a path.
  static constexpr const auto aborter_type = stream_aborter::source_aborter;

  /// Message type for propagating graceful shutdowns.
  using regular_shutdown = upstream_msg::drop;

  /// Message type for propagating errors.
  using irregular_shutdown = upstream_msg::forced_drop;

  /// Pointer to the parent actor.
  local_actor* self;

  /// Slot ID for the source.
  stream_slot slot;

  /// Handle to the source.
  strong_actor_ptr hdl;

  /// Priority of incoming batches from this source.
  stream_priority prio;

  /// ID of the last acknowledged batch ID.
  int64_t last_acked_batch_id;

  /// ID of the last received batch.
  int64_t last_batch_id;

  /// Amount of credit we have signaled upstream.
  long assigned_credit;

  /// Ideal size for individual batches.
  long desired_batch_size;

  /// Stores whether the source actor is failsafe, i.e., allows the runtime to
  /// redeploy it on failure.
  bool redeployable;

  /// Stores whether an error occurred during stream processing. Configures
  /// whether the destructor sends `close` or `forced_close` messages.
  error shutdown_reason;

  /// Constructs a path for given handle and stream ID.
  inbound_path(local_actor* selfptr, stream_slot id, strong_actor_ptr ptr);

  ~inbound_path();

  /// Updates `last_batch_id` and `assigned_credit`.
  void handle_batch(long batch_size, int64_t batch_id);

  /// Emits a `stream_msg::ack_batch` on this path and sets `assigned_credit`
  /// to `initial_demand`.
  void emit_ack_open(actor_addr rebind_from, long initial_demand,
                     bool is_redeployable);

  void emit_ack_batch(long new_demand);

  static void emit_irregular_shutdown(local_actor* self, stream_slot sid,
                                      const strong_actor_ptr& hdl,
                                      error reason);
};

/// @relates inbound_path
template <class Inspector>
typename Inspector::return_type inspect(Inspector& f, inbound_path& x) {
  return f(meta::type_name("inbound_path"), x.hdl, x.slot, x.prio,
           x.last_acked_batch_id, x.last_batch_id, x.assigned_credit);
}

} // namespace caf

#endif // CAF_INBOUND_PATH_HPP
