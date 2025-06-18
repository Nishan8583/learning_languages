const std = @import("std");

pub fn main() !void {
    const a = 101;
    const b = 100000;

    const diff = b - a;
    if (diff < 3) {
        std.debug.print("", .{});
    }
}
