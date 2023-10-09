#ifndef __LOAD_KEF_H__
#define __LOAD_KEF_H__

void load_kef_init(int argc, char* argv[]);
int load_kef_by_id(unsigned char app_id, pid_t parent, int run, const char* cmdl);
int load_kef_by_name(const char* app_name, pid_t parent, int run, const char* cmdl);

#define load_app(id)    load_kef_by_id(id, 0, 0, NULL)

#endif
