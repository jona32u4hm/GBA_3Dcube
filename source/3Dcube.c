
#include <stdio.h>
#include <tonc.h>

#define VIEW_W 160
#define VIEW_H 128

#define CX (VIEW_W/2)
#define CY (VIEW_H/2)

u16 r_x = 0x4000;
u16 r_y = 0x4000;
u16 r_z = 0x4000;


const int d = 5;

const VECTOR cube_vertices[] ALIGN4 = {
    {-d,-d,-d},
    { d,-d,-d},
    { d, d,-d},
    {-d, d,-d},
    {-d,-d, d},
    { d,-d, d},
    { d, d, d},
    {-d, d, d}
};



POINT p2D[8];

int main()
{
    REG_DISPCNT = DCNT_MODE5 | DCNT_BG2;
    REG_BG2CNT = BG_CBB(0) | BG_SBB(0) | BG_AFF_32x32;
    REG_BG2PA = 256;  // Set X-scaling to 1.0 (8.8 fixed point)
    REG_BG2PD = 256;  // Set Y-scaling to 1.0 (8.8 fixed point)
    REG_BG2X = -40 << 8;  // Shifts the viewport 40 pixels right
    REG_BG2Y = -16 << 8;  // Shifts the viewport 16 pixels down

    irq_init(NULL);
    irq_add(II_VBLANK, NULL);

    vid_page = vid_mem;

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


        const VECTOR* v3D = cube_vertices;


        POINT* v2D = p2D;


        for (int i = 0; i < 8; i++) 
        {
            // current point 

            VECTOR c = *v3D++;

            // rotate
            FIXED rotated_x = ((c.x * u.x) + (c.y * v.x) + (c.z * w.x)) >> 8;
            FIXED rotated_y = ((c.x * u.y) + (c.y * v.y) + (c.z * w.y)) >> 8;
            FIXED rotated_z = ((c.x * u.z) + (c.y * v.z) + (c.z * w.z)) >> 8;
            // all final rotations are in .4 format


            int z_awayfromcam = (100) - (rotated_z >> 4);;

            // apply perspective

            //    int           .4 -> int * .16
            int proj_x = (((rotated_x*700) )* (s32)lu_div(z_awayfromcam) )>> 20;
            int proj_y = (((rotated_y*700) )* (s32)lu_div(z_awayfromcam) )>> 20;

            v2D->x = CX - proj_x;
            v2D->y = CY + proj_y;
            v2D++;
        }

        VBlankIntrWait();

        m5_fill(CLR_BLACK);

        // --- BUILD CUBE ---
        // Front Face (Vertices 0, 1, 2, 3)
        m5_line(p2D[0].x, p2D[0].y, p2D[1].x, p2D[1].y, CLR_MAG);
        m5_line(p2D[1].x, p2D[1].y, p2D[2].x, p2D[2].y, CLR_MAG);
        m5_line(p2D[2].x, p2D[2].y, p2D[3].x, p2D[3].y, CLR_MAG);
        m5_line(p2D[3].x, p2D[3].y, p2D[0].x, p2D[0].y, CLR_MAG);

        // Back Face (Vertices 4, 5, 6, 7)
        m5_line(p2D[4].x, p2D[4].y, p2D[5].x, p2D[5].y, CLR_CYAN);
        m5_line(p2D[5].x, p2D[5].y, p2D[6].x, p2D[6].y, CLR_CYAN);
        m5_line(p2D[6].x, p2D[6].y, p2D[7].x, p2D[7].y, CLR_CYAN);
        m5_line(p2D[7].x, p2D[7].y, p2D[4].x, p2D[4].y, CLR_CYAN);

        // Interconnecting Lines (Front to Back)
        m5_line(p2D[0].x, p2D[0].y, p2D[4].x, p2D[4].y, CLR_RED);
        m5_line(p2D[1].x, p2D[1].y, p2D[5].x, p2D[5].y, CLR_RED);
        m5_line(p2D[2].x, p2D[2].y, p2D[6].x, p2D[6].y, CLR_RED);
        m5_line(p2D[3].x, p2D[3].y, p2D[7].x, p2D[7].y, CLR_RED);
        vid_flip();
    }
    return 0;
}