# endianness

// binary encoding
// so here we represent using 4 bytes {0,0,0,0}  = 0, {0,0,0,1} = 1, {0,0,0,2} =2 and so on until {0,0,0,255}
// so how do we represent 256? we use the next value {0,0,1,0} = 256, {0,0,1,1} = 257, {0,0,1,2} = 258
// Hwere we are using the right most part to represent value, i.e. the values on the right start the value, the other values are ignored if 0, so this is Big Endian, most significat bits on the right, i.e. we start 
// representing nmumber by changing values on right first
// Little endian we start represting from left, so 1 becomse {1,0,0,0}, and 257 becomes {1,1,0,0}
```zig
const std = @import("std");
const endianness = @import("builtin").cpu.arch.endian();

pub fn main() !void {
    var buf: [4]u8 = [_]u8{0} ** 4;

    std.mem.writeInt(u32,&buf,257, .big);  // most signifiacts bits are on the right
    std.debug.print("big endian {any}\n",.{buf});

    std.mem.writeInt(u32,&buf,257,.little);  // most signifiacts bits are on the left
    std.debug.print("little endian {any}\n",.{buf});

    std.debug.print("endinanness {any}\n",.{endianness});
}
```

