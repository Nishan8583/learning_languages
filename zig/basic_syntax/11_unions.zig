const expect = @import("std").testing.expect;

// unions can have many types, at the same type only one type is allowed
const Result = union {
    int: i64,
    float: f64,
    bool: bool,
};

//
//test "simple union" {
//    var result = Result{ .int = 1234 };

// right now, only .int is active, so accessing a field that is not active will throw an error
//    result.float = 12.34;
//}
//
// We can use tagged unions to detect which field is active

const Tag = enum { a, b, c };
const Tagged = union(Tag) { a: u8, b: f32, c: bool };

test "switch on tagged union" {
    var value = Tagged{ .b = 1.5 };

    // reember byte, float and b are not type in zig, here they are name
    // pointer are captured, captured values are immutable, but we dereference them using .*
    // and can update the values
    switch (value) {
        .a => |*byte| byte.* += 1,
        .b => |*float| float.* += 2,
        .c => |*b| b.* = !b.*,
    }
    try expect(value.b == 3);
}
