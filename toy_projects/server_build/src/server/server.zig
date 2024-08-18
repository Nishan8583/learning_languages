const std = @import("std");
const net = std.net;
const address = net.Address;

const welcome_message = "Hello From Server\n";

/// Creates a new server listening on the specified IP address and port.
///
/// # Parameters
///
/// * `ipAddress`: The IP address to listen on.
/// * `port`: The port number to listen on.
///
/// # Returns
///
/// A `net.Server` instance on success, or an error if the server could not be created.
pub fn newServer(ipAddress: []const u8, port: u16) !net.Server {
    const ipv4 = try address.parseIp4(ipAddress, port);
    const server = try net.Address.listen(ipv4, .{ .reuse_address = true, .reuse_port = true });
    return server;
}

pub fn listenAndServe(server: *net.Server) !void {
    defer server.deinit();

    var id: i32 = 0;
    while (true) {
        const connection = server.accept() catch |err| {
            std.debug.print("Could not accept any connection {}", .{err});
            return err;
        };
        _ = try std.Thread.spawn(.{}, handle_connection, .{ id, connection });
        id += 1;
    }
    // accepting one connection for now
}

pub fn handle_connection(id: i32, conn: net.Server.Connection) !void {
    var stream = conn.stream;
    defer stream.close();

    var allocator = std.heap.page_allocator;
    const buffer = try allocator.alloc(u8, 1024);
    defer allocator.free(buffer);

    _ = try stream.write(welcome_message);

    while (true) {
        const bytes_read = stream.read(buffer) catch |err| {
            std.debug.print("Could not read from connection {d} {}", .{ id, err });
            return;
        };
        if (bytes_read == 0) {
            std.debug.print("Connection was closed for {}", .{id});
            return;
        }
    }
}
