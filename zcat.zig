const std = @import("std");
const fs = std.fs;
const Allocator = std.mem.Allocator; // used just as a type, in the read_whole_file function

pub fn main() !void {

    // we need to create a custom allocator, any standard library function will not allocate memory on its own
    var gpa = std.heap.GeneralPurposeAllocator(.{}){};
    const allocator = gpa.allocator();

    try read_whole_file(allocator, "example.txt");
}

// read_whole_file takes an allocator, path its a string, returns error if any
pub fn read_whole_file(allocator: Allocator, path: []const u8) !void {
    const data = try fs.cwd().readFileAlloc(allocator, path, 1000);
    defer allocator.free(data); // defer called when returning from the function

    // output content to the screen
    try std.io.getStdOut().writer().writeAll(data);
}
