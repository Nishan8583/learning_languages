const fs = @import("std").fs;
const std = @import("std");
const os = @import("std").os;
const parse = @import("parse.zig");
const args = @import("parse_args.zig");
const print = @import("std").debug.print;
const append_flag = "-a";
const new_file_flag = "-n";

const ProgramError = error {
    FilePathError,
    DBParseError,
    AppenEmployeeError,
    AppendEmployeeNameNotProvidedError,
    AppendEmployeeAddressNotProvidedError,
    AppendEmployeeHoursNotProvided,
};

pub fn main() !void {

    // allocator for the parseing args
    var gpa = std.heap.GeneralPurposeAllocator(.{}){};
    const allocator = gpa.allocator();

    var cli_args = try args.Args.parse_args(allocator);
    defer cli_args.free(allocator);

    // if cli_args.file_path is not null, print it, I have written this just for the sake of showing a way to handle optional type;
    if (cli_args.file_path) |value| {
        print("{s}\n", .{value});
    }

    const file_path = cli_args.file_path orelse return ProgramError.FilePathError;

    var dbOption: ?*parse.DB = null;  // ?[T] means it could be null, or of type T, in this case it could be null or a pointer to parse.DB
    
    // if user asked to create a new file
    if (cli_args.new_file) {
        dbOption = try parse.DB.create_new_db_file(file_path,allocator);
    } else {
        dbOption = try parse.DB.open_db(file_path, allocator);
    }

    var db = dbOption orelse return ProgramError.DBParseError;  // such a cool way to handle null, orelse expression is executed if dbOtpion is null 
    print("DB created successfully.\n",.{});
    defer db.save_to_file() catch |err| {
        print("Errpr {}",.{err});
    };

    try db.load_employees();


    if (cli_args.append_employee) {
        const employee_arg = cli_args.employee_arg orelse return ProgramError.AppenEmployeeError;
        const delimeter = ',';

        var splitter = std.mem.splitScalar(u8,employee_arg,delimeter);

        const name = splitter.next() orelse return ProgramError.AppendEmployeeNameNotProvidedError;
        const address= splitter.next() orelse return ProgramError.AppendEmployeeAddressNotProvidedError;

        print("Appending employee {s} {s}\n",.{name,address});
        try db.add_employee(name,address);
    }


    for (db.employees.items) |e| {
        print("Employee = {s} Address {s}\n",.{e.name,e.address});
    }
    
}
