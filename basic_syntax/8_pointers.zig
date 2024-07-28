const expect = @import("std").testing.expect;

fn increment(num: *u8) void {

    // var_name.* is dereferencing a pointer
    num.* += 1;
}

test "pointers" {
    var x: u8 = 1;
    increment(&x); // passing pointer/referencing using &
    try expect(x == 2);
}

test "naughty poitner" {
    const x: u18 = 0;
    const y: *u8 = @ptrFromInt(x);
    _ = y;
}

test "const pointers" {
    const x: u8 = 1;

    var y = &x;
    y.* += 1;
}
