// 1. BumpAllocator
// 2. Takes in allocated memory
// 3. When alloc() is called, allocates memory
// 4. Bumps up the pointer

const std = @import("std");
const print = std.debug.print;
const panic = std.debug.panic;

const BumpAllocator = struct {
    memory: []u8,
    offset: u8,

    pub fn Init(memory: []u8) BumpAllocator {
        return BumpAllocator{
            .memory = memory,
            .offset = 0,
        };
    }

    pub fn Allocate(self: *BumpAllocator, alloc_size: u8, alignment: comptime_int) ?[]u8 {

        // std.mem.alignForward makes sure that we align to the next byte, like CPU expectes u32 to start form a aligned address
        // forwards the pointer to align for the type required, like increas the pointer to
        // For example u32 needs to be at a address that is divisible by 4. see my notes for more example
        const alligned_offset = std.mem.alignForward(u8, self.offset, alignment);
        const offset = alligned_offset + alloc_size;
        if (offset > self.memory.len) {
            return null;
        }
        const result = self.memory[alligned_offset..offset];
        self.offset = offset;
        return result;
    }
};

test "memory allocation" {
    const allocator = std.testing.allocator;

    const memory = try allocator.alloc(u8, 512);
    defer allocator.free(memory);

    var custom = BumpAllocator.Init(memory);

    //@alignOf returns the number of bytes this type needs to be aligned to
    const value = custom.Allocate(10, @alignOf(u32)) orelse {
        std.debug.print("Failure allocating", .{});
        return;
    };

    std.testing.expect(value.len == 10) catch |err| {
        std.debug.print("value length not matched expected 10, got {d} error={}", .{ value.len, err });
    };
    std.testing.expect(custom.offset == 10) catch |err| {
        std.debug.print("offset mismatch expected 10 got={d}\n error={} ", .{ custom.offset, err });
    };

    const value2 = custom.Allocate(20, @alignOf(u32)) orelse {
        std.debug.print("Failure allocating", .{});
        return;
    };
    std.debug.print("value2 = {d}\n", .{value2.len});

    std.testing.expect(value2.len == 20) catch {
        std.debug.print("expected length = 20,got{d}", .{value2.len});
        return;
    };
    value2[0] = 1;
    std.debug.print("{}\n", .{custom.offset});
    std.testing.expect(custom.offset == 32) catch {
        std.debug.print("expected offset = 32 ,got{d}", .{custom.offset});
        return;
    };
}

test "memory_allocaion_struct" {
    const test_type = struct {
        field1: u8,
    };

    const allocator = std.testing.allocator;
    const memory = try allocator.alloc(u8, 512);
    defer allocator.free(memory);

    var bump = BumpAllocator.Init(memory);
    var mem = bump.Allocate(@sizeOf(test_type), @alignOf(test_type)) orelse {
        panic("unable to do custom allocation, test failure", .{});
    };

    var t1: *test_type = @ptrCast(@alignCast(&mem));
    try std.testing.expect(@TypeOf(t1) == *test_type);

    t1.field1 = 10;
}
