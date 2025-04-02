const std = @import("std");
const posix = std.posix;
const net = std.net;
const print = std.debug.print;

// creating server using syscall just like in C
pub fn start_server() !void {

    //creating a socket
    const address = try net.Address.parseIp("127.0.0.1",8080);
    const tpe: u32 = posix.SOCK.STREAM;
    const protocol = posix.IPPROTO.TCP;
    const listener = try posix.socket(address.any.family,tpe,protocol);  // the first paramter is the domain, INET or INET6 we pass in any 
    defer posix.close(listener);


    // no we need to bind the socket 
    try posix.setsockopt(listener,posix.SOL.SOCKET,posix.SO.REUSEADDR, &std.mem.toBytes(@as(c_int,1)));  // setting socket options to make port reusable quickly
    try posix.bind(listener,&address.any, address.getOsSockLen());
    

    // Now make it listen 
    try posix.listen(listener,128);  // lets just make the backlog 128
    

    while (true) {
        var client_address: net.Address = undefined;
        var client_address_len: posix.socklen_t = @sizeOf(net.Address);

        const socket = posix.accept(listener,&client_address.any, &client_address_len,0) catch |err| {
            print("error accepting connection {}\n", .{err});
            continue;
        };
        defer posix.close(socket);
        
        print("Connection {}\n",.{client_address});

        write(socket, "HELLO and GOODBYE\n") catch |err| {
          print("error when trying to write to client socket {}\n",.{err});
        };
    }

}

fn writeAll(socket: posix.socket_t, msg: []const u8) !void {
    var pos: usize = 0;
    while (pos < msg.len) {
        const written = try posix.write(socket,msg[pos..]);
        if (written == 0) {
            return error.closed;
        }
        pos += written;
    }
}
