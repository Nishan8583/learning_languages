const Server = @import("./server.zig");
const std = @import("std");
const net = std.net;
const thread = std.Thread;
const time = std.time;

fn serveWrapper(s: *Server.server) !void {
    try s.listenAndServe();
}
test "server" {
    const serverIP = "127.0.0.1";
    const serverPort = 8080;
    var s = try Server.newServer(serverIP, serverPort);
    defer s.destroy();

    const serveThread = try thread.spawn(.{}, serveWrapper, .{&s});

    // now its already running?
    std.debug.print("Waiting for the server to be ready ", .{});
    time.sleep(5000);
    std.debug.print("Wait finished", .{});

    const ipv4 = try net.Address.parseIp4(serverIP, serverPort);
    const client = try net.tcpConnectToAddress(ipv4);

    var allocator = std.heap.page_allocator;
    const buffer = try allocator.alloc(u8, 1024);

    _ = try client.read(buffer);

    serveThread.join();
}
