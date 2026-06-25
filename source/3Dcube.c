
#include <stdio.h>
#include <tonc.h>


u16 r_x = 0x4000;
u16 r_y = 0x4000;
u16 r_z = 0x4000;


const int d = 10;
int x3D[] = { -d,  d,  d, -d, -d,  d,  d, -d };
int y3D[] = { -d, -d,  d,  d, -d, -d,  d,  d };
int z3D[] = { -d, -d, -d, -d,  d,  d,  d,  d };
FIXED x2D[] = {
    0, 0, 0, 0, 0, 0, 0, 0};
FIXED y2D[] = {
    0, 0, 0, 0, 0, 0, 0, 0};


int main()
{
    REG_DISPCNT = DCNT_MODE3 | DCNT_BG2;

    irq_init(NULL);
    irq_add(II_VBLANK, NULL);

    m3_rect(40, 20, 200, 140, CLR_BLACK);


    while(1)
	{
        r_x += 300;
        r_y += 300;
        r_z += 300;

        // Fetch Sin/Cos ONCE per frame instead of 8 times inside the loop!
        FIXED cosX = lu_cos(r_x), sinX = lu_sin(r_x);
        FIXED cosY = lu_cos(r_y), sinY = lu_sin(r_y);
        FIXED cosZ = lu_cos(r_z), sinZ = lu_sin(r_z);

        VECTOR u, v, w;

        // --- Basis Vector U (X-Axis Column) ---
        u.x = (cosY * cosZ) >> 12;
        u.y = (cosY * sinZ) >> 12;
        u.z = -sinY;
        // --- Basis Vector V (Y-Axis Column) ---
        v.x = (((sinX * sinY) >> 12) * cosZ >> 12) - ((cosX * sinZ) >> 12);
        v.y = (((sinX * sinY) >> 12) * sinZ >> 12) + ((cosX * cosZ) >> 12);
        v.z = (sinX * cosY) >> 12;
        // --- Basis Vector W (Z-Axis Column) ---
        w.x = (((cosX * sinY) >> 12) * cosZ >> 12) + ((sinX * sinZ) >> 12);
        w.y = (((cosX * sinY) >> 12) * sinZ >> 12) - ((sinX * cosZ) >> 12);
        w.z = (cosX * cosY) >> 12;

        for (int i = 0; i < 8; i++) 
        {
                
            FIXED rotated_x = ((x3D[i] * u.x) + (y3D[i] * v.x) + (z3D[i] * w.x)) >> 8;
            FIXED rotated_y = ((x3D[i] * u.y) + (y3D[i] * v.y) + (z3D[i] * w.y)) >> 8;
            FIXED rotated_z = ((x3D[i] * u.z) + (y3D[i] * v.z) + (z3D[i] * w.z)) >> 8;
            // all final rotations are in .4 format


            int z_awayfromcam = (200) - (rotated_z >> 4);;

            // apply perspective

            //    int           .4 -> int * .16
            FIXED proj_x = (((rotated_x*700) >> 4)* (s32)lu_div(z_awayfromcam) )>> 16;
            FIXED proj_y = (((rotated_y*700) >> 4)* (s32)lu_div(z_awayfromcam) )>> 16;
            x2D[i] = 120 - (proj_x );  
            y2D[i] = 80 + (proj_y );   
        }


		VBlankIntrWait();

        m3_fill(CLR_BLACK);



        // --- BUILD CUBE ---
        
        // Front Face (Vertices 0, 1, 2, 3)
        m3_line(x2D[0], y2D[0], x2D[1], y2D[1], CLR_MAG);
        m3_line(x2D[1], y2D[1], x2D[2], y2D[2], CLR_MAG);
        m3_line(x2D[2], y2D[2], x2D[3], y2D[3], CLR_MAG);
        m3_line(x2D[3], y2D[3], x2D[0], y2D[0], CLR_MAG);

        // Back Face (Vertices 4, 5, 6, 7)
        m3_line(x2D[4], y2D[4], x2D[5], y2D[5], CLR_CYAN);
        m3_line(x2D[5], y2D[5], x2D[6], y2D[6], CLR_CYAN);
        m3_line(x2D[6], y2D[6], x2D[7], y2D[7], CLR_CYAN);
        m3_line(x2D[7], y2D[7], x2D[4], y2D[4], CLR_CYAN);

        // Interconnecting Lines (Front to Back)
        m3_line(x2D[0], y2D[0], x2D[4], y2D[4], CLR_RED);
        m3_line(x2D[1], y2D[1], x2D[5], y2D[5], CLR_RED);
        m3_line(x2D[2], y2D[2], x2D[6], y2D[6], CLR_RED);
        m3_line(x2D[3], y2D[3], x2D[7], y2D[7], CLR_RED);
    }
    return 0;
}