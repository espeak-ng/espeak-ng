import sys
import struct

if __name__ == "__main__":
    expected_format = sys.argv[1]

    header = sys.stdin.buffer.read(12)
    if len(header) < 12:
        print("Invalid header length: %d, expected 12 bytes" % len(header), file=sys.stderr)
        sys.exit(1)

    chunk_id = header[0:4]
    if chunk_id != b"RIFF":
        print("Invalid CHUNK_ID: \"%s\", expected RIFF" % chunk_id.decode(), file=sys.stderr)
        sys.exit(1)


    # litte-endian 32-bit integer
    chunk_size = struct.unpack("<I", header[4:8])
    # TODO: ...

    format = header[8:12]
    if format != expected_format.encode("ascii"):
        print("Unexpected format: \"%s\", expected: \"%s\"" % format.decode(), expected_format, file=sys.stderr)
        sys.exit(1)
