#ifndef THREAD_POOL_SCHEDULER_HPP
#define THREAD_POOL_SCHEDULER_HPP

#include "cppa/scheduler.hpp"
#include "cppa/detail/thread.hpp"
#include "cppa/detail/scheduled_actor.hpp"

namespace cppa { namespace detail {

class thread_pool_scheduler : public scheduler
{

    typedef scheduler super;

 public:

    struct worker;

    void start() /*override*/;

    void stop() /*override*/;

    void schedule(scheduled_actor* what) /*override*/;

    actor_ptr spawn(event_based_actor* what);

    actor_ptr spawn(actor_behavior* behavior, scheduling_hint hint);

 private:

    typedef util::single_reader_queue<scheduled_actor> job_queue;

    job_queue m_queue;
    scheduled_actor_dummy m_dummy;
    thread m_supervisor;

    actor_ptr spawn_impl(scheduled_actor* what);

    static void worker_loop(worker*);
    static void supervisor_loop(job_queue*, scheduled_actor*);

};

} } // namespace cppa::detail

#endif // THREAD_POOL_SCHEDULER_HPP
