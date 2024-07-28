const std = @import("std");
const eql = std.mem.eql;
const expect = std.testing.expect;

test "createFile, write, seekTo, read" {
    const file = try std.fs.cwd().createFile(
        "junk_file.txt",
        .{ .read = true },
    );
    defer file.close();

    const bytes_written = try file.writeAll("hello file!");
    _ = bytes_written;

    var buffer: [100]u8 = undefined;
    try file.seekTo(0);

    const bytes_read = try file.readAll(&buffer);

    try expect(eql(u8, buffer[0..bytes_read], "hello file!"));
}
