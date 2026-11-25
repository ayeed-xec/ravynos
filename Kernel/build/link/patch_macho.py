import struct
import sys

MH_MAGIC_64 = 0xfeedfacf
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

DROP = {
    LC_MAIN,
    LC_DYLD_INFO,
    LC_DYLD_INFO_ONLY,
    LC_LOAD_DYLIB,
    LC_LOAD_WEAK_DYLIB,
    LC_REEXPORT_DYLIB,
    LC_DYSYMTAB,
    LC_FUNCTION_STARTS,
    LC_DATA_IN_CODE,
    LC_CODE_SIGNATURE,
}


def die(msg):
    print("patch_macho: error: %s" % msg)
    sys.exit(1)


def main():
    if len(sys.argv) != 2:
        die("usage: patch_macho.py <macho>")
    path = sys.argv[1]
    with open(path, "rb") as f:
        data = bytearray(f.read())

    if len(data) < 32:
        die("file too small")
    (magic, cputype, cpusubtype, filetype, ncmds, sizeofcmds, flags,
     reserved) = struct.unpack_from("<8I", data, 0)
    if magic != MH_MAGIC_64:
        die("bad magic 0x%08x" % magic)
    if filetype != MH_EXECUTE:
        die("not MH_EXECUTE")

    end = 32 + sizeofcmds
    if end > len(data):
        die("sizeofcmds 0x%x exceeds file" % sizeofcmds)

    segments = {}
    entryoff = None
    cmds = []
    dropped = []
    off = 32
    for _ in range(ncmds):
        cmd, cmdsize = struct.unpack_from("<2I", data, off)
        if cmdsize < 8 or off + cmdsize > end:
            die("load command 0x%x bad size" % cmd)
        raw = bytes(data[off:off + cmdsize])
        if cmd == LC_SEGMENT_64:
            name = raw[8:24].split(b"\x00", 1)[0].decode("ascii", "replace")
            (vmaddr, vmsize, fileoff, filesize, _maxp, _initp, _n,
             _sf) = struct.unpack_from("<QQQQiiII", raw, 24)
            segments[name] = (vmaddr, vmsize, fileoff, filesize)
        elif cmd == LC_MAIN:
            (entryoff,) = struct.unpack_from("<I", raw, 8)
        if cmd in DROP:
            dropped.append(cmd)
        else:
            cmds.append(raw)
        off += cmdsize
    if off != end:
        die("trailing bytes in load command block")

    if "__TEXT" not in segments:
        die("no __TEXT segment")
    if entryoff is None:
        die("no LC_MAIN emitted by linker; nothing to convert")

    t_vmaddr, t_vmsize, t_fileoff, t_filesize = segments["__TEXT"]
    if not (t_fileoff <= entryoff < t_fileoff + t_filesize):
        die("entryoff 0x%x outside __TEXT file range" % entryoff)
    rip = t_vmaddr + (entryoff - t_fileoff)

    if "__DATA" in segments:
        d_vmaddr, d_vmsize, _f, _fs = segments["__DATA"]
        rsp = d_vmaddr + d_vmsize
    else:
        rsp = t_vmaddr + t_vmsize

    state = [0] * 21
    state[7] = rsp
    state[16] = rip
    state[17] = 0x2
    state[18] = 0x8
    thread = struct.pack("<2I", LC_UNIXTHREAD, 16 + 168)
    thread += struct.pack("<2I", X86_THREAD_STATE64, X86_THREAD_STATE64_COUNT)
    thread += struct.pack("<21Q", *state)
    cmds.append(thread)

    new = b"".join(cmds)
    new_sizeof = len(new)

    flags = (flags & ~(MH_DYLDLINK | MH_PIE)) | MH_NOUNDEFS | MH_TWOLEVEL | MH_NOMULTIDEFS
    struct.pack_into("<8I", data, 0, magic, cputype, cpusubtype, MH_EXECUTE,
                     len(cmds), new_sizeof, flags, reserved)
    del data[32 + new_sizeof:end]
    data[32:32 + new_sizeof] = new
    with open(path, "wb") as f:
        f.write(data)
    print("patch_macho: dropped %d load commands, wrote LC_UNIXTHREAD rip=0x%012x rsp=0x%012x, flags=0x%x"
          % (len(dropped), rip, rsp, flags))


if __name__ == "__main__":
    main()
