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

#include "caf/outbound_path.hpp"

#include "caf/send.hpp"
#include "caf/logger.hpp"
#include "caf/no_stages.hpp"
#include "caf/local_actor.hpp"

namespace caf {

outbound_path::outbound_path(local_actor* selfptr, stream_slots id,
                             strong_actor_ptr ptr)
    : slots(id),
      self(selfptr),
      hdl(std::move(ptr)),
      next_batch_id(0),
      open_credit(0),
      desired_batch_size(0),
      redeployable(false),
      next_ack_id(0) {
  // nop
}

outbound_path::~outbound_path() {
  CAF_LOG_TRACE(CAF_ARG(shutdown_reason));
  if (hdl) {
    if (shutdown_reason == none)
      unsafe_send_as(self, hdl,
                     make<downstream_msg::close>(slots, self->address()));
    else
      unsafe_send_as(self, hdl,
                     make<downstream_msg::forced_close>(slots, self->address(),
                                                        shutdown_reason));
  }
  if (shutdown_reason != none)
    unsafe_response(self, std::move(cd.hdl), no_stages, cd.mid,
                    std::move(shutdown_reason));
}

void outbound_path::handle_ack_open(long initial_credit) {
  open_credit = initial_credit;
  cd.hdl = nullptr;
}

void outbound_path::emit_open(strong_actor_ptr origin,
                              mailbox_element::forwarding_stack stages,
                              message_id handshake_mid, message handshake_data,
                              stream_priority prio, bool is_redeployable) {
  CAF_LOG_TRACE(CAF_ARG(origin) << CAF_ARG(stages) << CAF_ARG(handshake_mid)
                << CAF_ARG(handshake_data) << CAF_ARG(prio)
                << CAF_ARG(is_redeployable));
  cd = client_data{origin, handshake_mid};
  redeployable = is_redeployable;
  hdl->enqueue(
    make_mailbox_element(
      std::move(origin), handshake_mid, std::move(stages),
      make_message(open_stream_msg{slots.sender, std::move(handshake_data),
                                   self->ctrl(), hdl, prio, is_redeployable})),
    self->context());
}

void outbound_path::emit_batch(long xs_size, message xs) {
  CAF_LOG_TRACE(CAF_ARG(xs_size) << CAF_ARG(xs));
  open_credit -= xs_size;
  auto bid = next_batch_id++;
  downstream_msg::batch batch{static_cast<int32_t>(xs_size), std::move(xs),
                              bid};
  if (redeployable)
    unacknowledged_batches.emplace_back(bid, batch);
  unsafe_send_as(self, hdl,
                 downstream_msg{slots, self->address(), std::move(batch)});
}

void outbound_path::emit_irregular_shutdown(local_actor* self,
                                            stream_slots slots,
                                            const strong_actor_ptr& hdl,
                                            error reason) {
  CAF_LOG_TRACE(CAF_ARG(reason));
  unsafe_send_as(self, hdl,
                 make<downstream_msg::forced_close>(slots, self->address(),
                                                    std::move(reason)));
}

} // namespace caf
