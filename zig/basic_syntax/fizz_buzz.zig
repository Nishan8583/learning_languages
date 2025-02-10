const std = @import("std");

pub fn main() !void {
    const stdout = std.io.getStdOut().writer();

    // redeclation can not be allowed
    //var count = 1 ; // type is //comptime_int, its value can only be changed during compile 15:40
    var count: u8 = 1;

    while (count <= 100) : (count += 1) {
        if (count % 3 == 0 and count % 5 == 0) {
            try stdout.writeAll("Fizz Buzz \n");
        } else if (count % 5 == 0) {
            try stdout.writeAll("Bizz\n");
        } else if (count % 3 == 0) {
            try stdout.writeAll("Fizz\n");
        } else {
            try stdout.print("{}\n", .{count});
        }
    }
}
