const std = @import("std");
const fs = std.fs;
const Allocator = std.mem.Allocator; // used just as a type, in the read_whole_file function
const expect = std.testing.expect;

pub fn read_file_line_by_line(file_path: []const u8) !void {
    var allocator = std.heap.page_allocator;
    var buffer = try allocator.alloc(u8, 512);
    //    _ = buffer;  // for some reason compiler treats this as never mutated, even if we
    //    write to this buffer
    defer allocator.free(buffer);
    var file: fs.File = undefined;

    if (fs.path.isAbsolute(file_path)) {
        file = fs.openFileAbsolute(file_path, .{ .mode = fs.File.OpenMode.read_only }) catch |err| {
            return err;
        };
    } else {
        const dir = fs.cwd();
        file = dir.openFile(file_path, .{ .mode = fs.File.OpenMode.read_only }) catch |err| {
            return err;
        };
    }
    defer file.close();

    var seek: usize = 0;
    while (true) {
        const bytes_read = try file.read(buffer);

        // EOF is not an error indication
        // when EOF is reached bytes_read is smaller than buffer.len
        if (bytes_read == 0) {
            break;
        }
        seek += bytes_read;
        file.seekTo(seek) catch |err| {
            std.debug.print("could not seek to position {d}\n", .{seek});
            std.debug.print("{s}", .{buffer[0..bytes_read]});
            return err;
        };

        std.debug.print("{s}\n", .{buffer[0..bytes_read]});
        for (buffer[0..bytes_read]) |byte| {
            std.debug.print("{d} ", .{byte});
        }
    }
}

pub fn read_file() !void {
    const allocator = std.heap.page_allocator;
    var args = try std.process.argsWithAllocator(allocator);
    defer args.deinit();
    _ = args.next(); // ignore itself
    while (true) {
        const v = args.next(); // args.next() returns ?([:0]const u8), ? means it returns either null or the other value, the we handle this optional type is in the below if condition
        if (v) |result| {
            std.debug.print("The arguement {s}", .{result});
            read_file_line_by_line(result) catch |err| {
                return err;
            };
        } else {
            break;
        }
    }
}

pub fn main() !void {

    // we need to create a custom allocator, any standard library function will not allocate memory on its own
    //var gpa = std.heap.GeneralPurposeAllocator(.{}){};
    //const allocator = gpa.allocator();
    //var allocator = std.heap.page_allocator;
    //var file_path = allocator.alloc(u8, 1024);
    //const file_path = "C:\\D\\projects\\Zig Projects\\example.txt";

    //const path = "C:\\D\\projects\\Zig Projects\\example.txt";
    //var mutable_path: [38]u8 = undefined;
    //std.mem.copy(u8, &mutable_path, path_str);

    // Cast the const array to a mutable slice
    //try read_file_line_by_line(path);

    //try read_whole_file(allocator, "example.txt");
    //
    try read_file();
}

// read_whole_file takes an allocator, path its a string, returns error if any
pub fn read_whole_file(path: []const u8) !void {
    const allocator = std.heap.page_allocator;
    const data = try fs.cwd().readFileAlloc(allocator, path, 1000);
    defer allocator.free(data); // defer called when returning from the function

    // output content to the screen
    try std.io.getStdOut().writer().writeAll(data);
}
