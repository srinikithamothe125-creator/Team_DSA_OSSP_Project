# LinuxPulse — Linux / WSL Resource Monitor (Option 2)

This version monitors the Linux environment running inside Ubuntu/WSL2.
It intentionally does not read Windows-wide hardware/resource values.

## Live information
- Hostname and Linux kernel release (`uname()`)
- CPU model (`/proc/cpuinfo`)
- CPU architecture and online logical CPU threads (`uname`, `sysconf`)
- CPU utilization sampled from `/proc/stat`
- RAM total and available (`/proc/meminfo`)
- Root Linux filesystem capacity and available space (`statvfs("/")`)
- Linux process count (`/proc`)
- Uptime (`/proc/uptime`)
- Load average (`/proc/loadavg`)

## Build and run in Ubuntu
From this folder:

```bash
gcc -Wall -Wextra -O2 -o linuxpulse web_server.c
./linuxpulse
```

Open `http://localhost:8080`. The dashboard refreshes every 5 seconds.
Stop with Ctrl+C.

## Important WSL2 note
These are genuine values reported by the Linux kernel/environment inside WSL2.
The CPU model may identify the physical processor, but CPU usage, RAM limits,
processes, uptime, and `/` disk capacity are the Linux/WSL view and can differ
from Windows Task Manager or the full physical laptop totals.
