#include "helper.h"

void *helper_realloc(void *array, size_t array_size, int current_length) {
  void *tmp = realloc(array, array_size * (current_length + 1));
  if (!tmp) {
    log_status(1, "Realloc failed, exiting the program");
    exit(1);
  }

  return tmp;
}