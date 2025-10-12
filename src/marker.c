#include "marker.h"


void marker_init();
void *marker_realloc(void *array, size_t array_size, int current_length);
unsigned char marker_file_step_and_store(struct marker *m, int current_position, FILE *fp);
int marker_unpack(struct marker *marker, FILE *fp);
int marker_unpack_UKN(struct marker *marker, FILE *fp);
int marker_unpack_APP(struct marker *marker, FILE *fp);
int marker_unpack_DQT(struct marker *marker, FILE *fp);
int marker_unpack_SOF(struct marker *marker, FILE *fp);
int marker_unpack_DHT(struct marker *marker, FILE *fp);
int marker_unpack_SOS(struct marker *marker, FILE *fp);
int marker_unpack_COM(struct marker *marker, FILE *fp);

void marker_init() {
    return;
};

void *marker_realloc(void *array, size_t array_size, int current_length) {
  void *tmp = realloc(array, array_size * (current_length + 1));
  if (!tmp) {
    log_status(1, "Realloc failed, exiting the program");
    exit(1);
  }

  return tmp;
}

unsigned char marker_file_step_and_store(struct marker *m, int current_position, FILE *fp) {
  unsigned char current_character = getc(fp);
  log_verbose(current_character);
  m->data[current_position] = current_character;
  return m->data[current_position];
}

// TODO: check this function does as its intended
void marker_free(struct marker *m, int marker_count) {
  for (int i = 0; i < marker_count; i++) {
    switch (m->code) {
      case MARKER_APP: break;
      case MARKER_DQT: break;
      case MARKER_SOF: {
        free(m->sof.factor_table);
        m->sof.factor_table = NULL;
        break;
      }
      case MARKER_DHT: {
        free(m->dht.table_class);
        m->dht.table_class = NULL;
        free(m->dht.table_destination);
        m->dht.table_destination = NULL;
        free(m->dht.number_of_bytes);
        m->dht.number_of_bytes = NULL;

        for (int i = 0; i < m->dht.number_of_tables; i++) {
          for (int j = 0; j < sizeof(*m->dht.bytes[i]) / sizeof(*m->dht.bytes[i][0]); j++) {
            free(m->dht.bytes[i][j]);
            m->dht.bytes[i][j] = NULL;
          }
          free(m->dht.bytes[i]);
          m->dht.bytes[i] = NULL;
        }

        break;
      }
      case MARKER_SOS: {
        free(m->sos.component_selector);
        m->sos.component_selector = NULL;
        free(m->sos.dc_table_selector);
        m->sos.dc_table_selector = NULL;
        free(m->sos.ac_table_selector);
        m->sos.ac_table_selector = NULL;
        break;
      }
    }
    free(m->data);
    m->data = NULL;
  }
  log_status(0, "Memory freed");
  return;
};

struct marker* marker_unpack_image (FILE *fp, int *marker_count) {
  int scan_status = 0;
  int check_character = 0;
  struct marker *marker_list = NULL;

  // TODO: see if i can remove the excess nesting
  // TODO: add a secondary file check (FF D8)
  while ((check_character = getc(fp)) != EOF) {
    unsigned char current_character = (unsigned char) check_character;
    log_verbose(current_character);


    if ((current_character == 0xFF) && (!scan_status)) {
      marker_list = marker_realloc(marker_list, sizeof(*marker_list), *marker_count);

      marker_unpack(&marker_list[*marker_count], fp);
      
      if (marker_list[(*marker_count)++].code == MARKER_SOS) {
        scan_status = 1;

        marker_list = marker_realloc(marker_list, sizeof(*marker_list), *marker_count);

        marker_list[*marker_count].code = MARKER_DATA;
        marker_list[*marker_count].length = 0;
        marker_list[*marker_count].data = NULL;
      }
    } else if (scan_status) {
      if (current_character == 0xFF) {
        unsigned char nextChar = getc(fp);
        log_verbose(nextChar);
        if (nextChar == 0x00) {
          marker_list[*marker_count].data = marker_realloc(marker_list[*marker_count].data, sizeof(*marker_list[*marker_count].data), marker_list[*marker_count].length);
          marker_list[*marker_count].data[marker_list[*marker_count].length++] = 0xFF;
          continue;
        }
        fseek(fp, -2L, SEEK_CUR);
        scan_status = 0;
        log_marker(&marker_list[(*marker_count)++]);
        continue;
      }
      marker_list[*marker_count].data = marker_realloc(marker_list[*marker_count].data, sizeof(*marker_list[*marker_count].data), marker_list[*marker_count].length);
      marker_list[*marker_count].data[marker_list[*marker_count].length++] = current_character;
    } else {
      log_status(1, "The JPEG was malformed or there was an error in the code, check the logs");
      return NULL;
    }
  }

  return marker_list;
}

// Marker Unpack Functions
int marker_unpack(struct marker *m, FILE *fp) {
    unsigned char current_character = getc(fp);
    log_verbose(current_character);
    m->code = current_character;
    m->length = 0;
    m->data = NULL;

    if ((m->code == MARKER_SOI) || (m->code == MARKER_EOI)) {
      log_marker(m);
      return 0;
    }

    int high_nibble;
    int low_nibble;

    // Grab marker length
    for (int i = 0; i < 2; i++) {
        current_character = getc(fp);
        log_verbose(current_character);
        if (i == 0) {
        high_nibble = current_character;
        m->length = current_character << 8;
        } else {
        low_nibble = current_character;
        m->length |= current_character;
        }
    }

    m->data = calloc(m->length, sizeof(unsigned char));
    m->data[0] = high_nibble;
    m->data[1] = low_nibble;
    
    // Grab marker Data
    switch (m->code) {
        case MARKER_APP: marker_unpack_APP(m, fp); break;
        case MARKER_DQT: marker_unpack_DQT(m, fp); break;
        case MARKER_SOF: marker_unpack_SOF(m, fp); break;
        case MARKER_DHT: marker_unpack_DHT(m, fp); break;
        case MARKER_SOS: marker_unpack_SOS(m, fp); break;
        case MARKER_COM: marker_unpack_COM(m, fp); break;
        default: {
        log_status(1, "this marker hasnt been implemented yet");
        marker_unpack_UKN(m, fp);
        }
    }

    log_marker(m);

    return 0;
}

// TODO: Try to consolidate these calls, maybe look at dispatch tables?
int marker_unpack_UKN(struct marker *m, FILE *fp) {
  unsigned char current_character = 0;
  int length = m->length - 2;
  int current_position = 2;

  for (int i = 0; i < length; i++) {
    current_character = marker_file_step_and_store(m, current_position++, fp);
  }

  return 0;
}

int marker_unpack_APP(struct marker *m, FILE *fp) {
  unsigned char current_character = 0;
  int length = m->length - 2;
  int current_position = 2;

  for (int i = 0; i < length; i++) {
    current_character = marker_file_step_and_store(m, current_position++, fp);
    // I know this could be a list but i want the data to be more explicit
    // APP length should always be 16
    switch (i) {
      case 6: m->app.major_version = current_character; break;
      case 7: m->app.minor_version = current_character; break;
      case 8: m->app.units = current_character; break;
      case 9: m->app.density_x = current_character << 8; break;
      case 10: m->app.density_x |= current_character; break;
      case 11: m->app.density_y = current_character << 8; break;
      case 12: m->app.density_y |= current_character; break;
      case 13: m->app.thumbnail_x = current_character; break;
      case 14: m->app.thumbnail_y = current_character; break;
      default: m->app.identifier[i] = current_character;
    }
  }

  return 0;
}

int marker_unpack_DQT(struct marker *m, FILE *fp) {
  unsigned char current_character = 0;
  int length = m->length - 2;
  int current_position = 2;

  current_character = marker_file_step_and_store(m, current_position++, fp);
  m->dqt.destination = current_character;
  length--;

  // FIXME: Make sure DQT is always 8x8
  // From Page 119 this can be 8 or 16 bits
  // For now this is fine
  for (int i = 0; i < 8; i++) {
    for (int j = 0; j < 8; j++) {
      current_character = marker_file_step_and_store(m, current_position++, fp);
      m->dqt.table[i][j] = current_character;
      length--;
    }
  }

  if (length != 0) log_status(0, "deocode_unpack_DQT ran into an error");

  return 0;
}

int marker_unpack_SOF(struct marker *m, FILE *fp) {
  unsigned char current_character = 0;
  int length = m->length - 2;
  int current_position = 2;

  // This first part should always be a length of 8
  for (int i = 0; i < length; i++) {
    current_character = marker_file_step_and_store(m, current_position++, fp);

    switch (i) {
      case 0: m->sof.precision = current_character; break;
      case 1: m->sof.lines = current_character << 8; break;
      case 2: m->sof.lines |= current_character; break;
      case 3: m->sof.samples = current_character << 8; break;
      case 4: m->sof.samples |= current_character; break;
      case 5: {
        m->sof.components = current_character;
        m->sof.factor_table = malloc(sizeof(unsigned char) * m->sof.components * 3);
        break;
      }
      default: {
        // FIXME: find a different way to do this
        // Factor Table ID: 8 bits
        // Horizontal Scaling: 4 bits
        // Vertical Scaling: 4 bits
        // Quant Table Selector: 8 bits
        // Hori and Vert Scaling share a byte
        m->sof.factor_table[i - 6] = current_character;
      }
    }
  }

  return 0;
}

int marker_unpack_DHT(struct marker *m, FILE *fp) {
  unsigned char current_character = 0;
  int length = m->length - 2;
  int current_position = 2;

  m->dht.number_of_tables = 0;
  m->dht.table_class = NULL;
  m->dht.table_destination = NULL;
  m->dht.number_of_bytes = NULL;
  m->dht.bytes = NULL;

  while (length > 0) {
    current_character = marker_file_step_and_store(m, current_position++, fp);

    m->dht.table_class = marker_realloc(m->dht.table_class, sizeof(*m->dht.table_class), m->dht.number_of_tables);
    m->dht.table_class[m->dht.number_of_tables] = current_character >> 4; // (current_character >> 4) & 0x0F
    if (m->dht.table_class[m->dht.number_of_tables] > 3) {
      log_status(1, "Error in DHT class");
      exit(1);
    }

    m->dht.table_destination = marker_realloc(m->dht.table_destination, sizeof(*m->dht.table_destination), m->dht.number_of_tables);
    m->dht.table_destination[m->dht.number_of_tables] = current_character & 0x0F;

    length--;

    m->dht.number_of_bytes = marker_realloc(m->dht.number_of_bytes, sizeof(*m->dht.number_of_bytes), m->dht.number_of_tables);
    m->dht.number_of_bytes[m->dht.number_of_tables] = calloc(HUFFMAN_CODE_LENGTH, sizeof(unsigned char));

    m->dht.bytes = marker_realloc(m->dht.bytes, sizeof(*m->dht.bytes), m->dht.number_of_tables);
    m->dht.bytes[m->dht.number_of_tables] = calloc(HUFFMAN_CODE_LENGTH, sizeof(unsigned char *));

    for (int i = 0; i < HUFFMAN_CODE_LENGTH; i++) {
      current_character = marker_file_step_and_store(m, current_position++, fp);
      m->dht.number_of_bytes[m->dht.number_of_tables][i] = current_character;
      m->dht.bytes[m->dht.number_of_tables][i] = malloc(m->dht.number_of_bytes[m->dht.number_of_tables][i]);
      length--;
    }

    for (int i = 0; i < HUFFMAN_CODE_LENGTH; i++) {
      if (m->dht.number_of_bytes[m->dht.number_of_tables][i] == 0) continue;

      for (int j = 0; j < m->dht.number_of_bytes[m->dht.number_of_tables][i]; j++) {
        current_character = marker_file_step_and_store(m, current_position++, fp);
        m->dht.bytes[m->dht.number_of_tables][i][j] = current_character;
        length--;
      }
      
    }
    m->dht.number_of_tables++;
  }

  if (length != 0) log_status(0, "deocode_unpack_DHT ran into an error");

  return 0;
}

int marker_unpack_SOS(struct marker *m, FILE *fp) {
  unsigned char current_character = 0;
  int length = m->length - 2;
  int current_position = 2;

  current_character = marker_file_step_and_store(m, current_position++, fp);
  m->sos.components = current_character;
  
  m->sos.component_selector = calloc(m->sos.components, sizeof(unsigned char));
  m->sos.dc_table_selector = calloc(m->sos.components, sizeof(unsigned char));
  m->sos.ac_table_selector = calloc(m->sos.components, sizeof(unsigned char));
  length--;

  

  for (int i = 0; i < m->sos.components; i++) {
    current_character = marker_file_step_and_store(m, current_position++, fp);
    m->sos.component_selector[i] = current_character;

    current_character = marker_file_step_and_store(m, current_position++, fp);
    m->sos.dc_table_selector[i] = current_character >> 4;
    m->sos.ac_table_selector[i] = current_character & 0x0F;

    length -= 2;
  }

  current_character = marker_file_step_and_store(m, current_position++, fp);
  m->sos.spectral_select_start = current_character;
  length--;

  current_character = marker_file_step_and_store(m, current_position++, fp);
  m->sos.spectral_select_end = current_character;
  length--;

  current_character = marker_file_step_and_store(m, current_position++, fp);
  m->sos.successive_high = current_character >> 4;
  m->sos.successive_low = current_character & 0x0F;
  length--;

  if (length != 0) log_status(0, "deocode_unpack_SOS ran into an error");

  return 0;
}

int marker_unpack_COM(struct marker *m, FILE *fp) {
  unsigned char current_character = 0;
  int length = m->length - 2;
  m->com.comment_length = length; // redundant I know
  int current_position = 2;

  m->com.comment = malloc(sizeof(char) * m->com.comment_length);

  for (int i = 0; i < length; i++) {
    current_character = getc(fp);
    log_verbose(current_character);
    m->data[current_position++] = current_character;
    m->com.comment[i] = current_character;
  }
  m->com.comment[length] = '\0'; // TODO: maybe rethink how i do this or read this

  return 0;
}