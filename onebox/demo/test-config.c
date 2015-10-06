#include "onebox-common.h"
#include "util-conf-node.h"
#include "test-config.h"

int ReadConfigTest(void)
{
#if 0
default-log-dir: /opt/suricata/var/log/suricata/

detect-engine:
  - profile: medium
  - custom-values:
      toclient-src-groups: 2
      toclient-dst-groups: 2
      toclient-sp-groups: 2
      toclient-dp-groups: 3
      toserver-src-groups: 2
      toserver-dst-groups: 4
      toserver-sp-groups: 2
      toserver-dp-groups: 25
  - sgh-mpm-context: auto
  - inspection-recursion-limit: 3000

threading:
  set-cpu-affinity: no
  cpu-affinity:
    - management-cpu-set:
        cpu: [ 0 ]  # include only these cpus in affinity settings
    - receive-cpu-set:
        cpu: [ 0 ]  # include only these cpus in affinity settings
    - decode-cpu-set:
        cpu: [ 0, 1 ]
        mode: "balanced"
    - stream-cpu-set:
        cpu: [ "0-1" ]
    - detect-cpu-set:
        cpu: [ "all" ]
        mode: "exclusive" # run detect threads in these cpus

        prio:
          low: [ 0 ]
          medium: [ "1-2" ]
          high: [ 3 ]
          default: "medium"
    - verdict-cpu-set:
        cpu: [ 0 ]
        prio:
          default: "high"
    - reject-cpu-set:
        cpu: [ 0 ]
        prio:
          default: "low"
    - output-cpu-set:
        cpu: [ "all" ]
        prio:
           default: "medium"

  detect-thread-ratio: 1.5

defrag:
  memcap: 32mb
  hash-size: 65536
  trackers: 65535 # number of defragmented flows to follow
  max-frags: 65535 # number of fragments to keep (higher than trackers)
  prealloc: yes
  timeout: 60

flow:
  memcap: 64mb
  hash-size: 65536
  prealloc: 10000
  emergency-recovery: 30

vlan:
  use-for-tracking: true

flow-timeouts:

  default:
    new: 30
    established: 300
    closed: 0
    emergency-new: 10
    emergency-established: 100
    emergency-closed: 0
  tcp:
    new: 60
    established: 3600
    closed: 120
    emergency-new: 10
    emergency-established: 300
    emergency-closed: 20
  udp:
    new: 30
    established: 300
    emergency-new: 10
    emergency-established: 100
  icmp:
    new: 30
    established: 300
    emergency-new: 10
    emergency-established: 100


stream:
  memcap: 128mb
  checksum-validation: yes      # reject wrong csums
  inline: auto                  # auto will use inline mode in IPS mode, yes or no set it statically
  reassembly:
    memcap: 128mb
    depth: 32mb                  # reassemble 1mb into a stream
    toserver-chunk-size: 2560
    toclient-chunk-size: 2560
    randomize-chunk-size: yes

# Set the order of alerts bassed on actions
# The default order is pass, drop, reject, alert
action-order:
  - pass
  - drop
  - reject
  - alert

app-layer:
  protocols:
    tls:
      enabled: yes
      detection-ports:
        dp: 443

      #no-reassemble: yes
    dcerpc:
      enabled: yes
    ftp:
      enabled: yes
    ssh:
      enabled: yes
    smtp:
      enabled: yes
    imap:
      enabled: detection-only
    msn:
      enabled: detection-only
    smb:
      enabled: yes
      detection-ports:
        dp: 139

    dns:

      tcp:
        enabled: yes
        detection-ports:
          dp: 53
      udp:
        enabled: yes
        detection-ports:
          dp: 53
    http:
      enabled: yes
      # memcap: 64mb

#endif
	intmax_t frag_pool_size = 0;
	int randomize = 0;
	char *default_log_dir=NULL;

	if (!ConfGetInt("defrag.max-frags", &frag_pool_size)){
		printf(">>>> get defrag.max-frags error\r\n");
	}else{
		printf(">>>> defrag.max-frags is %ld\r\n", frag_pool_size);
	}

	if ( ConfGetBool("stream.reassembly.randomize-chunk-size", &randomize) == 0 ){
		printf(">>>> get stream.reassembly.randomize-chunk-size error\r\n");
	}else{
		printf(">>>> stream.reassembly.randomize-chunk-size is %d\r\n", randomize);
	}

	if (!ConfGet("default-log-dir", &default_log_dir)){
		printf(">>>> get default-log-dir error\r\n");
	}else{
		printf("default-log-dir is %s\r\n", default_log_dir);
	}
	return 0;
}
