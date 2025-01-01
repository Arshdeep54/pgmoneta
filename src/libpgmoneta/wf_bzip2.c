/*
 * Copyright (C) 2025 The pgmoneta community
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this list
 * of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice, this
 * list of conditions and the following disclaimer in the documentation and/or other
 * materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its contributors may
 * be used to endorse or promote products derived from this software without specific
 * prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL
 * THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
 * OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR
 * TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/* pgmoneta */
#include <pgmoneta.h>
#include <bzip2_compression.h>
#include <deque.h>
#include <logging.h>
#include <utils.h>
#include <workers.h>
#include <workflow.h>

/* system */
#include <stdlib.h>
#include <string.h>

static int bzip2_setup(int, char*, struct deque*);
static int bzip2_execute_compress(int, char*, struct deque*);
static int bzip2_execute_uncompress(int, char*, struct deque*);
static int bzip2_teardown(int, char*, struct deque*);

struct workflow*
pgmoneta_create_bzip2(bool compress)
{
   struct workflow* wf = NULL;

   wf = (struct workflow*)malloc(sizeof(struct workflow));

   if (wf == NULL)
   {
      return NULL;
   }

   wf->setup = &bzip2_setup;

   if (compress == true)
   {
      wf->execute = &bzip2_execute_compress;
   }
   else
   {
      wf->execute = &bzip2_execute_uncompress;
   }

   wf->teardown = &bzip2_teardown;
   wf->next = NULL;

   return wf;
}

static int
bzip2_setup(int server, char* identifier, struct deque* nodes)
{
   struct configuration* config;

   config = (struct configuration*)shmem;

   pgmoneta_log_debug("BZip2 (setup): %s/%s", config->servers[server].name, identifier);
   pgmoneta_deque_list(nodes);

   return 0;
}

static int
bzip2_execute_compress(int server, char* identifier, struct deque* nodes)
{
   char* d = NULL;
   char* backup_base = NULL;
   char* backup_data = NULL;
   char* tarfile = NULL;
   time_t compression_time;
   int total_seconds;
   int hours;
   int minutes;
   int seconds;
   char elapsed[128];
   int number_of_workers = 0;
   struct workers* workers = NULL;
   struct configuration* config;

   config = (struct configuration*)shmem;

   pgmoneta_log_debug("BZip2 (compress): %s/%s", config->servers[server].name, identifier);
   pgmoneta_deque_list(nodes);

   compression_time = time(NULL);

   tarfile = (char*)pgmoneta_deque_get(nodes, NODE_TARFILE);

   if (tarfile == NULL)
   {
      number_of_workers = pgmoneta_get_number_of_workers(server);
      if (number_of_workers > 0)
      {
         pgmoneta_workers_initialize(number_of_workers, &workers);
      }

      backup_base = (char*)pgmoneta_deque_get(nodes, NODE_BACKUP_BASE);
      backup_data = (char*)pgmoneta_deque_get(nodes, NODE_BACKUP_DATA);

      pgmoneta_bzip2_data(backup_data, workers);
      pgmoneta_bzip2_tablespaces(backup_base, workers);

      if (number_of_workers > 0)
      {
         pgmoneta_workers_wait(workers);
         pgmoneta_workers_destroy(workers);
      }
   }
   else
   {
      d = pgmoneta_append(d, tarfile);
      d = pgmoneta_append(d, ".bz2");

      if (pgmoneta_exists(d))
      {
         pgmoneta_delete_file(d, true, NULL);
      }

      pgmoneta_bzip2_file(tarfile, d);
   }

   total_seconds = (int)difftime(time(NULL), compression_time);
   hours = total_seconds / 3600;
   minutes = (total_seconds % 3600) / 60;
   seconds = total_seconds % 60;

   memset(&elapsed[0], 0, sizeof(elapsed));
   sprintf(&elapsed[0], "%02i:%02i:%02i", hours, minutes, seconds);

   pgmoneta_log_debug("Compression: %s/%s (Elapsed: %s)", config->servers[server].name, identifier, &elapsed[0]);

   free(d);

   return 0;
}

static int
bzip2_execute_uncompress(int server, char* identifier, struct deque* nodes)
{
   char* base = NULL;
   time_t decompress_time;
   int total_seconds;
   int hours;
   int minutes;
   int seconds;
   char elapsed[128];
   int number_of_workers = 0;
   struct workers* workers = NULL;
   struct configuration* config;

   config = (struct configuration*)shmem;

   pgmoneta_log_debug("BZip2 (uncompress): %s/%s", config->servers[server].name, identifier);
   pgmoneta_deque_list(nodes);

   base = (char*)pgmoneta_deque_get(nodes, NODE_DESTINATION);
   if (base == NULL)
   {
      base = (char*)pgmoneta_deque_get(nodes, NODE_BACKUP_BASE);
   }
   if (base == NULL)
   {
      base = (char*)pgmoneta_deque_get(nodes, NODE_BACKUP_DATA);
   }

   decompress_time = time(NULL);

   number_of_workers = pgmoneta_get_number_of_workers(server);
   if (number_of_workers > 0)
   {
      pgmoneta_workers_initialize(number_of_workers, &workers);
   }

   pgmoneta_bunzip2_data(base, workers);

   if (number_of_workers > 0)
   {
      pgmoneta_workers_wait(workers);
      pgmoneta_workers_destroy(workers);
   }

   total_seconds = (int)difftime(time(NULL), decompress_time);
   hours = total_seconds / 3600;
   minutes = (total_seconds % 3600) / 60;
   seconds = total_seconds % 60;

   memset(&elapsed[0], 0, sizeof(elapsed));
   sprintf(&elapsed[0], "%02i:%02i:%02i", hours, minutes, seconds);

   pgmoneta_log_debug("Decompress: %s/%s (Elapsed: %s)", config->servers[server].name, identifier, &elapsed[0]);

   return 0;
}

static int
bzip2_teardown(int server, char* identifier, struct deque* nodes)
{
   struct configuration* config;

   config = (struct configuration*)shmem;

   pgmoneta_log_debug("BZip2 (teardown): %s/%s", config->servers[server].name, identifier);
   pgmoneta_deque_list(nodes);

   return 0;
}
