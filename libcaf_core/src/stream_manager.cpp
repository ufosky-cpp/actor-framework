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

#include "caf/stream_manager.hpp"

#include "caf/actor_addr.hpp"
#include "caf/actor_cast.hpp"
#include "caf/actor_control_block.hpp"
#include "caf/error.hpp"
#include "caf/expected.hpp"
#include "caf/inbound_path.hpp"
#include "caf/local_actor.hpp"
#include "caf/logger.hpp"
#include "caf/message.hpp"
#include "caf/outbound_path.hpp"
#include "caf/response_promise.hpp"
#include "caf/sec.hpp"
#include "caf/stream_scatterer.hpp"

namespace caf {

stream_manager::stream_manager(local_actor* selfptr)
    : self_(selfptr),
      pending_handshakes_(0) {
  // nop
}

stream_manager::~stream_manager() {
  // nop
}

error stream_manager::handle(inbound_path*, downstream_msg::batch&) {
  return none;
}

error stream_manager::handle(inbound_path* from, downstream_msg::close&) {
  out().take_path(from->slots);
  return none;
}

error stream_manager::handle(inbound_path*, downstream_msg::forced_close&) {
  return none;
}

error stream_manager::handle(stream_slots slots, upstream_msg::ack_open& x) {
  auto path = out().add_path(slots.invert(), x.rebind_to);
  path->open_credit = x.initial_demand;
  path->desired_batch_size = x.desired_batch_size;
  --pending_handshakes_;
  generate_messages();
  push();
  return none;
}

error stream_manager::handle(outbound_path*, upstream_msg::ack_batch&) {
  return none;
}

error stream_manager::handle(outbound_path*, upstream_msg::drop&) {
  return none;
}

error stream_manager::handle(outbound_path*, upstream_msg::forced_drop&) {
  return none;
}

void stream_manager::close() {
  out().close();
  // TODO: abort all input pahts as well
}

void stream_manager::abort(error reason) {
  out().abort(std::move(reason));
  // TODO: abort all input pahts as well
}

void stream_manager::push() {
  CAF_LOG_TRACE("");
  do {
    out().emit_batches();
  } while (generate_messages());
}

bool stream_manager::congested() const {
  return false;
}

void stream_manager::send_handshake(strong_actor_ptr dest, stream_slot slot,
                                    strong_actor_ptr client,
                                    mailbox_element::forwarding_stack fwd_stack,
                                    message_id mid, stream_priority prio) {
  CAF_ASSERT(dest != nullptr);
  ++pending_handshakes_;
  dest->enqueue(
    make_mailbox_element(
      std::move(client), mid, std::move(fwd_stack),
      open_stream_msg{slot, make_handshake(), self_->ctrl(), dest, prio}),
    self_->context());
}

void stream_manager::send_handshake(strong_actor_ptr dest, stream_slot slot,
                                    stream_priority prio) {
  mailbox_element::forwarding_stack fwd_stack;
  send_handshake(std::move(dest), slot, nullptr, std::move(fwd_stack),
                 make_message_id(), prio);
}

bool stream_manager::generate_messages() {
  return false;
}

void stream_manager::register_input_path(inbound_path* ptr) {
  inbound_paths_.emplace_back(ptr);
}

void stream_manager::deregister_input_path(inbound_path* ptr) noexcept {
  using std::swap;
  if (ptr != inbound_paths_.back()) {
    auto i = std::find(inbound_paths_.begin(), inbound_paths_.end(), ptr);
    CAF_ASSERT(i != inbound_paths_.end());
    swap(*i, inbound_paths_.back());
  }
  inbound_paths_.pop_back();
}

message stream_manager::make_final_result() {
  return none;
}

error stream_manager::process_batch(message&) {
  CAF_LOG_ERROR("stream_manager::process_batch called");
  return sec::invalid_stream_state;
}

void stream_manager::output_closed(error) {
  // nop
}

message stream_manager::make_handshake() const {
  CAF_LOG_ERROR("stream_manager::make_output_token called");
  return none;
}

void stream_manager::downstream_demand(outbound_path*, long) {
  CAF_LOG_ERROR("stream_manager::downstream_demand called");
}

void stream_manager::input_closed(error) {
  // nop
}

} // namespace caf
