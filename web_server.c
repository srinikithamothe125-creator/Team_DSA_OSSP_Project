#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/utsname.h>
#include <sys/statvfs.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <signal.h>
#include <time.h>

#define PORT 8080
#define PAGE_SIZE 24000

typedef struct {
    char hostname[128], kernel[128], uptime[80], cpu_name[128], cpu_model[160], load[128];
    long ram_total, ram_used;
    long disk_total, disk_used;
    int cpu_percent, cores, processes;
} SystemInfo;

static void read_system(SystemInfo *s) {
    memset(s, 0, sizeof(*s));
    struct utsname u;
    if (uname(&u) == 0) {
        snprintf(s->hostname, sizeof s->hostname, "%s", u.nodename);
        snprintf(s->kernel, sizeof s->kernel, "%s", u.release);
        snprintf(s->cpu_name, sizeof s->cpu_name, "%s", u.machine);
    } else {
        strcpy(s->hostname, "Linux host");
        strcpy(s->kernel, "Unavailable");
        strcpy(s->cpu_name, "Unknown");
    }

    strcpy(s->cpu_model, "Unknown Linux CPU");
    FILE *cpuinfo = fopen("/proc/cpuinfo", "r");
    if (cpuinfo) {
        char line[512];
        while (fgets(line, sizeof line, cpuinfo)) {
            if (strncmp(line, "model name", 10) == 0) {
                char *colon = strchr(line, ':');
                if (colon) {
                    colon++;
                    while (*colon == ' ' || *colon == '\\t') colon++;
                    colon[strcspn(colon, "\\r\\n")] = 0;
                    snprintf(s->cpu_model, sizeof s->cpu_model, "%s", colon);
                }
                break;
            }
        }
        fclose(cpuinfo);
    }

    FILE *f = fopen("/proc/uptime", "r");
    double seconds = 0;
    if (f) { fscanf(f, "%lf", &seconds); fclose(f); }
    int days = (int)(seconds / 86400);
    int hours = ((int)(seconds / 3600)) % 24;
    int mins = ((int)(seconds / 60)) % 60;
    snprintf(s->uptime, sizeof s->uptime, "%d days, %02d hours, %02d min", days, hours, mins);

    long total = 0, available = 0;
    f = fopen("/proc/meminfo", "r");
    if (f) {
        char line[256];
        while (fgets(line, sizeof line, f)) {
            long value;
            if (sscanf(line, "MemTotal: %ld", &value) == 1) total = value;
            else if (sscanf(line, "MemAvailable: %ld", &value) == 1) available = value;
        }
        fclose(f);
    }
    s->ram_total = total / 1024;
    s->ram_used = (total - available) / 1024;

    struct statvfs v;
    if (statvfs("/", &v) == 0) {
        unsigned long long all = (unsigned long long)v.f_blocks * v.f_frsize;
        unsigned long long freeb = (unsigned long long)v.f_bavail * v.f_frsize;
        s->disk_total = (long)(all / (1024ULL * 1024ULL * 1024ULL));
        s->disk_used = (long)((all - freeb) / (1024ULL * 1024ULL * 1024ULL));
    }

    f = fopen("/proc/loadavg", "r");
    if (f) { fgets(s->load, sizeof s->load, f); fclose(f); s->load[strcspn(s->load, "\n")] = 0; }

    s->cores = (int)sysconf(_SC_NPROCESSORS_ONLN);
    DIR *d = opendir("/proc");
    if (d) {
        struct dirent *e;
        while ((e = readdir(d)) != NULL) {
            if (e->d_type == DT_DIR && e->d_name[0] >= '0' && e->d_name[0] <= '9')
                s->processes++;
        }
        closedir(d);
    }

    unsigned long long a1=0,b1=0,a2=0,b2=0;
    unsigned long long user,nice,system,idle,iowait,irq,softirq,steal;
    f = fopen("/proc/stat", "r");
    if (f) {
        fscanf(f, "cpu %llu %llu %llu %llu %llu %llu %llu %llu",
               &user,&nice,&system,&idle,&iowait,&irq,&softirq,&steal);
        a1 = user+nice+system+idle+iowait+irq+softirq+steal;
        b1 = idle+iowait;
        fclose(f);
    }
    usleep(120000);
    f = fopen("/proc/stat", "r");
    if (f) {
        fscanf(f, "cpu %llu %llu %llu %llu %llu %llu %llu %llu",
               &user,&nice,&system,&idle,&iowait,&irq,&softirq,&steal);
        a2 = user+nice+system+idle+iowait+irq+softirq+steal;
        b2 = idle+iowait;
        fclose(f);
    }
    if (a2 > a1 && b2 >= b1) {
        unsigned long long total_delta = a2-a1;
        unsigned long long idle_delta = b2-b1;
        s->cpu_percent = (int)(100.0 * (double)(total_delta-idle_delta) / (double)total_delta);
    }
    if (s->cpu_percent < 0) s->cpu_percent = 0;
    if (s->cpu_percent > 100) s->cpu_percent = 100;
}

static int clamp_percent(long used, long total) {
    if (total <= 0) return 0;
    int p = (int)(100.0 * (double)used / (double)total);
    if (p < 0) return 0;
    if (p > 100) return 100;
    return p;
}

static void send_page(int client) {
    SystemInfo s;
    read_system(&s);
    int ram_pct = clamp_percent(s.ram_used, s.ram_total);
    int disk_pct = clamp_percent(s.disk_used, s.disk_total);
    char page[PAGE_SIZE];
    int n = snprintf(page, sizeof page,
"<!doctype html><html lang='en'><head><meta charset='utf-8'>"
"<meta name='viewport' content='width=device-width,initial-scale=1'>"
"<meta http-equiv='refresh' content='5'><title>LinuxPulse | Resource Monitor</title>"
"<style>"
"@import url('https://fonts.googleapis.com/css2?family=DM+Mono:wght@400;500&family=Manrope:wght@400;500;600;700;800&display=swap');"
":root{--bg:#080e16;--panel:#101b29;--panel2:#0c1521;--line:#203044;--muted:#8192a8;--text:#edf4fb;--green:#5de0b2;--blue:#78aefb;--purple:#b69aff}"
"*{box-sizing:border-box}body{margin:0;background:radial-gradient(ellipse at 75%% -20%%,#142a38 0,transparent 38%%),var(--bg);color:var(--text);font-family:Manrope,Arial,sans-serif;min-height:100vh}"
".wrap{max-width:1440px;margin:auto;padding:26px 34px 20px}.top{display:flex;align-items:center;justify-content:space-between;margin-bottom:22px}"
".brand{font-size:23px;font-weight:800;letter-spacing:-1.2px}.brand span{color:var(--green)}.tag{font:10px 'DM Mono';letter-spacing:2px;color:var(--muted);margin-top:4px}"
".status{font:11px 'DM Mono';color:var(--green);border:1px solid #245b4c;background:#10271f;border-radius:30px;padding:9px 13px}.dot{display:inline-block;width:7px;height:7px;background:var(--green);border-radius:50%%;margin-right:8px;box-shadow:0 0 12px var(--green)}"
".heading{display:flex;justify-content:space-between;align-items:end;margin:10px 0 18px}.heading h1{font-size:27px;letter-spacing:-1.2px;margin:0 0 5px}.heading p{font-size:12px;color:var(--muted);margin:0}.updated{font:10px 'DM Mono';color:var(--muted)}"
".stats{display:grid;grid-template-columns:repeat(4,1fr);gap:13px}.card{background:linear-gradient(145deg,#121f2e,#0e1825);border:1px solid var(--line);border-radius:13px;padding:17px 18px;min-width:0}.label{font:10px 'DM Mono';letter-spacing:1.4px;color:var(--muted);text-transform:uppercase}.value{font-size:20px;font-weight:800;letter-spacing:-.6px;margin:10px 0 4px;white-space:nowrap;overflow:hidden;text-overflow:ellipsis}.hint{font-size:10px;color:var(--green)}"
".middle{display:grid;grid-template-columns:1.25fr .95fr;gap:14px;margin-top:14px}.section-title{font-size:14px;font-weight:700;margin:0 0 13px}.resource{margin:13px 0 17px}.reshead{display:flex;justify-content:space-between;align-items:center;font-size:12px;margin-bottom:8px}.reshead span:last-child{font:11px 'DM Mono';color:#c3d0df}.track{height:8px;border-radius:20px;background:#202d3d;overflow:hidden}.bar{height:100%%;border-radius:20px;background:linear-gradient(90deg,#3abf99,#79e5c0)}.bar.cpu{background:linear-gradient(90deg,#5b9df4,#8bc3ff)}.bar.disk{background:linear-gradient(90deg,#9878ee,#c2a9ff)}"
".details{display:grid;grid-template-columns:1fr 1fr;gap:10px}.detail{background:var(--panel2);border:1px solid #1c2b3c;border-radius:10px;padding:12px}.detail small{display:block;font:9px 'DM Mono';color:var(--muted);letter-spacing:1px;margin-bottom:7px}.detail strong{font-size:12px;font-weight:600;overflow-wrap:anywhere}"
".bottom{display:grid;grid-template-columns:1fr 1fr 1fr;gap:13px;margin-top:14px}.mini{display:flex;align-items:center;justify-content:space-between}.mini strong{font-size:21px}.mini span{font:10px 'DM Mono';color:var(--muted)}"
".footer{display:flex;justify-content:space-between;border-top:1px solid #1b2939;margin-top:16px;padding-top:12px;color:#61748b;font:9px 'DM Mono';letter-spacing:1px}"
"@media(max-width:900px){.wrap{padding:22px}.stats{grid-template-columns:repeat(2,1fr)}.middle{grid-template-columns:1fr}.bottom{grid-template-columns:1fr 1fr}}"
"@media(max-width:560px){.wrap{padding:17px}.stats{grid-template-columns:1fr 1fr;gap:8px}.card{padding:13px}.value{font-size:16px}.heading{align-items:flex-start;flex-direction:column;gap:8px}.heading h1{font-size:23px}.bottom{grid-template-columns:1fr}.brand{font-size:20px}}"
"</style></head><body><main class='wrap'>"
"<header class='top'><div><div class='brand'>Linux<span>Pulse</span></div><div class='tag'>LINUX / WSL SYSTEM INTELLIGENCE</div></div><div class='status'><i class='dot'></i>MONITOR ACTIVE</div></header>"
"<section class='heading'><div><h1>System Overview</h1><p>Real-time visibility into your Linux machine and its resources.</p></div><div class='updated'>AUTO-REFRESH · EVERY 5 SECONDS</div></section>"
"<section class='stats'>"
"<article class='card'><div class='label'>Hostname</div><div class='value'>%s</div><div class='hint'>● Detected Linux environment</div></article>"
"<article class='card'><div class='label'>Kernel release</div><div class='value' style='font-size:15px'>%s</div><div class='hint'>Linux kernel release</div></article>"
"<article class='card'><div class='label'>System uptime</div><div class='value' style='font-size:17px'>%s</div><div class='hint'>Time since last boot</div></article>"
"<article class='card'><div class='label'>CPU architecture</div><div class='value'>%s</div><div class='hint'>%d logical CPU threads</div></article>"
"</section>"
"<section class='middle'><article class='card'><h2 class='section-title'>Resource utilization</h2>"
"<div class='resource'><div class='reshead'><span>CPU usage</span><span>%d%%</span></div><div class='track'><div class='bar cpu' style='width:%d%%'></div></div></div>"
"<div class='resource'><div class='reshead'><span>Memory usage</span><span>%ld MB / %ld MB · %d%%</span></div><div class='track'><div class='bar' style='width:%d%%'></div></div></div>"
"<div class='resource' style='margin-bottom:2px'><div class='reshead'><span>Root disk usage</span><span>%ld GB / %ld GB · %d%%</span></div><div class='track'><div class='bar disk' style='width:%d%%'></div></div></div>"
"</article><article class='card'><h2 class='section-title'>System details</h2><div class='details'>"
"<div class='detail'><small>CPU MODEL</small><strong>%s</strong></div>"
"<div class='detail'><small>RUNNING PROCESSES</small><strong>%d processes</strong></div>"
"<div class='detail'><small>LOGICAL CPU THREADS</small><strong>%d online</strong></div>"
"<div class='detail'><small>LOAD AVERAGE</small><strong>%s</strong></div>"
"<div class='detail'><small>PLATFORM</small><strong>Linux / Ubuntu</strong></div>"
"</div></article></section>"
"<section class='bottom'>"
"<article class='card mini'><div><div class='label'>CPU load</div><span>Current activity</span></div><strong>%d%%</strong></article>"
"<article class='card mini'><div><div class='label'>RAM available</div><span>Free memory estimate</span></div><strong>%ld MB</strong></article>"
"<article class='card mini'><div><div class='label'>Disk available</div><span>Root filesystem</span></div><strong>%ld GB</strong></article>"
"</section>"
"<footer class='footer'><span>LINUXPULSE · C-POWERED MONITOR</span><span>LIVE LINUX DATA · /proc · uname() · statvfs()</span></footer>"
"</main></body></html>",
s.hostname,s.kernel,s.uptime,s.cpu_name,s.cores,
s.cpu_percent,s.cpu_percent,
s.ram_used,s.ram_total,ram_pct,ram_pct,
s.disk_used,s.disk_total,disk_pct,disk_pct,
s.cpu_model,s.processes,s.cores,s.load,
s.cpu_percent,(s.ram_total-s.ram_used),(s.disk_total-s.disk_used));

    if (n < 0) return;
    char header[256];
    int h = snprintf(header, sizeof header,
        "HTTP/1.1 200 OK\r\nContent-Type: text/html; charset=utf-8\r\n"
        "Content-Length: %d\r\nConnection: close\r\n\r\n", n);
    write(client, header, (size_t)h);
    write(client, page, (size_t)n);
}

int main(void) {
    signal(SIGPIPE, SIG_IGN);
    int server = socket(AF_INET, SOCK_STREAM, 0);
    if (server < 0) { perror("socket"); return 1; }
    int yes = 1;
    setsockopt(server, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof yes);
    struct sockaddr_in addr = {0};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(PORT);
    if (bind(server, (struct sockaddr *)&addr, sizeof addr) < 0) {
        perror("bind"); close(server); return 1;
    }
    if (listen(server, 10) < 0) { perror("listen"); close(server); return 1; }
    printf("LinuxPulse is running at http://localhost:%d\\nPress Ctrl+C to stop.\\n", PORT);
    fflush(stdout);
    while (1) {
        int client = accept(server, NULL, NULL);
        if (client >= 0) { send_page(client); close(client); }
    }
    close(server);
    return 0;
}
