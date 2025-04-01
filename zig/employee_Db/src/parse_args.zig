const std = @import("std");
const allocator = @import("std").mem.Allocator;
const emp = @import("employee.zig");

const FILE_PATH="-f";
const NEW_FILE="-n";
const APPEND = "-a";
const LIST = "-l";


pub fn usage() void {
    std.debug.print("invalid arguements provided\n", .{});
    std.debug.print("Usage: \n", .{});
    std.debug.print("\t -f <path to file>\n", .{});
}
const ArgParseError = error {
    ErrInvalidError,
};


pub const Args = struct {
    new_file: bool,
    file_path: ?[]const u8,  // optional type, can be either null or T value
    append_employee: bool,
    employee_arg: ?[]const u8,
    list_employees: bool,


    // using allocator because the std.os.args way like C only works in linux according to docs, even if that process requires no allocations
    pub fn parse_args(gpa_allocator: allocator) !Args {

        const args = try std.process.argsAlloc(gpa_allocator);
        defer std.process.argsFree(gpa_allocator,args);

        var cli_args = Args{
            .new_file = false,
            .file_path = null,
            .append_employee = false,
            .employee_arg = null,
            .list_employees = false,
        };

        for (args, 0..) |arg,i| {
            //if (next_is_file) {
                //const file = try gpa_allocator.alloc(u8,arg.len);
                //@memcpy(file, arg);
                //cli_args.file_path = file;

                // cleaner and safer
             //   cli_args.file_path = try gpa_allocator.dupe(u8, arg);
              //  continue;
            //}

            if (std.mem.eql(u8,arg,FILE_PATH)) {  // parse the filepath
                if (i+1 >= args.len) {
                    usage();
                    return ArgParseError.ErrInvalidError;
                }
                cli_args.file_path = try gpa_allocator.dupe(u8,args[i+1]);
            } else if (std.mem.eql(u8,arg,NEW_FILE)) {
                cli_args.new_file = true;
            } else if (std.mem.eql(u8,arg,APPEND)) { // parse the command line arguements
                if (i+1 >= args.len) {
                    usage();
                    return ArgParseError.ErrInvalidError;
                }
                cli_args.employee_arg = try gpa_allocator.dupe(u8,args[i+1]);
                cli_args.append_employee = true;
            } else if (std.mem.eql(u8,arg,LIST)) {
                cli_args.list_employees = true;
            }
        }

    return cli_args;
    }

    pub fn free(self: *Args, gpa_allocator: allocator) void {
        if (self.file_path) |value| {  // unwrapping the value, getting the actual value, similar to Option. .? is equivalent to .unwrap()
            gpa_allocator.free(value);
        }
        if (self.employee_arg) |value|  {
            gpa_allocator.free(value);
        }
    }

};
