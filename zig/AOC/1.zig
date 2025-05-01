const std = @import("std");
const fs = std.fs;

pub fn main() !void {
    try solve2();
}

fn solve1() !void {
    const file = try fs.cwd().openFile("input", .{ .mode = fs.File.OpenMode.read_only });
    defer file.close();

    var buf: [1024]u8 = undefined;

    var reader = std.io.bufferedReader(file.reader());
    var buf_reader = reader.reader();

    var gpa = std.heap.GeneralPurposeAllocator(.{}){};
    const allocator = gpa.allocator();

    var left_list = std.ArrayList(i32).init(allocator);
    defer left_list.deinit();
    var right_list = std.ArrayList(i32).init(allocator);
    defer right_list.deinit();

    while (try buf_reader.readUntilDelimiterOrEof(&buf, '\n')) |line| {
        var it = std.mem.splitSequence(u8, line, "   ");

        const left = it.next() orelse return;
        const right = it.next() orelse return;

        const leftVal = try std.fmt.parseInt(i32, left, 10);
        const rightVal = try std.fmt.parseInt(i32, right, 10);

        try left_list.append(leftVal);
        try right_list.append(rightVal);
    }

    var total_distance: u64 = 0;
    // sort the arrays so that we get the smallest to biggest first
    std.mem.sort(i32, left_list.items, {}, comptime std.sort.asc(i32));
    std.mem.sort(i32, right_list.items, {}, comptime std.sort.asc(i32));

    for (0..left_list.items.len) |i| {
        const diff = left_list.items[i] - right_list.items[i];
        total_distance += @abs(diff);
    }
    std.debug.print("The total distance = {}\n", .{total_distance});
}

fn solve2() !void {
    var file = try std.fs.cwd().openFile("input", .{});
    defer file.close();

    var r = std.io.bufferedReader(file.reader());
    var buf_reader = r.reader();

    var gpa = std.heap.GeneralPurposeAllocator(.{}){};
    const allocator = gpa.allocator();

    var leftNumsArray = std.ArrayList(i32).init(allocator);
    defer leftNumsArray.deinit();

    var buf: [1028]u8 = undefined;

    // map holds the number and its count
    var rightValueCounts = std.AutoArrayHashMap(i32, i32).init(allocator);
    defer rightValueCounts.deinit();

    while (try buf_reader.readUntilDelimiterOrEof(&buf, '\n')) |line| {
        var values = std.mem.splitSequence(u8, line, "   ");

        const leftString = values.next() orelse {
            std.debug.print("left value is not present\n", .{});
            return;
        };

        const rightString = values.next() orelse {
            std.debug.print("Right values is not present\n", .{});
            return;
        };

        const leftInt = try std.fmt.parseInt(i32, leftString, 10);
        const rightInt = try std.fmt.parseInt(i32, rightString, 10);

        // if the value was already counted in rightInt, then increase the count
        if (rightValueCounts.get(rightInt)) |count| {
            try rightValueCounts.put(rightInt, count + 1);
        } else { // else set to 1
            try rightValueCounts.put(rightInt, 1);
        }

        try leftNumsArray.append(leftInt);
    }

    var sum: i32 = 0;
    for (leftNumsArray.items) |leftValues| {
        if (rightValueCounts.get(leftValues)) |count| {
            sum = sum + (leftValues * count);
        }
    }

    std.debug.print("printing type buf={} buf[0..]={} &buf={}\n", .{ @TypeOf(buf), @TypeOf(buf[0..]), @TypeOf(&buf) });
    std.debug.print("the answer for second challange is {d}\n", .{sum});
}
