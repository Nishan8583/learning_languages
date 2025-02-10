const expect = @import("std").testing.expect;

// erro is enum
// Here FileOpenError is error enum with possible values
const FileOpenError = error{
    AccessDenied,
    OutOfMemory,
    FileNotFOund,
};

// I believe This is
const AllocationError = error{OutOfMemory};

test "coerce error from a subset to a superset" {
    const err: FileOpenError = AllocationError.OutOfMemory;
    try expect(err == FileOpenError.OutOfMemory);
}

test "error union" {

    // error unions, 2 possible values,
    // AllocationError or u16
    const maybe_error: AllocationError!u16 = 10;

    // catch error, if error value will be set to 0
    const no_error = maybe_error catch 0;

    try expect(@TypeOf(no_error) == u16);
    try expect(no_error == 10);
}


// Eroor union return
fn failingFunction() error{Oops}!void {
    return error.Oops
}


test "returning an error" {

    // payload capturing
    failingFunction() catch |err| {
        try expect(err == err.Oops);
        return;
    }
}
