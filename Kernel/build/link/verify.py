import struct
import sys

MH_MAGIC_64 = 0xfeedfacf
CPU_TYPE_X86_64 = 0x01000007
MH_EXECUTE = 0x2
MH_NOUNDEFS = 0x1
MH_DYLDLINK = 0x4
MH_TWOLEVEL = 0x80
MH_NOMULTIDEFS = 0x200
MH_PIE = 0x200000

LC_UNIXTHREAD = 0x5
LC_SEGMENT_64 = 0x19
LC_DYSYMTAB = 0xB
LC_LOAD_DYLIB = 0xE
LC_LOAD_WEAK_DYLIB = 0x80000018
LC_REEXPORT_DYLIB = 0x8000001F
LC_CODE_SIGNATURE = 0x1D
LC_DYLD_INFO = 0x22
LC_DYLD_INFO_ONLY = 0x80000022
LC_FUNCTION_STARTS = 0x26
LC_DATA_IN_CODE = 0x29
LC_MAIN = 0x80000028

X86_THREAD_STATE64 = 4
X86_THREAD_STATE64_COUNT = 42

VM_PROT_WRITE = 2
VM_PROT_EXECUTE = 1

FORBIDDEN = {
    LC_MAIN: "LC_MAIN (must be LC_UNIXTHREAD)",
    LC_DYLD_INFO: "LC_DYLD_INFO",
    LC_DYLD_INFO_ONLY: "LC_DYLD_INFO_ONLY",
    LC_LOAD_DYLIB: "LC_LOAD_DYLIB",
    LC_LOAD_WEAK_DYLIB: "LC_LOAD_WEAK_DYLIB",
    LC_REEXPORT_DYLIB: "LC_REEXPORT_DYLIB",
    LC_CODE_SIGNATURE: "LC_CODE_SIGNATURE",
}

PAGE_SIZE = 0x1000


def fail(msg):
    print("verify: FAIL: %s" % msg)
    sys.exit(1)


def prot_str(prot):
    return "%s%s%s%s" % (
        "r" if prot & 4 else "-",
        "w" if prot & 2 else "-",
        "x" if prot & 1 else "-",
        "/",
    )


def main():
    if len(sys.argv) != 2:
        fail("usage: verify.py <macho>")
    path = sys.argv[1]
    try:
        with open(path, "rb") as f:
            data = f.read()
    except OSError as e:
        fail("cannot read %s: %s" % (path, e))

    if len(data) < 32:
        fail("file too small for Mach-O header")
    (magic, cputype, cpusubtype, filetype, ncmds, sizeofcmds, flags,
     reserved) = struct.unpack_from("<8I", data, 0)

    if magic != MH_MAGIC_64:
        fail("bad magic 0x%08x (want MH_MAGIC_64)" % magic)
    if cputype != CPU_TYPE_X86_64:
        fail("cputype 0x%08x (want x86_64)" % cputype)
    if filetype != MH_EXECUTE:
        fail("filetype %d (want MH_EXECUTE)" % filetype)

    static_ok = (flags & MH_NOUNDEFS) and (flags & MH_NOMULTIDEFS)
    dynamic_ok = not (flags & (MH_DYLDLINK | MH_PIE))
    if not static_ok:
        fail("flags 0x%x: want MH_NOUNDEFS|MH_NOMULTIDEFS set" % flags)
    if not dynamic_ok:
        fail("flags 0x%x: want MH_DYLDLINK and MH_PIE clear" % flags)
    print("verify: header OK: MH_EXECUTE x86_64 flags=0x%x (NOUNDEFS|TWOLEVEL|NOMULTIDEFS, no DYLDLINK/PIE)"
          % flags)

    end = 32 + sizeofcmds
    if end > len(data):
        fail("sizeofcmds 0x%x exceeds file" % sizeofcmds)

    segments = {}
    unixthread = None
    seen = set()
    off = 32
    for _ in range(ncmds):
        if off + 8 > end:
            fail("load command overruns header block")
        cmd, cmdsize = struct.unpack_from("<2I", data, off)
        if cmdsize < 8 or off + cmdsize > end:
            fail("load command 0x%x bad size %d" % (cmd, cmdsize))
        if cmd in seen:
            fail("duplicate load command 0x%x" % cmd)
        if cmd not in (LC_SEGMENT_64,):
            seen.add(cmd)
        if cmd in FORBIDDEN:
            fail(FORBIDDEN[cmd] + " present in static kernel")
        if cmd == LC_SEGMENT_64:
            name = data[off + 8:off + 24].split(b"\x00", 1)[0].decode("ascii", "replace")
            (vmaddr, vmsize, fileoff, filesize, maxprot, initprot, nsects,
             _segflags) = struct.unpack_from("<QQQQiiII", data, off + 24)
            body = off + cmdsize
            need = off + 72 + nsects * 80
            if need > off + cmdsize:
                fail("segment %s: nsects %d exceed cmdsize" % (name, nsects))
            print("verify: seg %-10s vmaddr=0x%012x vmsize=0x%-7x fileoff=0x%-7x filesize=0x%-7x %s%s nsects=%d"
                  % (name, vmaddr, vmsize, fileoff, filesize,
                     prot_str(maxprot), prot_str(initprot), nsects))
            if name in segments:
                fail("duplicate segment %s" % name)
            for label, prot in (("maxprot", maxprot), ("initprot", initprot)):
                if (prot & VM_PROT_WRITE) and (prot & VM_PROT_EXECUTE):
                    fail("segment %s: %s %s is write+execute (W^X)"
                         % (name, label, prot_str(prot)))
            segments[name] = (vmaddr, vmsize, fileoff, filesize, initprot)
            if name == "__PAGEZERO":
                if vmaddr != 0 or fileoff != 0 or filesize != 0:
                    fail("__PAGEZERO has file mapping")
                if vmsize % PAGE_SIZE != 0:
                    fail("__PAGEZERO vmsize not page aligned")
            elif name == "__LINKEDIT":
                vmsize_rounded = (vmsize + PAGE_SIZE - 1) & ~(PAGE_SIZE - 1)
                if filesize > vmsize_rounded:
                    fail("__LINKEDIT filesize 0x%x > page-rounded vmsize 0x%x"
                         % (filesize, vmsize_rounded))
                if fileoff + filesize > len(data):
                    fail("__LINKEDIT: data beyond EOF")
            else:
                if filesize > vmsize:
                    fail("segment %s: filesize 0x%x > vmsize 0x%x" % (name, filesize, vmsize))
                if fileoff % PAGE_SIZE != 0 or vmaddr % PAGE_SIZE != 0:
                    fail("segment %s: fileoff/vmaddr not page aligned" % name)
                if fileoff + filesize > len(data):
                    fail("segment %s: data beyond EOF" % name)
                if vmaddr % PAGE_SIZE != fileoff % PAGE_SIZE:
                    fail("segment %s: vmaddr/fileoff page misalignment" % name)
        elif cmd == LC_UNIXTHREAD:
            if unixthread is not None:
                fail("multiple LC_UNIXTHREAD")
            flavor, count = struct.unpack_from("<2I", data, off + 8)
            if flavor != X86_THREAD_STATE64:
                fail("LC_UNIXTHREAD flavor %d (want x86_THREAD_STATE64)" % flavor)
            if count != X86_THREAD_STATE64_COUNT:
                fail("thread state count %d (want %d)" % (count, X86_THREAD_STATE64_COUNT))
            if off + 16 + 168 > off + cmdsize:
                fail("LC_UNIXTHREAD cmdsize too small for x86_thread_state64_t")
            state = struct.unpack_from("<21Q", data, off + 16)
            unixthread = state
        off += cmdsize

    if off != end:
        fail("load command block has %d trailing pad bytes" % (end - off))

    if unixthread is None:
        fail("no LC_UNIXTHREAD load command")
    if "__TEXT" not in segments:
        fail("no __TEXT segment")

    (rax, rbx, rcx, rdx, rdi, rsi, rbp, rsp, r8, r9, r10, r11, r12, r13, r14,
     r15, rip, rflags, cs, fs, gs) = unixthread
    t_vmaddr, t_vmsize, t_fileoff, t_filesize, t_initprot = segments["__TEXT"]
    if not t_initprot & VM_PROT_EXECUTE:
        fail("__TEXT initprot %s is not executable" % prot_str(t_initprot))
    if not (t_vmaddr <= rip < t_vmaddr + t_vmsize):
        fail("entry rip 0x%x not inside __TEXT vm range" % rip)
    if rflags & 0x2 == 0:
        fail("entry rflags reserved bit 1 clear (state not valid)")
    entry_fileoff = t_fileoff + (rip - t_vmaddr)
    if entry_fileoff + 16 > t_fileoff + t_filesize:
        fail("entry rip maps beyond __TEXT file data")
    code = data[entry_fileoff:entry_fileoff + 16]
    if code == b"\x00" * 16:
        fail("entry point maps to zeroed file bytes")
    print("verify: LC_UNIXTHREAD x86_64: rip=0x%012x rsp=0x%x cs=0x%x rflags=0x%x -> fileoff 0x%x"
          % (rip, rsp, cs, rflags, entry_fileoff))

    print("verify: PASS: %s is a static x86_64 Mach-O kernel executable" % path)
    sys.exit(0)


if __name__ == "__main__":
    main()
