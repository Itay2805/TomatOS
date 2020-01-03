#ifndef TOMATKERNEL_DEBUG_H
#define TOMATKERNEL_DEBUG_H

#include <frg/logging.hpp>

namespace util {

    using frg_logger = frg::stack_buffer_logger<void(*)(const char*)>;

    /**
     * Pointer to the logger
     */
    extern frg_logger* logger;

    /**
     * Get the logger that we use for shit
     */
    inline frg_logger::item log() { return logger->operator()(); }

    /**
     * Initialize the debug logger so we
     * can use it before the constructors
     * are called
     */
    void init_debug_logger();
}


// TODO: Have this shutdown all cores and shit
#define ASSERT(expr) \
    do { \
        if(!(expr)) { \
            ::util::log() << "[-] Assertion failed at " << __FILENAME__ << ":" << __LINE__ << "!" << frg::endlog; \
            while(1); \
        } \
    } while(0)


#endif //TOMATKERNEL_DEBUG_H
