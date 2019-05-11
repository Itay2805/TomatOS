#ifndef TOMATKERNEL_SHELL_H
#define TOMATKERNEL_SHELL_H

bool resolve_and_open(const char* str, resource_t* resource);

void start_shell(const char* mount_point);

#endif //TOMATKERNEL_SHELL_H
