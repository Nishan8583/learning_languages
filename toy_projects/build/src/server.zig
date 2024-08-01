const std = @import("std");
const net = std.net;

pub fn create_server() void {
    const ipv4Address: net.Ip4Address = net.Ip4Address.init("127.0.0.1", 8080);
    std.debug.print("{}", .{ipv4Address});
    //const addr: net.Address = net.Address{
    //    .in:
    //};
}
