width  = 185;
height = 149;
module spinner() {
    union() {
        translate([2,-6.75,65])
            difference() {
                scale([0.7,0.7,0.5])
                    translate([-width/2,-height/2,-130])
                        cube([width,height,20]);
                    scale([0.7,0.7,1])
                        surface(file="image.png", center=true, invert=true, convexity=2);
            }
        translate([0,0,0])
            cylinder(h=10, r=40, center=true);
        translate([0,0,-25])
            difference() {
                cylinder(h=50, r=33/2, center=true);
                cylinder(h=50, r=28/2, center=true);
            }
    }
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

// The add_fillets module will either auto fillet the whole model or
// a section of the model depending on if ther are one or two children.
// 1 child:       Fillet the whole model
// Two children:  Fillet the first child in the area that is
//                 intersected by the second child 
module add_fillets(R=1,OB=1000,axis="xyz",fn=10)
{
    fn_even = ceil(fn/2)*2;
    if ($children==1)
    {
        minkowski_fillets(R=R,OB=OB,axis=axis,fn=fn_even)
        {
            children(0);
        }
    }
    if ($children==2)
    {
        union()
        {
            minkowski_fillets(R=R,OB=OB,axis=axis,fn=fn_even)
            {
                intersection()
                {
                    children(0);
                    children(1);
                }
            }
            difference()
            {
                children(0);
                children(1);
            }
        }
    }
}


// The add_rounds module will either auto round the whole model or
// a section of the model depending on if ther are one or two children.
// 1 child:       Round the whole model
// Two children:  Round the first child in the area that is
//                  intersected by the second child 
module add_rounds(R=1,OB=1000,axis="xyz",fn=10)
{
    fn_even = ceil(fn/2)*2;
    if ($children==1)
    {
        minkowski_rounds(R=R,OB=OB,axis=axis,fn=fn_even)
        {
            children(0);
        }
    }
    if ($children==2)
    {
        union()
        {
            minkowski_rounds(R=R,OB=OB,axis=axis,fn=fn_even)
            {
                intersection()
                {
                    children(0);
                    minkowski()
                    {
                        children(1);
                        if (axis=="x")   rotate([0 ,90,0]) cylinder(r=R,$fn=fn_even,center=true);
                        if (axis=="y")   rotate([90,0 ,0]) cylinder(r=R,$fn=fn_even,center=true);
                        if (axis=="z")   rotate([0 ,0 ,0]) cylinder(r=R,$fn=fn_even,center=true);
                        if (axis=="xyz") sphere(r=R,$fn=fn_even);
                    }
                }
            }
            difference()
            {
                children(0);
                children(1);
            }
        }
    }
}




module minkowski_fillets(R=1,OB=1000,axis="xyz",fn=10)
{
    OBplus=OB+4*R;
    fn_even = ceil(fn/2)*2;
    echo(fn_even);
    difference()
    {
        cube([OB,OB,OB],center=true);
        minkowski()
        {   
            difference()
            {
                cube([OBplus,OBplus,OBplus],center=true);
                minkowski()
                {
                    children(0);
                    if (axis=="x")   rotate([0 ,90,0]) cylinder(r=R,$fn=fn_even,center=true);
                    if (axis=="y")   rotate([90,0 ,0]) cylinder(r=R,$fn=fn_even,center=true);
                    if (axis=="z")   rotate([0 ,0 ,0]) cylinder(r=R,$fn=fn_even,center=true);
                    if (axis=="xyz") sphere(r=R,$fn=fn);
                }
            }
            if (axis=="x")   rotate([0 ,90,0]) cylinder(r=R,$fn=fn_even,center=true);
            if (axis=="y")   rotate([90,0 ,0]) cylinder(r=R,$fn=fn_even,center=true);
            if (axis=="z")   rotate([0 ,0 ,0]) cylinder(r=R,$fn=fn_even,center=true);
            if (axis=="xyz") sphere(r=R,$fn=fn_even);
        }
    }
}


module minkowski_rounds(R=1,OB=1000,axis="xyz",fn=10)
{
    OBplus=OB+4*R;
    fn_even = ceil(fn/2)*2;
    minkowski()
    {  
        difference()
        {
            cube([OB,OB,OB],center=true);
            minkowski()
            {
                difference()
                {
                    cube([OBplus,OBplus,OBplus],center=true);
                    children(0);
                }
                if (axis=="x")   rotate([0 ,90,0]) cylinder(r=R,$fn=fn_even,center=true);
                if (axis=="y")   rotate([90,0 ,0]) cylinder(r=R,$fn=fn_even,center=true);
                if (axis=="z")   rotate([0 ,0 ,0]) cylinder(r=R,$fn=fn_even,center=true);
                if (axis=="xyz") sphere(r=R,$fn=fn_even);
            }
        }
        if (axis=="x")   rotate([0 ,90,0]) cylinder(r=R,$fn=fn,center=true);
        if (axis=="y")   rotate([90,0 ,0]) cylinder(r=R,$fn=fn,center=true);
        if (axis=="z")   rotate([0 ,0 ,0]) cylinder(r=R,$fn=fn,center=true);
        if (axis=="xyz") sphere(r=R,$fn=fn_even);      
    }
}

//
//
//

add_fillets(R=10,OB=1000,axis="z",fn=6)
    spinner();