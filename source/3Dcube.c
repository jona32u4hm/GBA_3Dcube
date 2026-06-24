
#include <stdio.h>
#include <tonc.h>


u16 r_x = 0x4000;
u16 r_y = 0x4000;
u16 r_z = 0x4000;


const int d = 7;
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

    m3_fill(CLR_BLUE);
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

        for (int i = 0; i < 8; i++) 
        {
            // x rot:
            FIXED xrot_y = ((cosX * y3D[i]) >> 8) - ((sinX * z3D[i]) >> 8);
            FIXED xrot_z = ((sinX * y3D[i]) >> 8) + ((cosX * z3D[i]) >> 8);

            // y rot:
            FIXED yrot_x = ((cosY * x3D[i]) >> 8) + ((sinY * xrot_z) >> 12);
            FIXED yrot_z = ((-sinY * x3D[i]) >> 8) + ((cosY * xrot_z) >> 12);

            // z rot:
            FIXED zrot_x = ((cosZ * yrot_x) >> 12) - ((sinZ * xrot_y) >> 12);
            FIXED zrot_y = ((sinZ * yrot_x) >> 12) + ((cosZ * xrot_y) >> 12);
            int z_awayfromcam = (200) - (yrot_z >> 4);;

            // apply perspective
            // all final rotations are in .4 format

            //    int           .4 -> int * .16
            FIXED proj_x = (((zrot_x*700) >> 4)* (s32)lu_div(z_awayfromcam) )>> 16;
            FIXED proj_y = (((zrot_y*700) >> 4)* (s32)lu_div(z_awayfromcam) )>> 16;
            x2D[i] = 120 - (proj_x );  // 240 / 2 pre-calculated
            y2D[i] = 80 + (proj_y );   // 160 / 2 pre-calculated
        }


		VBlankIntrWait();

        // Clear previous frame by drawing a black background rectangle over the rendering area
        m3_rect(40, 20, 200, 140, CLR_BLACK);



        // --- BUILD CUBE ---
        
        // Front Face (Vertices 0, 1, 2, 3)
        m3_line(x2D[0], y2D[0], x2D[1], y2D[1], CLR_CYAN);
        m3_line(x2D[1], y2D[1], x2D[2], y2D[2], CLR_CYAN);
        m3_line(x2D[2], y2D[2], x2D[3], y2D[3], CLR_CYAN);
        m3_line(x2D[3], y2D[3], x2D[0], y2D[0], CLR_CYAN);

        // Back Face (Vertices 4, 5, 6, 7)
        m3_line(x2D[4], y2D[4], x2D[5], y2D[5], CLR_RED);
        m3_line(x2D[5], y2D[5], x2D[6], y2D[6], CLR_RED);
        m3_line(x2D[6], y2D[6], x2D[7], y2D[7], CLR_RED);
        m3_line(x2D[7], y2D[7], x2D[4], y2D[4], CLR_RED);

        // Interconnecting Lines (Front to Back)
        m3_line(x2D[0], y2D[0], x2D[4], y2D[4], CLR_MAG);
        m3_line(x2D[1], y2D[1], x2D[5], y2D[5], CLR_MAG);
        m3_line(x2D[2], y2D[2], x2D[6], y2D[6], CLR_MAG);
        m3_line(x2D[3], y2D[3], x2D[7], y2D[7], CLR_MAG);
    }
    return 0;
}