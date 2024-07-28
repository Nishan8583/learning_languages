// https://zig.guide/getting-started/running-tests
const std = @import("std");

pub fn main() void {
    std.debug.print("Hello {s}\n", .{"World"});
}
