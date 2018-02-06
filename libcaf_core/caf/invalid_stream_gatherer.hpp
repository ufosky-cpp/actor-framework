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

#ifndef CAF_INVALID_STREAM_GATHERER_HPP
#define CAF_INVALID_STREAM_GATHERER_HPP

#include "caf/stream_gatherer.hpp"

namespace caf {

/// Type-erased policy for receiving data from sources.
class invalid_stream_gatherer : public stream_gatherer {
public:
  invalid_stream_gatherer() = default;

  ~invalid_stream_gatherer() override;

  path_ptr add_path(stream_slot slot, strong_actor_ptr x,
                    strong_actor_ptr original_stage, stream_priority prio,
                    long available_credit, bool redeployable,
                    response_promise result_cb) override;

  bool remove_path(stream_slot slot, const actor_addr& x, error reason,
                   bool silent) override;

  void close(message reason) override;

  void abort(error reason) override;

  long num_paths() const override;

  bool closed() const override;

  bool continuous() const override;

  void continuous(bool value) override;

  path_ptr path_at(size_t index) override;

  path_ptr find(stream_slot slot, const actor_addr& x) override;

  void assign_credit(long downstream_capacity) override;

  long initial_credit(long downstream_capacity, path_type* x) override;
};

} // namespace caf

#endif // CAF_INVALID_STREAM_GATHERER_HPP
