/**
 * @file task_programmer01.h
 * ESP-IDF task programmer 
 * 
   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
 */
#ifndef __TASK_PROGRAMMER01_H__
#define __TASK_PROGRAMMER01_H__


#ifdef __cplusplus
extern "C" {
#endif

#include <time.h>

struct pattern_weekly{
  int PW_ID;
  int day;
  int PD_ID;
};

struct pattern_daily{
  int PD_ID;
  int PD_ID2;
  int hour;
  int minute;
  int target_var_ID;  
};

// Consider UNION to cope with float variable types
struct target_var{
  int target_var_ID;
  int target_var_value;
};

struct pattern_pgm{
  int day;
  int hour;
  int minute;
  int target_var_value;
};


int tp_init_structures();
int tp_activate_pattern(int weekly_pattern);
int activate_pattern(int pattern_weekly, struct pattern_pgm *active_pattern[50]);    // loads selected pattern to active pattern table
int get_target_value(time_t actual_time, int weekly_pgm_id, int override_temp_value, int target_value);

// TODO: add functions to add, update and delete new records:
//       Predefined records cannot be deleted, only some values can be modified.

// int reset_default_patterns();                                    // resets default patterns
// int add_target_temp(struct target_var target_temp);             // (exactly 1 record)
// int add_pattern_daily(struct pattern_daily[6], int records);    // (group, Maximum 6 transitions x day)
// int add_pattern_weekly(struct pattern_weekly[7]);               // (group. Exactly 7 records (days))


#ifdef __cplusplus
}
#endif

#endif  // __TASK_PROGRAMMER01_H__
