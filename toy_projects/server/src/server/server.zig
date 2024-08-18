const std = @import("std");
const net = std.net;
const address = net.Address;

const welcome_message = "Hello From Server\n";

// struct server
pub const server = struct {
    ip: []const u8,
    port: u16,
    boundAddress: address,
    listener: *net.Server,

    pub fn destroy(self: *const server) void {
        self.listener.deinit();
        return;
    }

    pub fn listenAndServe(self: *server) !void {

        // accepting one connection for now
        const connection = self.listener.accept() catch |err| {
            std.debug.print("Could not accept any connection {}", .{err});
            return err;
        };
        var stream = connection.stream;
        defer stream.close();

        var allocator = std.heap.page_allocator;
        const buffer = try allocator.alloc(u8, 1024);
        defer allocator.free(buffer);

        _ = try stream.write(welcome_message);

        while (true) {
            const bytes_read = stream.read(buffer) catch |err| {
                std.debug.print("Could not read from buffer {}", .{err});
                return err;
            };
            if (bytes_read == 0) {
                std.debug.print("No bytes read ", .{});
                return;
            }
            std.debug.print("{s}\n", .{buffer[0..bytes_read]});
        }
    }
};

pub fn newServer(ipAddress: []const u8, port: u16) !server {
    const ipv4 = try address.parseIp4(ipAddress, port);
    var listener = try net.Address.listen(ipv4, .{ .reuse_address = true, .reuse_port = true });
    const s = server{
        .ip = ipAddress,
        .port = port,
        .boundAddress = ipv4,
        .listener = &listener,
    };

    return s;
}
