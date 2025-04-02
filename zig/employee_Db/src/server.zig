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
    
    var buf: [128]u8 = [_]u8{0} ** 128;
    while (true) {
        var client_address: net.Address = undefined;
        var client_address_len: posix.socklen_t = @sizeOf(net.Address);

        const socket = posix.accept(listener,&client_address.any, &client_address_len,0) catch |err| {
            print("error accepting connection {}\n", .{err});
            continue;
        };
        defer posix.close(socket);
        
        print("Connection {}\n",.{client_address});

        const stream = net.Stream{.handle = socket};

        const read = stream.read(&buf) catch |err| {
            print("error reading  {}",.{err});
            continue;
        };

        if (read == 0) {
            continue;
        }

        stream.writeAll(buf[0..read]) catch |err| {
            print("error writing {}",.{err});
        };

    }

}


// Read the message boundaries readme notes
fn writeMessage(socket: posix.socket_t, msg: []const u8) !void {
    var buf: [4]u8 = undefined;
    std.mem.writeInt(u32,&buf,@intCast(msg.len),.little);
    try write(socket,&buf);
    try write(socket,msg);
    
}
fn write(socket: posix.socket_t, msg: []const u8) !void {
    var pos: usize = 0;
    while (pos < msg.len) {
        const written = try posix.write(socket,msg[pos..]);
        if (written == 0) {
            return error.closed;
        }
        pos += written;
    }
}

fn readMessage(socket: posix.socket_t, buf: []u8) ![]u8 {
    var header: [4]u8 = undefined;
    try readAll(socket, &header);

    const len = std.mem.readInt(u32, &header, .little);
    if (len > buf.len) {
        return error.BufferTooSmall;
    }

    const msg = buf[0..len];
    try readAll(socket, msg);
    return msg;
}

fn readAll(socket: posix.socket_t, buf: []u8) !void {
    var into = buf;
    while (into.len > 0) {
        const n = try posix.read(socket, into);
        if (n == 0) {
            return error.Closed;
        }
        into = into[n..];
    }
}
