const std = @import("std");
const fs = std.fs;
const print = std.debug.print;

pub fn main() !void {
    const file = try fs.cwd().openFile("input2", .{});
    defer file.close();

    var gpa = std.heap.GeneralPurposeAllocator(.{}){};
    const allocator = gpa.allocator();

    var bufReader = std.io.bufferedReader(file.reader());
    var reader = bufReader.reader();

    var buf: [1024]u8 = undefined;

    var count: u32 = 0;
    while (try reader.readUntilDelimiterOrEof(buf[0..], '\n')) |line| {
        //print("---------------------line {s} \n", .{line});

        var values = std.mem.splitSequence(u8, line, " ");
        var int_values = std.ArrayList(i32).init(allocator);
        defer int_values.deinit();

        while (values.next()) |v| {
            const i = std.fmt.parseInt(i32, v, 10) catch |err| {
                print("unable to parse into an integer {s}, err = {}\n", .{ v, err });
                return;
            };
            try int_values.append(i);
        }

        if (is_valid(int_values.items)) {
            count += 1;
        }
    }
    print("Total safe count {d}\n", .{count});
}

fn is_valid(list: []i32) bool {
    if (list.len < 2) {
        return false;
    }

    const diff = list[1] - list[0];
    if (diff == 0 or diff < -3 or diff > 3) {
        return false;
    }

    const increasing = diff > 0;

    for (1..list.len) |i| {
        // substract current element with previous
        const step = list[i] - list[i - 1];

        // if we are in increasing order
        if (increasing) {
            // if we are in increasing order, then the next value should be increasing
            // and shoudl be increasing to max of 3
            if (step < 1 or step > 3) {
                return false;
            }
        } else {
            // if decreasing then, it should decrease with max of 3
            if (step > -1 or step < -3) {
                return false;
            }
        }
    }
    return true;
}

fn is_valid_dampener(list: []i32) bool {
    if (list.len < 2) {
        return false;
    }

    var dampened = false;

    const diff = list[1] - list[0];
    if (diff == 0 or diff < -3 or diff > 3) {
        return false;
    }

    const increasing = diff > 0;

    for (1..list.len) |i| {
        // substract current element with previous
        const step = list[i] - list[i - 1];

        // if we are in increasing order
        if (increasing) {
            // if we are in increasing order, then the next value should be increasing
            // and shoudl be increasing to max of 3
            if (step < 1 or step > 3) {
                if (!dampened) {
                    list.
                }
                return false;
            }
        } else {
            // if decreasing then, it should decrease with max of 3
            if (step > -1 or step < -3) {
                return false;
            }
        }
    }
    return true;
}
