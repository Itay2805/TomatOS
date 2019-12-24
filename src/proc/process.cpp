#include <util/cpp_runtime.h>
#include <string.h>
#include <util/critical_section.hpp>
#include "process.hpp"
#include "thread.hpp"

namespace proc {

    static int next_pid = 1;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // process class
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    process::process(bool kproc)
        : _pid(next_pid++)
    {
        if(kproc) {
            // only a single kernel process can exist
            ASSERT(pid() == 1);
            this->vmm_context = mem::vmm::kernel;
        }else {
            this->vmm_context = new (vmm_context_buffer) mem::vmm::context();
        }
    }

    process::~process() {
        // make sure this is at the same place
        ASSERT(this->vmm_context == reinterpret_cast<mem::vmm::context*>(this->vmm_context_buffer));
        vmm_context->~context();
    }

    util::shared_ptr<thread> process::create_thread() {
        util::critical_section cs(lock());
        auto thread = util::make_shared<proc::thread>(processes.get(this->pid()));
        this->threads.put(thread->tid(), thread);
        return thread;
    }

    void process::kill() {
        // make sure not the kernel process
        ASSERT(this != kernel.get());

        // delete all the threads
        // make sure we also unlock this at the end
        {
            util::critical_section cs(this->lock());
            while(this->threads.len()) {
                this->threads.del(this->threads.key_at(this->threads.len() - 1));
            }
        }

        // remove the process from the process list
        util::critical_section cs(proc::lock);
        processes.del(this->pid());
        //******************************
        // DO NOT RUN CODE AFTER THIS
        //
        // Because we are deleting what
        // that might be the only reference
        // to the thread, we are probably
        // deleted and the memory of us
        // is invalid
        //
        //******************************
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // kernel process related
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    util::shared_ptr<process> kernel;

    util::spinlock lock;
    util::map<int, util::shared_ptr<process>> processes;

    util::shared_ptr<process> create_process(bool kernel) {
        util::critical_section cs(lock);
        auto proc = util::make_shared<process>(kernel);
        processes.put(proc->pid(), proc);
        return proc;
    }


}
