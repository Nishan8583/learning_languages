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

pub fn read_file_line_by_line(file_path: []u8, buffer: []u8, temp_buffer: []u8) !void {
    var file: fs.File = fs.openFileAbsolute(file_path, .{ .mode: fs.File.OpenMode.read_only }) catch |err| {
        return err;
    }
    defer file.close();
    
    var seek = 0;
    while (true) {
        const bytes_read: u8 = try file.read(temp_buffer);

        // EOF is not an error indication
        // when EOF is reached bytes_read is smaller than buffer.len
        if bytes_read < temp_buffer.len() {
            buffer[seek..]
            break;
        }
        seek += bytes_read;
        file.seekTo(seek) catch |err| {
            std.debug.print("could not seek to position {d}\n",.{seek});
            return err;
        };

    }

}

test "read line by line" {
    var allocator = std.heap.page_allocator;
    const file_content = "x";
    const file_name = "read_line_by_line_test.txt";

    const file = try fs.cwd().createFile(
        file_name,
        .{ .read = false },
    );
    defer fs.cwd().deleteFile(file_name);

    for (0..100000) | _ | {
     
        // .writeAll() ensures entire buffer is written in one call
        try file.writeAll(file_content,file_content);

    }

    file.Close();

    var buffer =try allocator.alloc(1000000);
    var temp_buffer = try allocator.alloc(100);
    defer allocator.free(temp_buffer);
    defer allocator.free(buffer);


    try read_file_line_by_line(file_name,buffer,temp_buffer);
    try testing.expect.equal(100000 == buffer.len);
    for (buffer, 0..) | char,_ | {
        testing.expect.equal("x" == char);
    }

}
