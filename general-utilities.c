/*
 * This file is part of the nqptp distribution (https://github.com/mikebrady/nqptp).
 * Copyright (c) 2021 Mike Brady.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, version 2.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 *
 * Commercial licensing is also available.
 */

#include "general-utilities.h"
#include "debug.h"
#include <arpa/inet.h> // ntohl etc.
#include <string.h>    // memcpy

void hcton64(uint64_t num, uint8_t *p) {
  uint64_t numc = num;
  numc = numc >> 32;
  uint32_t num_32 = numc;
  uint32_t rev = htonl(num_32);
  memcpy(p, &rev, sizeof(uint32_t));
  num_32 = num & 0xffffffff;
  p = p + 4;
  rev = htonl(num_32);
  memcpy(p, &rev, sizeof(uint32_t));
}

uint32_t nctohl(const uint8_t *p) { // read 4 characters from *p and do ntohl on them
  // this is to avoid possible aliasing violations
  uint32_t holder;
  memcpy(&holder, p, sizeof(holder));
  return ntohl(holder);
}

uint16_t nctohs(const uint8_t *p) { // read 2 characters from *p and do ntohs on them
  // this is to avoid possible aliasing violations
  uint16_t holder;
  memcpy(&holder, p, sizeof(holder));
  return ntohs(holder);
}

uint64_t timespec_to_ns(struct timespec *tn) {
  uint64_t tnfpsec = tn->tv_sec;
  uint64_t tnfpnsec = tn->tv_nsec;
  tnfpsec = tnfpsec * 1000000000;
  return tnfpsec + tnfpnsec;
}

uint64_t get_time_now() {
  struct timespec tn;
  clock_gettime(CLOCK_REALTIME, &tn); // this should be optionally CLOCK_MONOTONIC etc.
  return timespec_to_ns(&tn);
}
