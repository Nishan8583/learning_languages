const std = @import("std");
const fs = std.fs;

const testing = std.testing;

// read_file_absolute takes an absolute file path, and reads the content into a buffer
// returns a []u8 or error if any
pub fn read_file_absolute(file_path: []const u8, buffer: []u8) ![]u8 {
    var file: fs.File = fs.openFileAbsolute(file_path, .{ .mode = fs.File.OpenMode.read_only }) catch |err| {
        return err;
    };
    defer file.close();

    const bytes_read = file.readAll(buffer[0..]) catch |err| {
        return err;
    };

    _ = bytes_read;
    return buffer;
}

// test, create a file locally, get the path, check if the read contents match
test "create_file read_file" {
    var allocator = std.heap.page_allocator;

    const file_content = "create_file read_file test !!!!";
    const size = file_content.len;

    // create a file in the current directory
    const file = try fs.cwd().createFile(
        "test.txt",
        .{ .read = false },
    );

    try file.writeAll(file_content);
    file.close();
    // expect returns failure if ok conditin inside is not false
    // we expect value to be non zeor, if not it is going to be false
    // // doc is wrong, new gives void
    // try testing.expect(bw != 0);

    const buffer = try allocator.alloc(u8, size); // holds file content
    const file_path_buffer = try allocator.alloc(u8, 512); // holds file path
    defer allocator.free(buffer);
    defer allocator.free(file_path_buffer);

    const full_path = try fs.cwd().realpath("test.txt", file_path_buffer);
    const content: []u8 = read_file_absolute(full_path, buffer) catch |err| {
        std.debug.print("Test failed {}", .{err});
        return;
    };
    std.debug.print("Value {s}", .{content});
    try testing.expectEqualStrings(file_content, content);

    try fs.cwd().deleteFile("test.txt");
    //    const b: bool = std.mem.eql(u8, content, "Hello World");
    //   if (b) {
    //       std.debug.print("equal", .{});
    //  }
}
