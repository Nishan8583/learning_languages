const std = @import("std");
const print = std.debug.print;

pub fn main() !void {
    const reader = std.io.getStdIn().reader();
    var buffer: [4096]u8 = undefined;

    var gpa = std.heap.GeneralPurposeAllocator(.{}){};
    const allocator = gpa.allocator();
    while (true) {
        print("zigsh >>> ", .{});

        const input = try reader.readUntilDelimiterOrEof(&buffer, '\n');
        if (input) |command| {
            print("User input {s}\n", .{command});
            var child = std.process.Child.init(&.{ "cmd.exe", "/c", "dir" }, allocator);
            const val = try child.spawnAndWait();
            print("VAL {}\n", .{val});
        } else {
            print("EOF exiting", .{});
            return;
        }
    }
}
