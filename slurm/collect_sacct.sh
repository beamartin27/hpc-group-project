#!/bin/bash
JOBID="$1"
if [ -z "$JOBID" ]; then
  echo "Usage: $0 <JOBID>"
  exit 1
fi

sacct -j "$JOBID" \
  --format=JobID,JobName%20,State,Elapsed,TotalCPU,AllocNodes,AllocCPUS \
  --parsable2 >> results/sacct_log.csv
