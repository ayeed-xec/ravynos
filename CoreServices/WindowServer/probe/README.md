# Mach-IPC ping/pong probe (WP-B1)

Status: sources compile against the real xnu-12377.121.6 Mach headers
(`clang -fsyntax-only -Wall -Werror`, strict-clean). On-device run is
gated on the guest kernel booting (WP-A2).

## Design

The guest has no launchd. init sets the task-special bootstrap port
(`TASK_BOOTSTRAP_PORT`) before starting user land.

- Server (`probe_server.c`): creates a receive right, advertises it in
  the bootstrap namespace (see probe_message.h: WINDOWSERVER_SVC_NAME),
  then loops on `mach_msg` receive. Every `MSG_ID_INLINE` ping becomes a
  pong (CODE_PONG) sent back on the caller's send right.
- Client (`probe_client.c`): looks the service up via the bootstrap
  port, sends 4 pings, counts pongs; exit 0 only if all 4 round-trip.
- Shared helpers (`probe_common.c`) and the message layout
  (`probe_message.h`) live beside them; the message struct follows the
  WindowServer contract style in ../message.h (mach_msg_header_t +
  inline payload), ids MSG_ID_INLINE (90211) reserved for the probe.

## Verification recipe (host)

S=<clone>/Kernel/build/mig/shim
X=<clone>/Kernel/xnu
G=<clone>/Kernel/build/mig/gen
clang -fsyntax-only -Wall -Werror -Wno-macro-redefined -std=gnu17 \
  -include $S/mach_decl_shim.h -include $S/sys/_types/_printflike.h \
  -include $S/sys/_types/_unused.h \
  -I. -I$S -I$G -I$X/osfmk -I$X/libkern -I$X/libsyscall/mach \
  -I$X/libsyscall/mach/mach probe_{common,server,client}.c

The include shim and generated headers come from the mig harness
(Kernel/build/mig). The generated mach/*.h staged in shim/mach are
mig output (clock, exc, host_priv, host_security, mach_exc, mach_host,
mach_port, notify, processor, processor_set, task, thread_act, vm_map,
semaphore, ledger families) — regenerate with Kernel/build/mig/build.sh
+ mig, never hand-edit.
