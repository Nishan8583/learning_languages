const std = @import("std");

const constant: i32 = 5;

var variable: u32 = 5000;


// arrays
const a = [5]u8{1,2,3,4,5}
cosnt b = [_]u8{1,2,3,4,5}  // infer size at runtime

pub fn main() void {
    std.debug.print("{i32 d }", .{ constant, variable });
}
