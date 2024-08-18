const Server = @import("./server.zig");
const std = @import("std");
const net = std.net;
const thread = std.Thread;
const time = std.time;
const testing = std.testing;
const welcome_message = "Hello From Server\n";

pub fn startClient(wg: *std.Thread.WaitGroup, address: []const u8, port: u16) !void {
    defer wg.finish();
    const addr = try net.Address.parseIp4(address, port);
    const client = try net.tcpConnectToAddress(addr);

    var allocator = std.heap.page_allocator;
    const buffer = try allocator.alloc(u8, 1024);
    const b = try client.read(buffer);
    try testing.expectEqualStrings(welcome_message, buffer[0..b]);
}

test "server" {
    const serverIP = "127.0.0.1";
    const serverPort = 8080;
    var s = try Server.newServer(serverIP, serverPort);

    _ = try thread.spawn(.{}, Server.listenAndServe, .{&s});

    // now its already running?
    std.debug.print("Waiting for the server to be ready ", .{});
    time.sleep(5000);
    std.debug.print("Wait finished", .{});
    var wg = std.Thread.WaitGroup{};

    wg.start();
    _ = try std.Thread.spawn(.{}, startClient, .{ &wg, serverIP, serverPort });
    wg.start();
    _ = try std.Thread.spawn(.{}, startClient, .{ &wg, serverIP, serverPort });
    std.debug.print("Waitig for all tests to finish", .{});
    wg.wait();
    //serveThread.join();
}
