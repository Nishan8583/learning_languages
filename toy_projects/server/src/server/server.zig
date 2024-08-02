const std = @import("std");
const net = std.net;
const address = net.Address;

pub fn create_server() !void {
    const ipv4 = try address.parseIp4("127.0.0.1", 8080);

    var server = try address.listen(ipv4, .{ .reuse_port = true, .reuse_address = true });
    std.debug.print("Server Started \n", .{});
    defer server.deinit();

    const connection = try server.accept();
    std.debug.print("New connection established \n", .{});
    var stream = connection.stream;
    defer stream.close();

    var allocator = std.heap.page_allocator;
    const buffer = try allocator.alloc(u8, 1024);
    defer allocator.free(buffer);

    const welcome_message = "Hello Client\n";
    _ = try stream.write(welcome_message);
    while (true) {
        const bytes_read = try stream.read(buffer);
        if (bytes_read == 0) {
            std.debug.print("No bytes read ", .{});
            return;
        }
        std.debug.print("{s}\n", .{buffer[0..bytes_read]});
    }
}
