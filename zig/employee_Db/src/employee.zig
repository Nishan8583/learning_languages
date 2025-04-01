// 1. Define a struct
// 2. DB shoulld have a slice of employees ?
//
//
pub const EmpAllocType= enum {
    stack,
    heap,
};

pub const Emplpoyee = struct {
    name: []const u8,
    address: []const u8,
    alloc_type: EmpAllocType,
    pub fn new(name: []const u8, address: []const u8) Emplpoyee {
        return Emplpoyee{
            .name = name,
            .address = address,
            .alloc_type = EmpAllocType.stack,
        };
    }
};
