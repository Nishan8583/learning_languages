const std = @import("std");
const net = std.net;
const address = std.net.Address;
const page_allocator = std.heap.page_allocator;

pub fn new(ip: []const u8, port: u16) client {
    const buffer = try page_allocator.alloc(u8, 1024);
    return client{
        .ip = ip,
        .port = port,
        .buffer = buffer,
    };
}

const client = struct {
    ip: []const u8,
    port: u16,
    stream: net.Stream,
    buffer: [1024]u8,

    pub fn connect(self: *client) !void {
        const addr = try address.parseIp4(self.ip, self.port);
        const stream = try net.tcpConnectToAddress(addr);
        _ = try stream.read(self.buffer);
        self.stream = stream;
    }

    pub fn start_chat(self: *client) !void {
        const stdin = std.io.getStdIn();
        const reader = stdin.reader();

        while (true) {
            // !?[]u8: This is the return type of the function.
            // The ! symbol indicates that the function may return an error, and the ? symbol indicates that the function may return null. The return type is a slice of bytes ([]u8).
            const line = reader.readUntilDelimiterOrEof(self.buffer, '\n') catch |err| {
                std.debug.print("Error {}", .{err});
                return;
            };
            if (line == null) {
                return;
            }
            try self.stream.write(line);
        }
    }
};
