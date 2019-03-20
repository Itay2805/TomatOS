#ifndef COMMON_STDBOOL_H
#define COMMON_STDBOOL_H

#if !defined(__cplusplus) || !defined(__bool_true_false_are_defined)
    #define __bool_true_false_are_defined
    
    typedef unsigned char bool;
    #define true ((bool)1)
    #define false ((bool)0)
#endif

#endif