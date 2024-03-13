#include <dirent.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

void clean_env(void);
void welcome(void);
void fetch_input_hashes_from_DB(void);
void detect_hashType_generate_hashfiles(void);
void launch_hashcat(void);
void update_db(void);
