#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#define CURRENT_DIR "."
#define INIT_FILE "init.json"
#define REPO_DIR "./Repo"
#define MAKEFILE "Makefile"
#define PROGRAM_TARGET "main"
#define PROGRAM_SRC "main.c"

// Main functions
int init();
int push();
int add();

// Helper functions
int has_init();
int write_to_init();
int open_directory(const char *path, DIR **dir);
int move_files();
void handle_file_move(const char *filename);

// Main function
int main(int argc, char **argv) {
  if (argc < 2) {
    printf("You must have at least one argument.\n");
    return EXIT_FAILURE;
  }

  if (strcmp(argv[1], "init") == 0) {
    return init();
  } else if (strcmp(argv[1], "push") == 0) {
    return push();
  }

  return EXIT_FAILURE;
}

// Main functions
int init() {
  if (has_init() == 1) {
    printf("Repository already initialized.\n");
    return EXIT_FAILURE;
  }

  write_to_init();
  printf("init.json has been created successfully.\n");
  return EXIT_SUCCESS;
}

int push() {
  if (has_init() == 0) {
    printf("Repository already added to git.\n");
    return EXIT_FAILURE;
  }

  struct stat st = {0};
  if (stat(REPO_DIR, &st) == -1) {
    if (mkdir(REPO_DIR, 0777) == -1) {
      perror("Error creating directory");
      return EXIT_FAILURE;
    } else {
      printf("Directory '%s' created successfully.\n", REPO_DIR);
    }
  } else {
    system("rm -rf ./Repo");
    if (mkdir(REPO_DIR, 0777) == -1) {
      perror("Error creating Repo directory");
      return EXIT_FAILURE;
    }
    move_files();
  }

  return EXIT_SUCCESS;
}

// Helper functions
int has_init() {
  DIR *dir;
  struct dirent *entry;

  if (open_directory(CURRENT_DIR, &dir) == EXIT_FAILURE) {
    return EXIT_FAILURE;
  }

  while ((entry = readdir(dir)) != NULL) {
    if (strcmp(entry->d_name, INIT_FILE) == 0) {
      closedir(dir);
      return EXIT_FAILURE; // init.json found, meaning repo already initialized
    }
  }

  closedir(dir);
  return EXIT_SUCCESS;
}

int write_to_init() {
  FILE *file = fopen(INIT_FILE, "w");
  if (!file) {
    perror("Failed to open file \"init.json\"");
    return EXIT_FAILURE;
  }

  DIR *dir;
  struct dirent *entry;
  if (open_directory(CURRENT_DIR, &dir) == EXIT_FAILURE) {
    fclose(file);
    return EXIT_FAILURE;
  }

  while ((entry = readdir(dir)) != NULL) {
    if (strcmp(entry->d_name, INIT_FILE) != 0 &&
        strcmp(entry->d_name, CURRENT_DIR) != 0 &&
        strcmp(entry->d_name, "..") != 0 &&
        strcmp(entry->d_name, ".git") != 0) {
      fprintf(file, "%s\n", entry->d_name);
    }
  }

  fclose(file);
  closedir(dir);
  return EXIT_SUCCESS;
}

int open_directory(const char *path, DIR **dir) {
  *dir = opendir(path);
  if (*dir == NULL) {
    perror("Failed to open directory");
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}

void handle_file_move(const char *filename) {
  FILE *file;
  FILE *copied_file;
  long length;
  char *file_content;
  char new_file_path[1024];

  file = fopen(filename, "r");
  if (!file) {
    printf("Failed to open file %s\n", filename);
    return;
  }

  fseek(file, 0, SEEK_END);
  length = ftell(file);
  fseek(file, 0, SEEK_SET);

  file_content = malloc(length + 1);
  if (!file_content) {
    printf("Failed to allocate memory for %s\n", filename);
    fclose(file);
    return;
  }

  int bytes_read = fread(file_content, 1, length, file);
  if (bytes_read != length) {
    printf("Failed to read the entire file %s\n", filename);
    fclose(file);
    free(file_content);
    return;
  }

  file_content[length] = '\0';

  snprintf(new_file_path, sizeof(new_file_path), "%s/%s", REPO_DIR, filename);

  copied_file = fopen(new_file_path, "w");
  if (!copied_file) {
    printf("Failed to create new file in Repo: %s\n", new_file_path);
    free(file_content);
    fclose(file);
    return;
  }

  fwrite(file_content, 1, length, copied_file);
  printf("File content copied to: %s\n", new_file_path);

  fclose(copied_file);
  free(file_content);
  fclose(file);
}

int move_files() {
  DIR *dir;
  struct dirent *entry;

  if (open_directory(CURRENT_DIR, &dir) == EXIT_FAILURE) {
    return EXIT_FAILURE;
  }

  while ((entry = readdir(dir)) != NULL) {
    if (strcmp(entry->d_name, "Repo") != 0 && strcmp(entry->d_name, ".") != 0 &&
        strcmp(entry->d_name, "..") != 0 &&
        strcmp(entry->d_name, ".git") != 0 &&
        strcmp(entry->d_name, INIT_FILE) != 0 &&
        strcmp(entry->d_name, MAKEFILE) != 0 &&
        strcmp(entry->d_name, PROGRAM_TARGET) != 0 &&
        strcmp(entry->d_name, PROGRAM_SRC) != 0) {
      handle_file_move(entry->d_name);
    }
  }

  closedir(dir);
  return EXIT_SUCCESS;
}
