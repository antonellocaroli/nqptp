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

#ifndef NQPTP_CLOCK_SOURCES_H
#define NQPTP_CLOCK_SOURCES_H

#include "nqptp.h"

// transaction tracking
enum stage { waiting_for_sync, sync_seen, follow_up_seen };

typedef enum {
  clock_is_valid,
  clock_is_a_timing_peer,
  clock_is_qualified,
  clock_is_becoming_master,
  clock_is_master
} clock_flags;

// 8 samples per seconds
#define MAX_TIMING_SAMPLES 47
typedef struct {
  uint64_t local_time, clock_time;
} timing_samples;

// information about each clock source
typedef struct {
  char ip[64]; // 64 is nicely aligned and bigger than INET6_ADDRSTRLEN (46)
  uint64_t clock_id;
  uint64_t local_time; // the local time when the offset was calculated
  uint64_t source_time;
  uint64_t local_to_source_time_offset; // add this to the local time to get source time
  uint32_t flags;
  uint16_t in_use;
  uint64_t previous_offset, previous_offset_time, last_sync_time;
  uint64_t mastership_start_time; // set to the time of the first sample used as master

  // for garbage collection
  uint64_t time_of_last_use; // will be taken out of use if not used for a while and not in the
                             // timing peer group
  // (A member of the timing peer group could appear and disappear so will not be gc'ed.)
  // for Announce Qualification
  uint64_t announce_times[4]; // we'll check qualification and currency using these
  int is_one_of_ours;         // true if it is one of our own clocks

  timing_samples samples[MAX_TIMING_SAMPLES];
  int vacant_samples; // the number of elements in the timing_samples array that are not yet used
  int next_sample_goes_here; // point to where in the timing samples array the next entries should
                             // go

  // these are for finding the best clock to use
  // See Figure 27 and 27 pp 89 -- 90 for the Data set comparison algorithm

  uint8_t grandmasterPriority1;
  uint32_t grandmasterQuality; // class/accuracy/variance -- lower is better
  uint8_t grandmasterClass;
  uint8_t grandmasterAccuracy;
  uint16_t grandmasterVariance;
  uint8_t grandmasterPriority2;
  uint64_t grandmasterIdentity;
  uint16_t stepsRemoved;

} clock_source_private_data;

int find_clock_source_record(char *sender_string, clock_source_private_data *clocks_private_info);

int create_clock_source_record(char *sender_string, clock_source_private_data *clocks_private_info);

void update_clock_self_identifications(clock_source_private_data *clocks_private_info);

void manage_clock_sources(uint64_t reception_time, clock_source_private_data *clocks_private_info);

extern clock_source_private_data clocks_private[MAX_CLOCKS];

void update_master();

void debug_log_nqptp_status(int level);

#endif
