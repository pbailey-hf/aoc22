const std = @import("std");

const Vec2 = struct {
    x: i32 = 0,
    y: i32 = 0,

    fn eq(self: Vec2, other: Vec2) bool {
        return self.x == other.x and self.y == other.y;
    }
};

fn add(a: Vec2, b: Vec2) Vec2 {
    var new = a;
    new.x += b.x;
    new.y += b.y;
    return new;
}

fn sub(a: Vec2, b: Vec2) Vec2 {
    var new = a;
    new.x -= b.x;
    new.y -= b.y;
    return new;
}

fn adjacent(a: Vec2, b: Vec2) !bool {
    var diff = sub(a, b);
    return (try std.math.absInt(diff.x)) <= 1 and (try std.math.absInt(diff.y)) <= 1;
}

const Dir = enum {
    up,
    down,
    left,
    right,

    fn vec(self: Dir) Vec2 {
        return switch (self) {
            Dir.up => Vec2{ .y = 1 },
            Dir.down => Vec2{ .y = -1 },
            Dir.left => Vec2{ .x = -1 },
            Dir.right => Vec2{ .x = 1 },
        };
    }
};

fn charToDir(c: u8) !Dir {
    return switch (c) {
        'U' => Dir.up,
        'D' => Dir.down,
        'L' => Dir.left,
        'R' => Dir.right,
        else => error.ArgNotFound,
    };
}

fn copysign(mag: i32, sign: i32) i32 {
    if (sign >= 0) return mag;
    return -mag;
}

fn doMove(head: Vec2, tail: *Vec2) !void {
    if (!try adjacent(head, tail.*)) {
        var diff = sub(head, tail.*);
        if (try std.math.absInt(diff.x) > 1) diff.x = copysign(1, diff.x);
        if (try std.math.absInt(diff.y) > 1) diff.y = copysign(1, diff.y);
        tail.* = add(tail.*, diff);
    }
}

fn print(rope: [10]Vec2) !void {
    const stdout = std.io.getStdOut().writer();

    var xmax: i32 = -10000;
    var ymax: i32 = -10000;
    var xmin: i32 = 10000;
    var ymin: i32 = 10000;
    for (rope) |k| {
        xmax = std.math.max3(k.x, xmax, 0);
        ymax = std.math.max3(k.y, ymax, 0);
        xmin = std.math.min3(k.x, xmin, 0);
        ymin = std.math.min3(k.y, ymin, 0);
    }
    var iy: i32 = ymax;

    while (iy >= ymin) {
        var ix: i32 = xmin;
        while (ix <= xmax) {
            const p = Vec2{ .x = ix, .y = iy };
            var ir: usize = 0;
            var printed = false;
            while (ir < 10) {
                if (p.eq(rope[ir])) {
                    try stdout.print("{}", .{ir});
                    printed = true;
                    break;
                }
                ir += 1;
            }
            if (!printed) {
                _ = try stdout.write(".");
            }
            ix += 1;
        }
        _ = try stdout.write("\n");
        iy -= 1;
    }

    _ = try stdout.write("\n");
}

pub fn main() !void {
    const stdin = std.io.getStdIn().reader();
    const stdout = std.io.getStdOut().writer();

    var gpa = std.heap.GeneralPurposeAllocator(.{}){};
    const alloc = gpa.allocator();
    var visited = std.AutoHashMap(Vec2, void).init(alloc);
    defer visited.deinit();

    var rope = [_]Vec2{.{}} ** 10;
    var head = &rope[0];
    var tail = &rope[9];
    try print(rope);

    var buffer: [32]u8 = undefined;
    while (try stdin.readUntilDelimiterOrEof(&buffer, '\n')) |line| {
        try stdout.print("{s}\n", .{line});
        const move = (try charToDir(line[0])).vec();
        const nmoves = try std.fmt.parseInt(i32, line[2..], 10);
        var n: i32 = 0;
        while (n < nmoves) {
            head.* = add(head.*, move);
            var ir: usize = 1;
            while (ir < 10) {
                try doMove(rope[ir - 1], &rope[ir]);
                ir += 1;
            }
            //try print(rope);
            try visited.put(tail.*, {});
            n += 1;
        }
    }

    try stdout.print("{}\n", .{visited.count()});
}
