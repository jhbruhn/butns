include <./roundedcube.scad>

height = 5.3;

s = 8.5;

px = 11.43;
py = 11.4;

inlayslack = 1;
rubberslack = .6;

inlayheight = 2.6 - inlayslack;

w = 0.42*3;
h = 0.12*3;

cross = true;
/*
intersection() {
difference() {
union() {
for(x=[0:8-1]) for(y=[0:8-1])
translate([x * px, y * py, height / 2]) roundedcube([s, s, height], center = true, radius = 1.5, "zmax");

//translate([- s, -s, 0])cube([7 * px + s * 2, 7 * py + s * 2, inlayheight ]);
if (cross) {
for(x=[0:14])
translate([4 * px, (x-3) * py, h/2]) rotate([0, 0, 45]) cube([14*px, w, h], center = true);

for(x=[0:14])
translate([4 * px, (x-4) * py, h/2]) rotate([0, 0, -45]) cube([14*px, w, h], center = true);
} else {
for(x=[0:8-1]) for(y=[0:8-1]) {
translate([x * px, y * py - s / 4, h/2]) cube([12, w, h], center=true);
translate([x * px, y * py + s / 4, h/2]) cube([12, w, h], center=true);
translate([x * px - s / 4, y * py, h/2]) cube([w, 12, h], center=true);
translate([x * px + s / 4, y * py, h/2]) cube([w, 12, h], center=true);
}
}

}
translate([3.5 * px, 3.5 * py, 0]) cylinder(h = 20, d = 5.4, center = true);
translate([-px/2-1, -py/2-1, 0]) cylinder(h = 20, d = 5.8, center = true);
translate([7.5 * px+1, 7.5 * py+1, 0]) cylinder(h = 20, d = 5.8, center = true);
translate([-px/2-1, 7.5 * py+1, 0]) cylinder(h = 20, d = 5.8, center = true);
translate([7.5 * px+1, -py/2-1, 0]) cylinder(h = 20, d = 5.8, center = true);

}
translate([-s/2-1.5, -s/2-1.5, -1]) roundedcube([8*px-2.9+3, 8*py-2.9+3, 10], radius=1.5, center=false, "zmax");


}

for(x=[0:8-1]) for(y=[0:8-1])
translate([x * px + s/2, y * py, inlayheight / 2]) cube([3.25, s/3, inlayheight], center = true);

for(y=[0:8-1])
translate([-1 * px + s - 1, y * py, inlayheight / 2]) cube([2.75, s/4, inlayheight], center = true);
*/
color("blue")
for(x=[0:8-1]) for(y=[0:8-1])
translate([x * px - s/2+.15, y * py, (height - 2) / 2]) cube([.4, s-3, height - 2], center = true);
/*


color("blue") translate([0, 0, inlayheight]) difference() {
 translate([3.5*px, 3.5*py, (inlayslack - rubberslack) / 2]) cube([7*px, 7*py, inlayslack - rubberslack], center = true);
 
 for(x=[0:8-1]) for(y=[0:8-1])
translate([x * px, y * py, height / 2]) roundedcube([s+rubberslack, s + rubberslack, height], center = true, radius = 1.5, "zmax");

}*/