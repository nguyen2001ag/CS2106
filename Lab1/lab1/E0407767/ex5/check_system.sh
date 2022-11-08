#!/bin/bash

####################
# Lab 1 Exercise 5
# Name: Nguyen Minh Nguyen
# Student No: A0200786E
# Lab Group: B07
####################

# Fill the below up
hostname=$(uname -n)
machine_hardware=$(uname -sm)
# max_user_process_count= $(ulimit -u)
max_user_process_count=$(cat /proc/sys/kernel/pid_max)
user_process_count=$(ps -u $(whoami) --no-headers | wc -l)
user_with_most_processes=$(ps -eo user | sort | uniq -c | sort -n | tail -1 |awk '{print $2}')
mem_free_percentage=$(free | grep Mem | awk '{print $4/$2 * 100.0}')

echo "Hostname: $hostname"
echo "Machine Hardware: $machine_hardware"
echo "Max User Processes: $max_user_process_count"
echo "User Processes: $user_process_count"
echo "User With Most Processes: $user_with_most_processes"
echo "Memory Free (%): $mem_free_percentage"

## Ask if User processes refer to us, we the user
## Ask if hostname is different from username
