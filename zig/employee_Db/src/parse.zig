const MAGIC_HEADER = [4]u8{ 'n', 'i', 's', 'h' };

const db_header = struct {
    magic_bytes: [4]u8,
    employee_count: usize,
};

const fs = @import("std").fs;
const std = @import("std");
const emp = @import("employee.zig");
const print = @import("std").debug.print;
const ArrayList = std.ArrayList;

pub const DBError = error{
    ValidationError,
    ReadingEmployeeCountError,
};

// DB holds information for writing DB into a file
pub const DB = struct {
    file_name: []const u8, // filepath to the db on disk
    file: fs.File, // holds the file structure we use to read and write to file
    employees: ArrayList(emp.Emplpoyee), // growable list of employee
    allocator: std.mem.Allocator, // allocater interface we need this for allocator allocation

    // create_new_db file creats a new db file with the magic header set
    pub fn create_new_db_file(file_name: []const u8, allocator: std.mem.Allocator) !*DB {  // I had to return the pointer, else we could not return a var, and complained that it is not mutaded inside

        // create and write db header stuffs to file
        const file = try fs.cwd().createFile(file_name, .{ .read = true });
        const zero_len = [_]u32{0};
        const len_bytes = std.mem.toBytes(zero_len); // need to convert size to len_bytes, becuase we can only write []u8
        try file.writeAll(&MAGIC_HEADER);
        try file.writeAll(&len_bytes);

        const db = try allocator.create(DB);

        //dereference it and add it there
        db.* = DB{
            .file_name = file_name,
            .file = file,
            .employees = ArrayList(emp.Emplpoyee).init(allocator),
            .allocator = allocator,
        };
        return db;
    }

    pub fn open_db(file_name: []const u8, allocator: std.mem.Allocator) !*DB {
        const file = try fs.cwd().openFile(file_name, .{ .mode = fs.File.OpenMode.read_write });

        // since we call db.validate_header() below, the value needs to be mutable
        const db = try allocator.create(DB);
        db.* = DB{
            .file_name = file_name,
            .file = file,
            .employees = ArrayList(emp.Emplpoyee).init(allocator),
            .allocator = allocator,
        };

        try db.validate_header();
        
        // read till magic header length
        try db.file.seekTo(MAGIC_HEADER.len);

        return db;
    }

    pub fn load_employees(self: *DB) !void {
        
        // the size of employee count is u64, so we ned 8 elements each of 8 bits to hold the value
        var e_count_bytes: [8]u8 = [_]u8{0} ** 8;
        const bytes_read = try self.file.readAll(&e_count_bytes);

        if (bytes_read != @sizeOf(u64)) {
            return DBError.ReadingEmployeeCountError;
        }

        const count = std.mem.bytesToValue(u64,&e_count_bytes);
        print("Employee count {d}\n",.{count});


        var i: u64 = 0; 

        // loop till we get all the employees
        while (i < count) {

            // next 64 bits holds name length, so we need an array of u8 to hold the 64 bit value
            var name_count_bytes: [8]u8 = [_]u8{0} ** 8;
            const nB = try self.file.readAll(&name_count_bytes);  // read the size of name length

            // make sure that we read 64 bits
            if (nB  != @sizeOf(u64)) {
                return DBError.ReadingEmployeeCountError;
            }

            // get an actual integer value from previous u64 we read earlier
            const emp_len = std.mem.bytesToValue(u64,&name_count_bytes);
            const e_usize = @as(usize,@intCast(emp_len));  // convert u64 into usize because that sthe value we need for alloc parameter

            // actually create a buffer for employee name and store that
            const e_name: []u8 = try self.allocator.alloc(u8,e_usize);
            const eB = try self.file.readAll(e_name);
            if (eB != e_usize) {
                return DBError.ReadingEmployeeCountError;
            }

            // Repeat the same thing for address
            // next will be the size of address,
            var address_count_bytes: [8]u8 = [_]u8{0} ** 8;
            const aB = try self.file.readAll(&address_count_bytes);

            if (aB != @sizeOf(u64)) {
                return DBError.ReadingEmployeeCountError;
            }

            const a_name_len = std.mem.bytesToValue(u64,&address_count_bytes);
            const a_usize = @as(usize,@intCast(a_name_len));

            //we got size of address. now read address 
            const address: []u8 = try self.allocator.alloc(u8,a_usize);
            const abb = try self.file.readAll(address);
            if (abb != a_usize) {
                print("expected {d} got {d} value {s}\n",.{aB,abb,address});
                return DBError.ReadingEmployeeCountError;
            }

            const e1 = emp.Emplpoyee{
                .name = e_name,
                .address =  address,
                .alloc_type = emp.EmpAllocType.heap,
            };

            try self.employees.append(e1);


            i+=1;
        }
    }


    // validate header ensures that the the file is infact a db file, by checking MAGIC_HEADER bytes
    pub fn validate_header(self: *DB) !void {

        // seek to previous position after the check
        const prev_seek_pos = try self.file.getPos();
        defer self.file.seekTo(prev_seek_pos) catch |err| {
            std.debug.print("could not seek back {}", .{err});
        };

        try self.file.seekTo(0); // seek to beginning to get MAGIC HEADER
        var buffer: [4]u8 = [_]u8{0} ** 4; // zero out the array

        const bytes_read = try self.file.read(buffer[0..]); // read the file, into the buffer, buffer[0..], we are slicing, so we are creating a new value that is a slice and has a length
        if (bytes_read != 4) {
            return DBError.ValidationError;
        }
        if (!std.mem.eql(u8, MAGIC_HEADER[0..], buffer[0..])) { // compare if both are euqal, if not throw validation error
            return DBError.ValidationError;
        }

        return;
    }

    pub fn close(self: *DB) void {
        if (self.employees.items.len > 0) {
        for (self.employees.items) |e| {

            // if the contents of the struct was allocated in heap, free it
            if (e.alloc_type == emp.EmpAllocType.heap) {
                print("{s}\n",.{e.name});
                self.allocator.free(e.name); // Free memory for employee name
                self.allocator.free(e.address); // Free memory for employee address
            }
        }

        } 
        self.employees.deinit();
        self.file.close();
    }

    pub fn add_employee(self: *DB, name: []const u8, address: []const u8) !void {
        const employee = emp.Emplpoyee.new(name, address);
        try self.employees.append(employee);
    }

    pub fn save_to_file(self: *DB) !void {
        try self.file.seekTo(0);
        try self.file.writeAll(MAGIC_HEADER[0..]);

        const employee_count = @as(u64, self.employees.items.len);
        const eB = std.mem.toBytes(employee_count);
        try self.file.writeAll(&eB);

        for (self.employees.items) |e| {
            const name_len = @as(u64, @intCast(e.name.len)); // usize is platform dependedent, casting it to u64
            const nB = std.mem.toBytes(name_len);
            const address_len = @as(u64, @intCast(e.address.len));
            const aB = std.mem.toBytes(address_len);
            std.debug.print("length {d} {d}\n", .{ name_len, address_len });
            try self.file.writeAll(&nB);
            try self.file.writeAll(e.name);
            try self.file.writeAll(&aB);
            try self.file.writeAll(e.address);
        }
    }
};


test "add_employe_flow" {
    const file_name = "test.db";

    var gpa = std.heap.GeneralPurposeAllocator(.{}){};
    const allocator = gpa.allocator();
    var db = try DB.create_new_db_file(file_name, allocator);
    errdefer db.close();

    try db.validate_header();

    const name = "nishan";
    const address = "thecho";
    try db.add_employee(name, address);

    const name2 = "nishan2";
    const address2 = "thecho2";
    try db.add_employee(name2, address2);

    try std.testing.expect(db.employees.items.len == 2);
    try db.save_to_file();

    db.close();

    var db2 = try DB.open_db(file_name,std.testing.allocator);
    defer db2.close();

    try db2.load_employees();
    try std.testing.expect(db.employees.items.len == 2);

}
