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

#ifndef CAF_STREAM_RESULT_HPP
#define CAF_STREAM_RESULT_HPP

#include "caf/fwd.hpp"
#include "caf/none.hpp"
#include "caf/stream_slot.hpp"
#include "caf/stream_manager.hpp"

#include "caf/meta/type_name.hpp"

namespace caf {

/// Terminates a stream by reducing it to a single value.
template <class T>
class stream_result {
public:
  stream_result(stream_result&&) = default;
  stream_result(const stream_result&) = default;
  stream_result& operator=(stream_result&&) = default;
  stream_result& operator=(const stream_result&) = default;

  stream_result(none_t = none) : slot_(0) {
    // nop
  }

  stream_result(stream_slot id) : slot_(id) {
    // nop
  }

  /// Convenience constructor for returning the result of `self->new_stream_result`
  /// and similar functions.
  stream_result(stream_slot id, stream_manager_ptr sptr)
      : slot_(id),
        ptr_(std::move(sptr)) {
    // nop
  }

  /// Convenience constructor for returning the result of `self->new_stream_result`
  /// and similar functions.
  stream_result(stream_result other,  stream_manager_ptr sptr)
      : slot_(other.slot_),
        ptr_(std::move(sptr)) {
    // nop
  }

  /// Returns the unique identifier for this stream_result.
  inline stream_slot slot() const {
    return slot_;
  }

  /// Returns the handler assigned to this stream_result on this actor.
  inline const stream_manager_ptr& ptr() const {
    return ptr_;
  }

  template <class Inspector>
  friend typename Inspector::result_type inspect(Inspector& f,
                                                 stream_result& x) {
    return f(meta::type_name("stream_result"), x.slot_);
  }

private:
  stream_slot slot_;
  stream_manager_ptr ptr_;
};

} // namespace caf

#endif // CAF_STREAM_RESULT_HPP
