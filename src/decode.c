#include "decode.h"

static FILE *image_pointer = NULL;

int decode_JPEG(char image[256]) {

  if ((image_pointer = fopen(image, "rb")) == NULL) {
		log_status(1, "Image could not be opened");
    return 1;
	} else {
    char msg[1024];
    snprintf(msg, sizeof(msg), "%s opened", image);
    log_status(0, msg);
  }

  struct marker *m = marker_unpack_image(image_pointer);
  if (m == NULL) {
    return 1;
  }
  marker_free(m);
  
	fclose(image_pointer);

	return 0;
}