    
    
difference() {
    cylinder(r=8, h=28, $fn=360);
    translate([0,0,10])
        cylinder(r=4, h=10);
    cylinder(r=1.6, h=10, $fn=360);
    translate([-3,2,0])
        cube([6,3,9]);
    translate([0,0,5])
        rotate([270,0,0])
            cylinder(r=1.6,h=30, $fn=360);
}
difference() {
translate([0,0,10])
    cylinder(r=17.5, h=9);
translate([0,0,14.5])
    rotate_extrude(convexity = 10)
        translate([17.5, 0, 0])
            circle(r = 2.5);
}

translate([-50,0,0])
{
    difference() {
        cylinder(r=8, h=28, $fn=360);
        translate([0,0,10])
            cylinder(r=4, h=10);
        cylinder(r=1.6, h=10, $fn=360);
        translate([-3,2,0])
            cube([6,3,9]);
        translate([0,0,5])
            rotate([270,0,0])
                cylinder(r=1.6,h=30, $fn=360);
    translate([0,0,14.5])
        rotate_extrude(convexity = 10)
            translate([8, 0, 0])
                circle(r = 2.5);
    }
}

translate([50,0,0])
    difference() {
        cylinder(r=8, h=20, $fn=360);
        translate([0,0,10])
            cylinder(r=4, h=10);
        cylinder(r=1.6, h=10, $fn=360);
        translate([-3,2,0])
            cube([6,3,9]);
        translate([0,0,5])
            rotate([270,0,0])
                cylinder(r=1.6,h=30, $fn=360);
    }
