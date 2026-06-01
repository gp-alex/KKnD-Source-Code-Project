const std = @import("std");

pub fn build(b: *std.Build) void {
    const target = b.standardTargetOptions(.{});
    const optimize = b.standardOptimizeOption(.{});

    const c_flags: []const []const u8 = &.{ "-std=c2x", "-Wall", "-Wextra" };

    // --- raylib (from zig package manager) ---
    const raylib_dep = b.dependency("raylib", .{
        .target = target,
        .optimize = optimize,
    });
    const raylib_lib = raylib_dep.artifact("raylib");

    // --- kknd executable ---
    const kknd_mod = b.createModule(.{
        .target = target,
        .optimize = optimize,
        .link_libc = true,
    });

    kknd_mod.addCSourceFiles(.{
        .files = &.{"src/kknd/kknd.c"},
        .flags = c_flags,
    });

    kknd_mod.addIncludePath(b.path("src/kknd"));
    kknd_mod.addSystemIncludePath(b.path("vendor"));
    kknd_mod.addIncludePath(raylib_dep.path("src"));
    kknd_mod.linkLibrary(raylib_lib);

    const kknd = b.addExecutable(.{
        .name = "kknd",
        .root_module = kknd_mod,
    });

    b.installArtifact(kknd);

    // --- run step ---
    const run_cmd = b.addRunArtifact(kknd);
    run_cmd.step.dependOn(b.getInstallStep());
    if (b.args) |args| {
        run_cmd.addArgs(args);
    }

    const run_step = b.step("run", "Run kknd");
    run_step.dependOn(&run_cmd.step);
}
