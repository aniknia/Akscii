#include "marker.h"

void marker_init() {
    return;
};

void *marker_realloc(void *array, size_t array_size, int current_length) {
  int *tmp = realloc(array, array_size * (current_length + 1));
  if (!tmp) {
    log_status(1, "Realloc failed, exiting the program");
    exit(1);
  }

  return tmp;
}

void marker_free(struct marker *m) {
  for (int i = 0; i < sizeof(m) / sizeof(m[0]); i++) {
    switch (m->code) {
      case MARKER_APP: break;
      case MARKER_DQT: break;
      case MARKER_SOF: {
        free(m->sof.factor_table);
        m->sof.factor_table = NULL;
        break;
      }
      case MARKER_DHT: {
        free(m->dht.class);
        m->dht.class = NULL;
        free(m->dht.destination);
        m->dht.destination = NULL;
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

struct marker* marker_unpack_image (FILE *fp) {
  int scan_status = 0;
  int current_marker = 0;
  int current_character = 0;
  struct marker *marker_list = NULL;

  // TODO: see if i can remove the excess nesting
  // TODO: add a secondary file check (FF D8)
  while ((current_character = getc(fp)) != EOF) {
    log_verbose(current_character);

    if ((current_character == 0xFF) && (!scan_status)) {
      struct marker *tmp = realloc(marker_list, sizeof(*marker_list) * (current_marker + 1));
      if (!tmp) {
        marker_free(marker_list);
        log_status(1, "Realloc failed, returning NULL pointer");
        return NULL;
      }
      marker_list = tmp;

      marker_unpack(&marker_list[current_marker], fp);
      
      if (marker_list[current_marker++].code == MARKER_SOS) {
        scan_status = 1;

        struct marker *tmp = realloc(marker_list, sizeof(*marker_list) * (current_marker + 1));
        if (!tmp) {
          marker_free(marker_list);
          log_status(1, "Realloc failed, returning NULL pointer");
          return NULL;
        }
        marker_list = tmp;

        marker_list[current_marker].code = MARKER_DATA;
        marker_list[current_marker].length = 0;
        marker_list[current_marker].data = NULL;
      }
    } else if (scan_status) {
      struct marker *m = &marker_list[current_marker];
      if (current_character == 0xFF) {
        int nextChar = getc(fp);
        log_verbose(nextChar);
        if (nextChar == 0x00) {
          m->data = marker_realloc(m->data, sizeof(*m->data), m->length);
          m->data[m->length++] = 0xFF;
          continue;
        }
        ungetc(nextChar, fp);
        ungetc(current_character, fp);
        scan_status = 0;
        log_marker(&marker_list[current_marker]);
        current_marker++;
        continue;
      }
      m->data = marker_realloc(m->data, sizeof(*m->data), m->length);
      m->data[m->length++] = 0xFF;
    } else {
      log_status(1, "The JPEG was malformed or there was an error in the code, check the logs");
      return NULL;
    }
  }

  return marker_list;
}

// Marker Unpack Functions
int marker_unpack(struct marker *m, FILE *fp) {
    int current_character = getc(fp);
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

    m->data = malloc(sizeof(int) * m->length);
    m->data[0] = high_nibble;
    m->data[1] = low_nibble;

    // Grab marker Data
    switch (m->code) {
        case MARKER_APP: marker_unpack_APP(m, fp); break;
        case MARKER_DQT: marker_unpack_DQT(m, fp); break;
        case MARKER_SOF: marker_unpack_SOF(m, fp); break;
        case MARKER_DHT: marker_unpack_UKN(m, fp); break;
        case MARKER_SOS: marker_unpack_SOS(m, fp); break;
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
  int current_character = 0;
  int length = m->length - 2;
  int current_position = 2;

  for (int i = 0; i < length; i++) {
    current_character = getc(fp);
    log_verbose(current_character);
    m->data[current_position++] = current_character;
  }

  return 0;
}

int marker_unpack_APP(struct marker *m, FILE *fp) {
  int current_character = 0;
  int length = m->length - 2;
  int current_position = 2;

  for (int i = 0; i < length; i++) {
    current_character = getc(fp);
    log_verbose(current_character);
    m->data[current_position++] = current_character;
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
  int current_character = 0;
  int length = m->length - 2;
  int current_position = 2;

  current_character = getc(fp);
  log_verbose(current_character);
  m->dqt.destination = current_character;
  m->data[current_position++] = current_character;
  length--;

  // FIXME: Make sure DQT is always 8x8
  // From Page 119 this can be 8 or 16 bits
  // For now this is fine
  for (int i = 0; i < 8; i++) {
    for (int j = 0; j < 8; j++) {
      current_character = getc(fp);
      log_verbose(current_character);
      m->data[current_position++] = current_character;
      m->dqt.table[i][j] = current_character;
      length--;
    }
  }

  if (length != 0) log_status(0, "deocode_unpack_DQT ran into an error");

  return 0;
}

int marker_unpack_SOF(struct marker *m, FILE *fp) {
  int current_character = 0;
  int length = m->length - 2;
  int current_position = 2;

  // This first part should always be a length of 8
  for (int i = 0; i < length; i++) {
    current_character = getc(fp);
    log_verbose(current_character);
    m->data[current_position++] = current_character;

    switch (i) {
      case 0: m->sof.precision = current_character; break;
      case 1: m->sof.lines = current_character << 8; break;
      case 2: m->sof.lines |= current_character; break;
      case 3: m->sof.samples = current_character << 8; break;
      case 4: m->sof.samples |= current_character; break;
      case 5: {
        m->sof.components = current_character;
        m->sof.factor_table = malloc(sizeof(int) * m->sof.components * 3);
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
  int current_character = 0;
  int length = m->length - 2;
  int current_position = 2;

  for (int i = 0; i < length; i++) {
    current_character = getc(fp);
    log_verbose(current_character);
    m->data[current_position++] = current_character;
  }
  length = 0;

  m->dht.number_of_tables = 0;
  while (length > 0) {
    current_character = getc(fp);
    log_verbose(current_character);
    m->data[current_position++] = current_character;

    m->dht.class = marker_realloc(m->dht.class, sizeof(*m->dht.class), m->dht.number_of_tables);
    m->dht.class[m->dht.number_of_tables] = current_character >> 4; // (current_character >> 4) & 0x0F
    if (m->dht.class[m->dht.number_of_tables] > 3) {
      log_status(1, "Error in DHT class");
      exit(1);
    }

    m->dht.destination = marker_realloc(m->dht.destination, sizeof(*m->dht.destination), m->dht.number_of_tables);
    m->dht.destination[m->dht.number_of_tables] = current_character & 0x0F;

    length--;

    m->dht.number_of_bytes = marker_realloc(m->dht.number_of_bytes, sizeof(*m->dht.number_of_bytes), m->dht.number_of_tables);

    m->dht.bytes = marker_realloc(m->dht.bytes, sizeof(*m->dht.bytes), m->dht.number_of_tables);
    m->dht.bytes[m->dht.number_of_tables] = malloc(sizeof(*m->dht.number_of_bytes));

    for (int i = 0; i < sizeof(*m->dht.number_of_bytes) / sizeof(*m->dht.number_of_bytes[0]); i++) {
      current_character = getc(fp);
      log_verbose(current_character);
      m->data[current_position++] = current_character;
      m->dht.number_of_bytes[m->dht.number_of_tables][i] = current_character;
      m->dht.bytes[m->dht.number_of_tables][i] = malloc(m->dht.number_of_bytes[m->dht.number_of_tables][i]);
      length--;
    }

    for (int i = 0; i < sizeof(*m->dht.number_of_bytes) / sizeof(*m->dht.number_of_bytes[0]); i++) {
      if (m->dht.number_of_bytes[m->dht.number_of_tables][i] == 0) continue;

      for (int j = 0; j < m->dht.number_of_bytes[m->dht.number_of_tables][i]; j++) {
        current_character = getc(fp);
      log_verbose(current_character);
      m->data[current_position++] = current_character;
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
  int current_character = 0;
  int length = m->length - 2;
  int current_position = 2;

  current_character = getc(fp);
  log_verbose(current_character);
  m->data[current_position++] = current_character;
  m->sos.components = current_character;
  m->sos.component_selector = malloc(sizeof(int) * m->sos.components);
  m->sos.dc_table_selector = malloc(sizeof(int) * m->sos.components);
  m->sos.ac_table_selector = malloc(sizeof(int) * m->sos.components);
  length--;

  for (int i = 0; i < m->sos.components; i++) {
    current_character = getc(fp);
    log_verbose(current_character);
    m->data[current_position++] = current_character;
    m->sos.component_selector[i] = current_character;

    current_character = getc(fp);
    log_verbose(current_character);
    m->data[current_position++] = current_character;
    m->sos.dc_table_selector[i] = current_character >> 4;
    m->sos.ac_table_selector[i] = current_character & 0x0F;

    length -= 2;
  }

  current_character = getc(fp);
  log_verbose(current_character);
  m->data[current_position++] = current_character;
  m->sos.spectral_select_start = current_character;
  length--;

  current_character = getc(fp);
  log_verbose(current_character);
  m->data[current_position++] = current_character;
  m->sos.spectral_select_end = current_character;
  length--;

  current_character = getc(fp);
  log_verbose(current_character);
  m->data[current_position++] = current_character;
  m->sos.successive_high = current_character >> 4;
  m->sos.successive_low = current_character & 0x0F;
  length--;

  if (length != 0) log_status(0, "deocode_unpack_SOS ran into an error");

  return 0;
}