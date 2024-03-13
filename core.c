#include "core.h"
#include "config.h"

char *getCurrentDateTime(void) {
  static char datetime[50];
  time_t rawtime;
  struct tm *timeinfo;

  time(&rawtime);
  timeinfo = localtime(&rawtime);

  strftime(datetime, 50, "%Y-%m-%d %H:%M:%S", timeinfo);

  return datetime;
}

void remove_last_char(char *line) {
  uint16_t len = strlen(line);
  line[len - 1] = '\0';
}

void log_dump_to_file(char *type, char *msg) {
  char log[1024];
  sprintf(log, "echo '%s:%s:%s' >> log/log.txt", getCurrentDateTime(), type,
          msg);
  system(log);
}

void log_dump(char *type, char *msg) { log_dump_to_file(type, msg); }

void welcome(void) {
  printf("Welcome to hashcatEngine\n");
  log_dump("INFO", "hashcatEngine STARTED");
}

void clean_env(void) {
  system("rm -rf logs/*");
  system("rm -rf hashes_to_process/*");
  system("rm -rf hashes_cracked/*");
}

void fetch_input_hashes_from_DB(void) {
  log_dump("INFO", "input.hash fetched successfully");
}

void generate_hashfile(char *hash, char *hashType) {
  char log[1024];
  sprintf(log, "echo '%s' >> hashes_to_process/%s", hash, hashType);
  system(log);
}

void launch_haiti(char *hash) {
  FILE *fp;
  char buffer[2048];
  char command[2048];
  sprintf(command, HAITI, hash);

  fp = popen(command, "r");
  if (fp == NULL) {
    log_dump("ERROR", "Failed to run haiti");
    return;
  }

  while (fgets(buffer, 2048, fp) != NULL) {
    remove_last_char(buffer);
    generate_hashfile(hash, buffer);
  }

  pclose(fp);
  return;
}

void handle_hashType(char *hash, char *hashType) {
  char *token = strtok(hashType, ",");
  while (token != NULL) {
    generate_hashfile(hash, token);
    token = strtok(NULL, ",");
  }
}

void detect_hashtype(char *hash, char *hashType) {
  if (strcmp(hashType, "-1") == 0)
    launch_haiti(hash);
  else
    handle_hashType(hash, hashType);
}

void parse_hashtype(char *line) {
  char hash[1024] = {0};
  char hashType[32] = {0};

  uint8_t loop = 0;
  char *token = strtok(line, ":");
  while (token != NULL) {
    switch (loop) {
    case 0:
      strcpy(hash, token);
      break;
    case 1:
      remove_last_char(token);
      strcpy(hashType, token);
      break;
    default:
      log_dump("ERROR", "input.hash parsing error");
      return;
    }
    token = strtok(NULL, ":");
    loop++;
  }
  detect_hashtype(hash, hashType);
}

void detect_hashType_generate_hashfiles(void) {
  FILE *file;
  char line[2048];

  file = fopen("DB_input/input.hash", "r");
  if (file == NULL) {
    log_dump("ERROR", "Failed to open input.hash file");
    return;
  }

  while (fgets(line, 2048, file) != NULL) {
    parse_hashtype(line);
  }

  fclose(file);
  return;
}

void hashcat(char *hash) {
  char log[1024];
  sprintf(log, HASHCAT, hash, hash, hash);
  printf("HASHCAT:%s\n", log);
  system(log);
}

void launch_hashcat(void) {
  DIR *dir;
  struct dirent *entry;

  dir = opendir("hashes_to_process");
  if (dir == NULL) {
    log_dump("ERROR", "Failed to open input directory");
    return;
  }

  while ((entry = readdir(dir)) != NULL) {
    if (entry->d_type == DT_REG) {
      hashcat(entry->d_name);
    }
  }

  closedir(dir);

  return;
}
void insert_db(char *line) { printf("%s", line); }

void readTextFromFile(const char *filepath) {
  FILE *file = fopen(filepath, "r");
  if (file == NULL) {
    log_dump("ERROR", "Failed to open .hash file");
    return;
  }

  char line[1024];
  while (fgets(line, 1024, file) != NULL) {
    insert_db(line);
  }

  fclose(file);
}

void readTextFromFilesInFolder(const char *folderpath) {
  DIR *dir;
  struct dirent *entry;
  char filepath[1024];

  dir = opendir(folderpath);
  if (dir == NULL) {
    log_dump("ERROR", "Failed to open hashes_cracked directory");
    return;
  }

  while ((entry = readdir(dir)) != NULL) {
    if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0) {
      snprintf(filepath, sizeof(filepath), "%s/%s", folderpath, entry->d_name);

      if (entry->d_type == DT_DIR) {
        readTextFromFilesInFolder(filepath);
      } else {
        readTextFromFile(filepath);
      }
    }
  }

  closedir(dir);
}
void update_db(void) { readTextFromFilesInFolder("hashes_cracked"); }
