#!/bin/bash

# Initialize variables
selected_flag=""


while [[ "$#" -gt 0 ]]; do
  case $1 in
    -2022)
      if [[ -n "$selected_flag" ]]; then
        echo "Error: Only one flag can be selected." >&2
        exit 1
      fi
      selected_flag="-2022"
      ./premake5 vs2022
      ;;
    -2019)
      if [[ -n "$selected_flag" ]]; then
        echo "Error: Only one flag can be selected." >&2
        exit 1
      fi
      selected_flag="-2019"
      ./premake5 vs2019
      ;;
    *)
      echo "Unknown option: $1" >&2
      exit 1
      ;;
  esac
  shift
done


if [[ -z "$selected_flag" ]]; then
  echo "Error: No flag selected." >&2
  exit 1
fi


echo "Selected flag: $selected_flag"
