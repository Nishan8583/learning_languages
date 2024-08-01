const std = @import("std");

pub fn build(b: *std.Build) void {
    const exe = b.addExecutable(.{
        .name = "hello", // going to be the executable thats built
        .root_source_file = .{ .path = "src/main.zig" }, // main file path
        .target = b.standardTargetOptions(.{}), // target to build
        .optimize = b.standardOptimizeOption(.{}), // debug, release ...
    });
    b.installArtifact(exe);

    const run_exe = b.addRunArtifact(exe);

    const run_step = b.step("run", "Run the application");
    run_step.dependOn(&run_exe.step);
}
